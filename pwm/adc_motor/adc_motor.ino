#define WINDOW_SIZE 10
#define THRESHOLD 20 // 触发阈值

const int adcPin = 32; //adc 引脚，默认12位

// 滑动平均滤波所需的数组
int adcBuffer[WINDOW_SIZE] = {0};
int bufferIndex = 0;
int sum = 0;

// 核心变量：记录开机时的初始基准值（永不改变）
int middleValue = 0; 
int lastValue = 0; 

// 定义电调信号引脚
const int escPin = 23;
const int ledcChannel = 0;

// 电调控制参数
const int freq = 50;          // 电调标准控制频率：50Hz
const int resolution = 12;    // 12位分辨率，最大值 4095，周期为 20ms

// 脉宽对应的占空比计算 (公式: 目标脉宽us / 20000us * 4095)
const int dutyMin = 205;      // 最低油门 (1ms)：用于解锁电调
const int dutyMax = 410;      // 最大油门 (2.0ms)：全速转动  // 中立油门 (1.5ms)
float dutyUnit;

void setup() {
  Serial.begin(9600); // 建议提高波特率，加快串口传输
  
  // 1. 初始化滑动平均数组
  int initialVal = analogRead(adcPin);
  for (int i = 0; i < WINDOW_SIZE; i++) {
    adcBuffer[i] = initialVal;
  }
  sum = initialVal * WINDOW_SIZE;
  
  // 2. 将开机时的状态设为绝对中立点
  middleValue = initialVal;
  lastValue = initialVal;
  Serial.print("System Initialized. Center Value: ");
  Serial.println(middleValue);
  // 计算单位变化对应的占空比增量
  dutyUnit = (float)(dutyMax - dutyMin) /  (4095-middleValue);

  // 初始化 PWM 通道并绑定引脚，2.x库的使用方法
  // ledcSetup(ledcChannel, freq, resolution);
  // ledcAttachPin(escPin, ledcChannel);
  // 3.x库的使用方法
  ledcAttachChannel(escPin, freq, resolution, ledcChannel);

  // 发送最低油门信号以解锁电调
  Serial.println("正在发送最低油门，解锁电调...");
  ledcWrite(ledcChannel, dutyMin);
  
  // 等待电调完成自检（通常伴随滴滴滴的提示音，需等待 3 秒左右）
  Serial.println("等待电调自检...");
  delay(5000); 
  Serial.println("电调自检完成，准备启动！");
}

void loop() {
  // 1. 高效滑动平均滤波（每次只读1次）
  int current = analogRead(adcPin);
  sum = sum - adcBuffer[bufferIndex] + current;
  adcBuffer[bufferIndex] = current;
  bufferIndex++;
  if (bufferIndex >= WINDOW_SIZE) bufferIndex = 0;
  
  current = sum / WINDOW_SIZE;
  
  // 2. 计算与初始基准的差值
  int midDiff = current - middleValue;
  int lastDiff = abs(current - lastValue);
  
  // 3. 判断是否超过阈值
  if (midDiff >= THRESHOLD) {
    int dutyNow = dutyMin + midDiff * dutyUnit;
    if (dutyNow > dutyMax) dutyNow = dutyMax;
    // ledcWrite(ledcChannel, dutyNow);//2.x库的用法
    ledcWrite(escPin, dutyNow);//3.x 库的使用方法
    // Serial.printf("now is: %d\n",dutyNow);
  } 
  else {
    // ledcWrite(ledcChannel, dutyMin);
    ledcWrite(escPin, dutyMin);
    // Serial.println("min duty");
    
  }
  lastValue = current;
  delay(10); 
}