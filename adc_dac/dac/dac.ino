// dac引脚只有2个8bit通道。分别是25，26
#define LED 25

void setup(){
    Serial.begin(9600);
}

void loop(){
    if (Serial.available() > 0) {
        // 读取完整的数字，而不是单个字符
        int data = Serial.parseInt(); 
        
        // 限制范围在 0~255 之间
        if (data < 0) data = 0;
        if (data > 255) data = 255;
        
        // 输出到 DAC
        dacWrite(LED, data);
        
        // 打印反馈
        Serial.print("DAC 输出设置为: ");
        Serial.println(data);
        
        // 清除串口缓冲区中可能残留的换行符等
        while (Serial.available() > 0) {
            Serial.read(); 
        }
    }
    
    // 注意：这里不需要 delay(1000)
    // 因为我们要让 DAC 一直保持上次的值，而不是每秒刷新
}