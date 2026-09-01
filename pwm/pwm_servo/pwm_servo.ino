// 定义舵机信号引脚
const int escPin = 23;
const int ledcChannel = 0;

// 电调控制参数
const int freq = 50;          // 电调标准控制频率：50Hz，周期1s/50=20ms
const int resolution = 12;    // 12位分辨率，最大值 4095，对应周期为 20ms

// 脉宽对应的占空比计算 (公式: 目标脉宽ms / 20ms * 4095)
const int duty0 = 102;      // 0.5ms, 0
const int duty1 = 205;      // 1ms, 45
const int duty15 = 307;  // 1.5ms, 90
const int duty2 = 409;  // 2ms, 135
const int duty25 = 512;  // 2.5ms, 180
int duty[] = {102, 205, 307, 409, 512};


void setup() {
  Serial.begin(9600);

  // 1. 初始化 PWM 通道并绑定引脚
  ledcSetup(ledcChannel, freq, resolution);
  ledcAttachPin(escPin, ledcChannel);
}

void loop() {
  for (int i=0;i<5;i++){
    ledcWrite(ledcChannel, duty[i]);
    Serial.printf("now duty: %d\n", duty[i]);
    delay(1000);
  }
  for (int i=3;i>0;i--){
    ledcWrite(ledcChannel, duty[i]);
    Serial.printf("now duty: %d\n", duty[i]);
    delay(1000);
  }
}