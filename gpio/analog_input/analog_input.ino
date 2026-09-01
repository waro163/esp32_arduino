#define led 2
int bright;
void setup() {
  // put your setup code here, to run once:
  pinMode(led, INPUT);
}

void loop() {
  // put your main code here, to run repeatedly:
  bright = analogRead(led);

  delay(10);
}
