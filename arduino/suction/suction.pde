
int suctionSensor=7;
int ledPin=13;
int button=12;
int solenoid_2=11;
int solenoid_1=10;
int fiveVolt = 9;
int STATE;

void setup(){
  Serial.begin(9600);
  pinMode(fiveVolt, OUTPUT);
  digitalWrite(fiveVolt, HIGH);
  pinMode(ledPin,OUTPUT);
  pinMode(suctionSensor,INPUT); 
  pinMode(button,INPUT);
  pinMode(solenoid_2, OUTPUT);
  pinMode(solenoid_1, OUTPUT);
  STATE=0;
}

void loop(){
  switch(STATE){
    case 0: 
      SUCTION_OFF();
      break;
    case 1:
      SUCTION_ON();
      break;
    case 2: 
      DO_ACTION();
      break;
    default:
      Serial.println("Error");    
  }
}

void SUCTION_OFF(){
  //turn solenoid 1 on for .5 sec
  digitalWrite(solenoid_1,HIGH);
  delay(1000);
  digitalWrite(solenoid_1, LOW);
  while(!digitalRead(button)){
   Serial.println("Suction off"); 
  } //Make sure that the button unpressed means HIGH  
  
  STATE=1;
}
void SUCTION_ON(){
  //turn solenoid 2 on for .5 sec
  digitalWrite(solenoid_2,HIGH);
  delay(1000);
  digitalWrite(solenoid_2, LOW);
  while(!digitalRead(suctionSensor)){
   Serial.println("Suction on"); 
  }
  STATE=2;
}
void DO_ACTION(){
  digitalWrite(ledPin, HIGH);
  while(!digitalRead(button)){
    Serial.println("Action"); 
  }
  digitalWrite(ledPin,LOW);
  STATE=0;
}
