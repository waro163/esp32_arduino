#define WINDOW_SIZE 10
#define THRESHOLD 50          // 空闲死区，低于此差值保持最低油门
#define CALIBRATE_DISCARD 20  // 丢掉上电后不稳定的前几次采样
#define CALIBRATE_SAMPLES 32  // 基准值取多次平均
#define MIN_TRAVEL 200        // 有效行程下限，避免分母过小导致过敏

const int adcPin = 32; //adc 引脚，默认12位

// 滑动平均滤波所需的数组
int adcBuffer[WINDOW_SIZE] = {0};
int bufferIndex = 0;
int sum = 0;

// 开机标定的零油门基准（运行中不再改变）
int middleValue = 0;
int lastDuty = -1;

// 定义电调信号引脚
const int escPin = 23;
const int ledcChannel = 0;

// 电调控制参数
const int freq = 50;          // 电调标准控制频率：50Hz
const int resolution = 12;    // 12位分辨率，最大值 4095，周期为 20ms

// 脉宽对应的占空比计算 (公式: 目标脉宽us / 20000us * 4095)
const int dutyMin = 205;      // 最低油门 (1.0ms)：用于解锁电调
const int dutyMax = 410;      // 最大油门 (2.0ms)：全速转动
float dutyUnit;

void setup() {
  Serial.begin(115200);
  analogSetPinAttenuation(adcPin, ADC_11db); // 约 0–3.3V 量程

  // 1. 丢掉上电后不稳定的 ADC 读数
  for (int i = 0; i < CALIBRATE_DISCARD; i++) {
    analogRead(adcPin);
    delay(2);
  }

  // 2. 多次采样取平均，作为零油门基准
  long calibSum = 0;
  for (int i = 0; i < CALIBRATE_SAMPLES; i++) {
    calibSum += analogRead(adcPin);
    delay(2);
  }
  int initialVal = calibSum / CALIBRATE_SAMPLES;

  for (int i = 0; i < WINDOW_SIZE; i++) {
    adcBuffer[i] = initialVal;
  }
  sum = initialVal * WINDOW_SIZE;
  middleValue = initialVal;

  Serial.print("System Initialized. Center Value: ");
  Serial.println(middleValue);

  // 有效行程过短时用下限，避免除零或油门过敏
  int travel = 4095 - middleValue;
  if (travel < MIN_TRAVEL) {
    Serial.println("Warning: ADC near full scale at boot, throttle mapping limited.");
    travel = MIN_TRAVEL;
  }
  dutyUnit = (float)(dutyMax - dutyMin) / travel;

  // 初始化 PWM 通道并绑定引脚，2.x库的使用方法
  // ledcSetup(ledcChannel, freq, resolution);
  // ledcAttachPin(escPin, ledcChannel);
  // 3.x库的使用方法
  ledcAttachChannel(escPin, freq, resolution, ledcChannel);

  // 发送最低油门信号以解锁电调
  Serial.println("正在发送最低油门，解锁电调...");
  ledcWrite(ledcChannel, dutyMin);
  lastDuty = dutyMin;

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

  // 2. 相对开机基准的增量；低于死区则保持停转
  int midDiff = current - middleValue;
  int dutyNow = dutyMin;
  if (midDiff >= THRESHOLD) {
    dutyNow = dutyMin + (int)(midDiff * dutyUnit + 0.5f);
    if (dutyNow > dutyMax) dutyNow = dutyMax;
  }

  if (dutyNow != lastDuty) {
    // ledcWrite(ledcChannel, dutyNow);//2.x库的用法
    ledcWrite(escPin, dutyNow);//3.x 库的使用方法
    lastDuty = dutyNow;
  }

  delay(10);
}
