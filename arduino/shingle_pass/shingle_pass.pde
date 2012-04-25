//track state
int STATE;

//button and led pins
int ledPin = 13;
int button = 12;

//pins for drill actuation
int drill_A = 2; //A
int drill_B = 3; //B
int drill_en = 4; //C

//solenoid pins for cylinder actuation
int cylinder_D = 10; //D
int cylinder_E = 11; //E

//NOT SUPPORTED RIGHT NOW
/*
//solenoid pins for suction actuation
int suction_1 = 5;
int suction_2 = 6;
//suction sensor
int suctionSensor = 7;
*/

//pins for limit switches
int limit_in = 8; //F
int limit_out = 9; //G

void setup()
{
  Serial.begin(9600);
  
  STATE = 0;
  
  pinMode(ledPin, OUTPUT);
  pinMode(button, INPUT);
  pinMode(drill_A, OUTPUT);
  pinMode(drill_B, OUTPUT);
  pinMode(drill_en, OUTPUT);
  pinMode(cylinder_D, OUTPUT);
  pinMode(cylinder_E, OUTPUT);
  pinMode(limit_in, INPUT);
  pinMode(limit_out, INPUT);
}

void loop()
{
  switch(STATE){
    case 0:
      ALL_OFF();
      break;
    case 1:
      SUCTION_ON();
      break;
    case 2:
      LIFT_CYLINDER();
      break;
    case 3:
      DRILL_FORWARD();
      break;
    case 4:
      LOWER_CYLINDER();
      break;
    case 5:
      SUCTION_OFF();
      break;
    case 6:
      NAILING();  //5 second simulation
      break;
    case 7:
      LIFT_CYLINDER();
      break;
    case 8:
      DRILL_BACKWARD();
      break;
    case 9:
      STATE = 0;
      break;
    default:
      Serial.println("Error");    
  }
}

void ALL_OFF()
{
  Serial.println("ALL OFF");
  //placeholder for suction off
  digitalWrite(ledPin, LOW);
  //turn drill off
  digitalWrite(drill_en, LOW);
  digitalWrite(drill_A, HIGH);
  digitalWrite(drill_B, HIGH);
  //lower cylinder
  digitalWrite(cylinder_E, HIGH);
  delay(500);
  digitalWrite(cylinder_E, LOW);
  
  delay(5000);
  STATE+=1;
}

void SUCTION_ON()
{
  Serial.println("SUCTION ON");
  //placeholder for suction on
  digitalWrite(ledPin, HIGH);
  
  delay(2000);
  STATE+=1;
}

void LIFT_CYLINDER()
{
  Serial.println("LIFT CYLINDER");
  //lift cylinder
  digitalWrite(cylinder_D, HIGH);
  delay(500);
  digitalWrite(cylinder_D, LOW);
  
  delay(2000);
  STATE+=1;
}

void DRILL_FORWARD()
{
  //turn drill forward
  digitalWrite(drill_en, HIGH);
  digitalWrite(drill_A, HIGH);    //LOW/LOW = BACK?
  digitalWrite(drill_B, HIGH);
  
  while(!digitalRead(limit_out)){    //LIMIT SWITCH HIGH = HIT?
    Serial.println("DRILL FORWARD");
  }
  //turn drill off
  digitalWrite(drill_en, LOW);
  digitalWrite(drill_A, LOW);    //HIGH/HIGH = FRONT?
  digitalWrite(drill_B, LOW);
  STATE+=1;
}

void LOWER_CYLINDER()
{
  Serial.println("LOWER CYLINDER");
  //lower cylinder
  digitalWrite(cylinder_E, HIGH);
  delay(500);
  digitalWrite(cylinder_E, LOW);
  
  delay(2000);
  STATE+=1;
}

void SUCTION_OFF()
{
  Serial.println("SUCTION OFF");
  //placeholder for suction off
  digitalWrite(ledPin, LOW);
  
  delay(2000);
  STATE+=1;
}

void NAILING()
{
  Serial.println("NAILING");
  delay(5000);
  STATE+=1;
}

void DRILL_BACKWARD()
{
  //turn drill backward
  digitalWrite(drill_en, HIGH);
  digitalWrite(drill_A, LOW);    //HIGH/HIGH = FRONT?
  digitalWrite(drill_B, LOW);
  
  while(!digitalRead(limit_in)){    //LIMIT SWITCH HIGH = HIT?
    Serial.println("DRILL BACKWARD");
  }
  //turn drill off
  digitalWrite(drill_en, LOW);
  digitalWrite(drill_A, HIGH);    //LOW/LOW = BACK?
  digitalWrite(drill_B, HIGH);
  STATE+=1;
}
