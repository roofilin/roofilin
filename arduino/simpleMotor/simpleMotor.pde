int r1 = 5;
int r2 = 6;
int l1 = 10;
int l2 = 11;

void setup() {
  /*
  pinMode(l1, OUTPUT);
  pinMode(l2, OUTPUT);
  pinMode(r1, OUTPUT);
  pinMode(r2, OUTPUT);
  
  analogWrite(l1, 55);
  analogWrite(l2, 0);
  analogWrite(r1, 55);
  analogWrite(r2, 0);
  delay(1800);
  
  analogWrite(l1, 30);
  analogWrite(l2, 0);
  analogWrite(r1, 30);
  analogWrite(r2, 0);
  delay(200);
  
  
  analogWrite(l1, 0);
  analogWrite(l2, 0);
  analogWrite(r1, 0);
  analogWrite(r2, 0);
  delay(100);
  */
  analogWrite(l1, 0);
  analogWrite(l2, 155);
  analogWrite(r1, 0);
  analogWrite(r2, 155);
  delay(1800);
  analogWrite(l1, 0);
  analogWrite(l2, 30);
  analogWrite(r1, 0);
  analogWrite(r2, 30);
  delay(200);
  
}

void loop() {
  analogWrite(l1, 0);
  analogWrite(l2, 0);
  analogWrite(r1, 0);
  analogWrite(r2, 0);
  
  
}
