String incomingByte;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  Serial2.begin(9600);
}

void loop() {
  // put your main code here, to run repeatedly:
  if (Serial2.available()) {
    incomingByte = Serial2.readStringUntil('\n');
    Serial.print("millis: ");
    Serial.println(incomingByte);
  }
}