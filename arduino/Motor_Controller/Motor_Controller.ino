  int incomingByte;
  //motor 2
  //int RIGHT_In1=5; //forward
  //int RIGHT_In2=6; //reverse
  //motor 1
  //int LEFT_In1=10;  //forward
  //int LEFT_In2=11;  //reverse
  
  //HIGH is forward, LOW backward. check LEFT/RIGHT
  /*
  int RIGHT_DIR = 4;
  int RIGHT_SPEED = 5;
  int LEFT_DIR = 7;
  int LEFT_SPEED = 6;
  */
  
  //right = inner, left = outer
  //outer wheel
  int out_dir = 5;
  int out_speed = 4;
  
  //inner wheel
  int in_dir = 2;
  int in_speed = 3;
  
  
  int LEFT_BIT;
  int RIGHT_BIT;
  int TIME;
  
  int S;
  
void setup() {              
  pinMode(28, OUTPUT);      
  pinMode(30, OUTPUT);      
  pinMode(32, OUTPUT);      
  pinMode(34, OUTPUT);      
  pinMode(36, OUTPUT);  
  digitalWrite(28, LOW);
  digitalWrite(30, LOW);
  digitalWrite(32, LOW);
  digitalWrite(34, LOW);
  digitalWrite(36, LOW);
  pinMode(in_dir, OUTPUT); //In1
  pinMode(in_speed, OUTPUT); //In2
  pinMode(out_dir, OUTPUT); //In1
  pinMode(out_speed, OUTPUT); //In2
  Serial.begin(19200);
}

void loop() {
  delay(200);
  if (Serial.available() > 0) {
      int inByte = Serial.read();

      LEFT_BIT = (inByte >> 1) & 1;
      RIGHT_BIT = inByte & 1;
      TIME = ((inByte >> 2) & 63)*200;
      S = (inByte >> 7) & 1;
      
      if(S) {
        if(RIGHT_BIT==1){
          //right wheel backwards
          digitalWrite(in_dir, LOW);
          digitalWrite(out_dir, LOW);
          analogWrite(in_speed,60);
          analogWrite(out_speed,0);
          delay(1800);
          analogWrite(in_speed, 0);
          analogWrite(out_speed, 0);
          Serial.write(5);
        }
        else if(LEFT_BIT==1){
          //left wheel backwards
          digitalWrite(in_dir, LOW);
          digitalWrite(out_dir, LOW);
          analogWrite(in_speed,0);
          analogWrite(out_speed,60);
          delay(1800);
          analogWrite(in_speed, 0);
          analogWrite(out_speed, 0);
          Serial.write(5);
        }
        else{
          Serial.write(55);
        }
      }
      //////
      else if(TIME == 0){
        analogWrite(in_speed, 0);
        analogWrite(out_speed, 0);
        digitalWrite(in_dir, LOW);
        digitalWrite(out_dir, LOW);
        Serial.write(5);
      }
      //move forward
      else if(RIGHT_BIT==0 && LEFT_BIT==0){
        digitalWrite(in_dir, HIGH);
        digitalWrite(out_dir, HIGH);
        analogWrite(in_speed,60);
        analogWrite(out_speed,60);
        delay(TIME);
        analogWrite(in_speed, 0);
        analogWrite(out_speed, 0);
        Serial.write(5);
      }
      //move backward
      else if(RIGHT_BIT==1 && LEFT_BIT==1){
        digitalWrite(in_dir, LOW);
        digitalWrite(out_dir, LOW);
        analogWrite(in_speed,60);
        analogWrite(out_speed,60);
        delay(TIME);
        analogWrite(in_speed, 0);
        analogWrite(out_speed, 0);
        Serial.write(5);
      }
      //ROR
      else if(RIGHT_BIT==1 && LEFT_BIT==0){
        digitalWrite(in_dir, LOW);
        digitalWrite(out_dir, HIGH);
        analogWrite(in_speed,80);
        analogWrite(out_speed,80);
        delay(TIME);
        analogWrite(in_speed, 0);
        analogWrite(out_speed, 0);
        Serial.write(5);
      }
      //ROL
      else if(RIGHT_BIT==0 && LEFT_BIT==1){
        digitalWrite(in_dir, HIGH);
        digitalWrite(out_dir, LOW);
        analogWrite(in_speed,80);
        analogWrite(out_speed,80);
        delay(TIME);
        analogWrite(in_speed, 0);
        analogWrite(out_speed, 0);
        Serial.write(5);
      }
      else{
        digitalWrite(in_dir, LOW);
        digitalWrite(out_dir, LOW);
        analogWrite(in_speed,0);
        analogWrite(out_speed,0);
        Serial.write(55);
      }
  }
}
