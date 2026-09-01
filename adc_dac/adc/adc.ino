/*
adc 共有adc1和adc2，共有18个通道，对应不同的管脚
12位精度，0-4095
*/
#define inpin 32
int currentInput = 0;

void setup(){
    // 无需设置输入模式即可
    // pinMode(inpin, INPUT);
     Serial.begin(9600);
}

void loop(){
    int data = analogRead(inpin);
    if (abs(currentInput-data)>=20){
        Serial.println(data);
        currentInput = data;
    }
}