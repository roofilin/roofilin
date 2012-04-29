#include <Servo.h>

Servo myservo;

int drill_forward_1 = 30;
int drill_forward_2 = 32;
int drill_backward_1 = 34;
int drill_backward_2 = 36;
int drill_en = 28;



void setup()
{
 pinMode(drill_forward_1, OUTPUT);
 pinMode(drill_forward_2, OUTPUT);
 pinMode(drill_backward_1, OUTPUT);
 pinMode(drill_backward_2, OUTPUT);
 pinMode(drill_en, OUTPUT);

 pinMode(8, OUTPUT);
 pinMode(9, OUTPUT);

 myservo.attach(13);



 //////////////////////INITIALIZATION////////////////////////////
 myservo.write(180);
 digitalWrite(drill_en, LOW);
 digitalWrite(drill_forward_1, LOW);
 digitalWrite(drill_forward_2, LOW);
 digitalWrite(drill_backward_1, LOW);
 digitalWrite(drill_backward_2, LOW);
 digitalWrite(8, LOW);
 digitalWrite(9, LOW);
 ////////////////////////////////////////////////////////////////




}

void loop()
{


}


void move_y(int dir, int time) {
 //1 is away from robot, 0 is towards (in)
 switch(dir) {
   case 0:
     digitalWrite(drill_forward_1, LOW);
     digitalWrite(drill_forward_2, LOW);
     digitalWrite(drill_backward_1, HIGH);
     digitalWrite(drill_backward_2, HIGH);
     break;
   case 1:
     digitalWrite(drill_forward_1, HIGH);
     digitalWrite(drill_forward_2, HIGH);
     digitalWrite(drill_backward_1, LOW);
     digitalWrite(drill_backward_2, LOW);
     break;
   default:
     digitalWrite(drill_forward_1, LOW);
     digitalWrite(drill_forward_2, LOW);
     digitalWrite(drill_backward_1, LOW);
     digitalWrite(drill_backward_2, LOW);
 }
 delay(1000);
 digitalWrite(drill_en, HIGH);
 delay(time);
 digitalWrite(drill_en, LOW);
 delay(500);
 digitalWrite(drill_forward_1, LOW);
 digitalWrite(drill_forward_2, LOW);
 digitalWrite(drill_backward_1, LOW);
 digitalWrite(drill_backward_2, LOW);
}

void nail() {
 myservo.write(20);
 delay(1000);
 myservo.write(180);
 delay(1000);
}

void raise_arm() {
 digitalWrite(8, HIGH);
 digitalWrite(9, LOW);
 delay(300);
 digitalWrite(8, LOW);
 digitalWrite(9, LOW);
}

void lower_arm() {
 digitalWrite(9, HIGH);
 digitalWrite(8, LOW);
 delay(300);
 digitalWrite(8, LOW);
 digitalWrite(9, LOW);
}
