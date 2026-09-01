#define inpin 32
#define WINDOW_SIZE 10
#define THRESHOLD 20 // 触发阈值

// 滑动平均滤波所需的数组
int adcBuffer[WINDOW_SIZE] = {0};
int bufferIndex = 0;
int sum = 0;

// 核心变量：记录开机时的初始基准值（永不改变）
int middleValue = 0; 

int lastValue = 0; 

void setup() {
  Serial.begin(9600); // 建议提高波特率，加快串口传输
  
  // 1. 初始化滑动平均数组
  int initialVal = analogRead(inpin);
  for (int i = 0; i < WINDOW_SIZE; i++) {
    adcBuffer[i] = initialVal;
  }
  sum = initialVal * WINDOW_SIZE;
  
  // 2. 将开机时的状态设为绝对中立点
  middleValue = initialVal;
  lastValue = initialVal;
  Serial.print("System Initialized. Center Value: ");
  Serial.println(middleValue);
}

void loop() {
  // 1. 高效滑动平均滤波（每次只读1次）
  int current = analogRead(inpin);
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
    if (lastDiff>=THRESHOLD){
      Serial.print("larger: ");
      Serial.println(current);
    }
  } 
  else if (midDiff <= -THRESHOLD) {
    if (lastDiff>=THRESHOLD){
      Serial.print("less: ");
      Serial.println(current);
    }
  }
  lastValue = current;
  delay(10); 
}