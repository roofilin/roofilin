
int r_dir= 7;    //OUT WHEEL
int r_speed = 6;
int l_dir = 4;   //IN WHEEL
int l_speed = 5;

//HIGH = FORWARD

void setup() {
  
  pinMode(l_dir, OUTPUT);
  pinMode(l_speed, OUTPUT);
  pinMode(r_dir, OUTPUT);
  pinMode(r_speed, OUTPUT);
  /*
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
  /*
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
  */
  digitalWrite(l_dir, LOW);
  analogWrite(l_speed, 55);
  digitalWrite(r_dir, HIGH);
  analogWrite(r_speed, 55);
  delay(1000);
}

void loop() {
  analogWrite(l_speed, 0);
  analogWrite(r_speed, 0);
  
}
