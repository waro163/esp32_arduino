const int frequency = 1000;// pwm频率1khz，最大40Mhz
const int channel = 1; //pwm通道0-15
const int bit1 = 12; // pwm解析位数(8位或者12位)
const int led = 2;
int x = 0;// led亮度

void setup(){
    // Initialize channels
    // channels 0-15, resolution 1-16 bits, freq limits depend on resolution
    // ledcSetup(uint8_t channel, uint32_t freq, uint8_t resolution_bits);
    ledcSetup(channel, frequency, bit1);
    ledcAttachPin(led, channel);//分配gpio引脚到通道
    // Serial.begin(9600)
}

void loop(){
    for (x = 0; x<4096;x++){
        ledcWrite(channel, x);
        delay(1);
    }
}