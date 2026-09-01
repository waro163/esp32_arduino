#define inpin 32

// 1. 定义滑动窗口的大小（这里取 10 次采样求平均，数值越大越平滑，但响应越迟钝）
#define WINDOW_SIZE 10

// 2. 定义一个数组来存储历史采样值，并初始化为 0
int adcBuffer[WINDOW_SIZE] = {0};

// 3. 记录当前数组写到了哪个位置（索引）
int bufferIndex = 0;

// 4. 记录上一次的平滑值，用于触发串口输出
int lastSmoothedValue = 0;

void setup() {
    Serial.begin(9600);
    
    // 初始化：将缓冲区全部填充为初始读取值，防止刚开机时数据从 0 突变
    int initialVal = analogRead(inpin);
    for (int i = 0; i < WINDOW_SIZE; i++) {
        adcBuffer[i] = initialVal;
    }
    lastSmoothedValue = initialVal;
}

void loop() {
    // 1. 读取当前 ADC 值
    int rawValue = analogRead(inpin);
    
    // 2. 将新值放入缓冲区（覆盖最旧的值）
    adcBuffer[bufferIndex] = rawValue;
    bufferIndex++;
    if (bufferIndex >= WINDOW_SIZE) {
        bufferIndex = 0; // 索引循环使用
    }
    
    // 3. 计算缓冲区里所有数据的总和
    long sum = 0;
    for (int i = 0; i < WINDOW_SIZE; i++) {
        sum += adcBuffer[i];
    }
    
    // 4. 计算滑动平均值
    int smoothedValue = sum / WINDOW_SIZE;
    
    // 5. 平滑后的值变化超过 20 才输出
    // 滤波 + 死区 = 极其稳定且不会刷屏的串口输出
    if (abs(smoothedValue - lastSmoothedValue) >= 20) {
        Serial.print("Raw: ");
        Serial.print(rawValue);
        Serial.print(" | Smoothed: ");
        Serial.println(smoothedValue);
        
        lastSmoothedValue = smoothedValue;
    }
    
    // 稍微延时 10ms，控制采样频率（100Hz），避免 CPU 满载
    delay(10); 
}