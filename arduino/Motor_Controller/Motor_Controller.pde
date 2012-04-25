  int incomingByte;
  //motor 2
  int RIGHT_In1=5; //forward
  int RIGHT_In2=6; //reverse
  //motor 1
  int LEFT_In1=10;  //forward
  int LEFT_In2=11;  //reverse
  
  int LEFT_BIT;
  int RIGHT_BIT;
  int TIME;
  
  int S;
  
void setup() {                
  pinMode(RIGHT_In1, OUTPUT); //In1
  pinMode(RIGHT_In2, OUTPUT); //In2

  pinMode(LEFT_In1, OUTPUT); //In1
  pinMode(LEFT_In2, OUTPUT); //In2
  Serial.begin(19200);
}

void loop() {
  delay(200);
  if (Serial.available() > 0) {
      int inByte = Serial.read();
      //Serial.println(inByte >> 4);
      //Serial.println(inByte & 15);

      LEFT_BIT = (inByte >> 1) & 1;
      RIGHT_BIT = inByte & 1;
      TIME = ((inByte >> 2) & 63)*55;
      S = (inByte >> 7) & 1;
      
      if(S) {
        if(RIGHT_BIT==1){
          //right wheel backwards
          analogWrite(RIGHT_In1,0);
          analogWrite(RIGHT_In2,55);
          analogWrite(LEFT_In1,0);
          analogWrite(LEFT_In2,0);
          delay(1800);
          analogWrite(RIGHT_In1,0);
          analogWrite(RIGHT_In2,0);
          analogWrite(LEFT_In1,0);
          analogWrite(LEFT_In2,0);
          Serial.write(5);
        }
        else if(LEFT_BIT==1){
          //left wheel backwards
          analogWrite(RIGHT_In1,0);
          analogWrite(RIGHT_In2,0);
          analogWrite(LEFT_In1,0);
          analogWrite(LEFT_In2,55);
          delay(1800);
          analogWrite(RIGHT_In1,0);
          analogWrite(RIGHT_In2,0);
          analogWrite(LEFT_In1,0);
          analogWrite(LEFT_In2,0);
          Serial.write(5);
        }
      }
      //////
      else if(TIME == 0){
        analogWrite(RIGHT_In1,0);
        analogWrite(RIGHT_In2,0);
        analogWrite(LEFT_In1,0);
        analogWrite(LEFT_In2,0);
        Serial.write(5);
      }
      //move forward
      else if(RIGHT_BIT==0 && LEFT_BIT==0){
        analogWrite(RIGHT_In1,55);
        analogWrite(RIGHT_In2,0);
        analogWrite(LEFT_In1,55);
        analogWrite(LEFT_In2,0);
        delay(TIME);
        analogWrite(RIGHT_In1,0);
        analogWrite(RIGHT_In2,0);
        analogWrite(LEFT_In1,0);
        analogWrite(LEFT_In2,0);
        Serial.write(5);
      }
      //move backward
      else if(RIGHT_BIT==1 && LEFT_BIT==1){
        analogWrite(RIGHT_In1,0);
        analogWrite(RIGHT_In2,55);
        analogWrite(LEFT_In1,0);
        analogWrite(LEFT_In2,55);
        delay(TIME);
        analogWrite(RIGHT_In1,0);
        analogWrite(RIGHT_In2,0);
        analogWrite(LEFT_In1,0);
        analogWrite(LEFT_In2,0);
        Serial.write(5);
      }
      //ROR
      else if(RIGHT_BIT==1 && LEFT_BIT==0){
        analogWrite(RIGHT_In1,0);
        analogWrite(RIGHT_In2,55);
        analogWrite(LEFT_In1,55);
        analogWrite(LEFT_In2,0);
        delay(TIME);
        analogWrite(RIGHT_In1,0);
        analogWrite(RIGHT_In2,0);
        analogWrite(LEFT_In1,0);
        analogWrite(LEFT_In2,0);
        Serial.write(5);
      }
      //ROL
      else if(RIGHT_BIT==0 && LEFT_BIT==1){
        analogWrite(RIGHT_In1,55);
        analogWrite(RIGHT_In2,0);
        analogWrite(LEFT_In1,0);
        analogWrite(LEFT_In2,55);
        delay(TIME);
        analogWrite(RIGHT_In1,0);
        analogWrite(RIGHT_In2,0);
        analogWrite(LEFT_In1,0);
        analogWrite(LEFT_In2,0);
        Serial.write(5);
      }
      else{
        analogWrite(RIGHT_In1,0);
        analogWrite(RIGHT_In2,0);
        analogWrite(LEFT_In1,0);
        analogWrite(LEFT_In2,0);
        Serial.write(5);
      }
  }
}
