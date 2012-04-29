  int incomingByte;
  
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
  
  
  int foot_1 = ((B01000000>>2)&63)*200;
  int deg_90 = ((B00110000>>2)&63)*200;
  
  
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
      
      switch(inByte){
        case 0:
          //actuate();
          Serial.write(5);
          break;
        case 1:
        case 2:
          forward(foot_1);
          Serial.write(5);
          break;
        case 3:
          backward(foot_1/2);
          ROR(deg_90);
          backward(foot_1/2);
          Serial.write(5);
          break;
        case 4:
          ROR(deg_90);
          forward(foot_1);
          ROR(deg_90);
          Serial.write(5);
          break;
        case 5:
          backward(foot_1);
          ROR(deg_90);
          Serial.write(5);
          break;
        case 6:
          forward(foot_1);
          Serial.write(5);
          break;
        case 7:
          ROR(deg_90);
          forward(foot_1/2);
          ROL(deg_90);
          backward(foot_1*3/2);
          Serial.write(5);
          break;
        default:
          Serial.println("BROKEN");
          break;
      }
  }
}

void forward(int t)
{      //move forward
  //else if(RIGHT_BIT==0 && LEFT_BIT==0){
    digitalWrite(in_dir, HIGH);
    digitalWrite(out_dir, HIGH);
    analogWrite(in_speed,60);
    analogWrite(out_speed,60);
    delay(t);
    analogWrite(in_speed, 0);
    analogWrite(out_speed, 0);
    //Serial.write(5);
  //}
}

void backward(int t)
{      //move backward
  //else if(RIGHT_BIT==1 && LEFT_BIT==1){
    digitalWrite(in_dir, LOW);
    digitalWrite(out_dir, LOW);
    analogWrite(in_speed,60);
    analogWrite(out_speed,60);
    delay(t);
    analogWrite(in_speed, 0);
    analogWrite(out_speed, 0);
    //Serial.write(5);
  //}
}

void ROR(int t)
{      //ROR
  //else if(RIGHT_BIT==1 && LEFT_BIT==0){
    digitalWrite(in_dir, LOW);
    digitalWrite(out_dir, HIGH);
    analogWrite(in_speed,80);
    analogWrite(out_speed,80);
    delay(t);
    analogWrite(in_speed, 0);
    analogWrite(out_speed, 0);
    //Serial.write(5);
  //}
}

void ROL(int t)
{      //ROL
  //else if(RIGHT_BIT==0 && LEFT_BIT==1){
    digitalWrite(in_dir, HIGH);
    digitalWrite(out_dir, LOW);
    analogWrite(in_speed,80);
    analogWrite(out_speed,80);
    delay(TIME);
    analogWrite(in_speed, 0);
    analogWrite(out_speed, 0);
    //Serial.write(5);
  //}
}
