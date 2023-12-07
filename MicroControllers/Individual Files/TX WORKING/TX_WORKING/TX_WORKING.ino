uint32_t millistime;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  Serial1.begin(9600);
}

void loop() {
  // put your main code here, to run repeatedly:
  millistime = millis() / 1000;
  Serial.println(millistime);
  Serial1.println(millistime);
  delay(1000);
}
