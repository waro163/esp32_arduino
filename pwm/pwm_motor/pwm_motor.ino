// 定义电调信号引脚
const int escPin = 23;
const int ledcChannel = 0;

// 电调控制参数
const int freq = 50;          // 电调标准控制频率：50Hz
const int resolution = 12;    // 12位分辨率，最大值 4095，周期为 20ms

// 脉宽对应的占空比计算 (公式: 目标脉宽us / 20000us * 4095)
const int dutyMin = 205;      // 最低油门 (1ms)：用于解锁电调
const int dutyMax = 410;      // 最大油门 (2.0ms)：全速转动
const int dutyNeutral = 204;  // 中立油门 (1.5ms)：部分电调的停止状态

void setup() {
  Serial.begin(9600);

  // 1. 初始化 PWM 通道并绑定引脚
  ledcSetup(ledcChannel, freq, resolution);
  ledcAttachPin(escPin, ledcChannel);

  // 2. 发送最低油门信号以解锁电调
  Serial.println("正在发送最低油门，解锁电调...");
  ledcWrite(ledcChannel, dutyMin);
  
  // 3. 等待电调完成自检（通常伴随滴滴滴的提示音，需等待 3 秒左右）
  Serial.println("等待电调自检...");
  delay(5000); 
  Serial.println("电调自检完成，准备启动！");
}

void loop() {
  // 4. 平滑加速到全速，避免电流过大或电调保护
  Serial.println("逐渐加速至全速...");
  for (int duty = dutyMin; duty <= dutyMax; duty += 5) {
    ledcWrite(ledcChannel, duty);
    delay(20); // 每次增加一点占空比，给电机响应时间
  }

  Serial.println("已达到全速转动！");
  
  // 保持全速转动，直到断电
  while (true) {
    delay(1000);
  }
}