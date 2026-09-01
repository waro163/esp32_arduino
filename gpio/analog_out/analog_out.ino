
#include <analogWrite.h>

#define led 2
int brightness = 0;
int brightStep = 1;

void setup() {
  // put your setup code here, to run once:
  pinMode(led, OUTPUT);
}

void loop() {
  // put your main code here, to run repeatedly:
  brightness += brightStep;
  if ( brightness == 0 || brightness == 255 ) {
    brightStep = -brightStep;
  }

  analogWrite(led, brightness);

  delay(10);
}
