// 定义引脚（假设按键接在 GPIO 0，板载 LED 接在 GPIO 2）
#define BUTTON_PIN 32
#define LED_PIN 2

// 定义消抖时间阈值（毫秒）。机械按键通常需要 50ms~200ms，这里设为 200ms 确保绝对稳定
#define DEBOUNCE_DELAY 200 

// 1. 定义一个 volatile 变量作为中断标志位
volatile bool isr_flag = false;

// 2. 定义一个 volatile 变量记录上一次中断触发的时间（必须用 unsigned long）
volatile unsigned long last_interrupt_time = 0;

// 中断服务函数（ISR）
void ARDUINO_ISR_ATTR buttonISR() {
  // 获取当前系统运行时间
  unsigned long current_time = millis();
  
  // 核心消抖逻辑：只有当距离上次触发的时间超过了 DEBOUNCE_DELAY，才认为是一次有效按键
  if (current_time - last_interrupt_time > DEBOUNCE_DELAY) {
    isr_flag = true;
  }
  
  // 更新最后一次触发的时间
  last_interrupt_time = current_time;
}

void setup() {
  Serial.begin(9600);
  
  // 初始化 LED 引脚为输出
  pinMode(LED_PIN, OUTPUT);
  
  // 初始化按键引脚为输入，并启用内部上拉电阻（按键另一端需接地）
  // INPUT_PULLDOWN
  pinMode(BUTTON_PIN, INPUT_PULLUP);
  
  // 绑定外部中断，下降沿触发（按键按下瞬间电平从高变低）
  // RISING
  attachInterrupt(digitalPinToInterrupt(BUTTON_PIN), buttonISR, FALLING);
  
  Serial.println("带消抖的按键中断测试开始，请按下按键...");
}

void loop() {
  // 在主循环中检查标志位
  if (isr_flag) {
    // 执行业务逻辑：翻转 LED 状态
    digitalWrite(LED_PIN, !digitalRead(LED_PIN));
    
    // 打印日志
    Serial.println("按键有效触发，LED状态已切换！");
    
    // 处理完毕后，务必清除标志位，防止重复触发
    isr_flag = false; 
  }
}