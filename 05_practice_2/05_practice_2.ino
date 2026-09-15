int count;

void setup() {
  pinMode(7, OUTPUT);

}

void loop() {
  count = 0;
  digitalWrite(7, LOW);
  delay(1000);
  while (count != 5){
    digitalWrite(7, HIGH);
    delay(100);
    digitalWrite(7, LOW);
    delay(100);
    count++;
  }
  digitalWrite(7, HIGH);
  while (1) {}

}
