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
  digitalWrite(drill_en, LOW);
  digitalWrite(drill_forward_1, LOW);    //LOW/LOW = BACK?
  digitalWrite(drill_forward_2, LOW);
  digitalWrite(drill_backward_1, LOW);    //LOW/LOW = BACK?
  digitalWrite(drill_backward_2, LOW);
}

void loop()
{
  //set low
  digitalWrite(drill_en, LOW);
  delay(1000);
  //change directions
  digitalWrite(drill_forward_1, LOW);
  digitalWrite(drill_forward_2, LOW);
  digitalWrite(drill_backward_1, HIGH);
  digitalWrite(drill_backward_2, HIGH);
  delay(1000);
  //set high
  digitalWrite(drill_en, HIGH);
  delay(1000);  //moves for 1 second
  
  //set low
  digitalWrite(drill_en, LOW);
  delay(1000);
  //change directions
  digitalWrite(drill_forward_1, HIGH);
  digitalWrite(drill_forward_2, HIGH);
  digitalWrite(drill_backward_1, LOW);
  digitalWrite(drill_backward_2, LOW);
  delay(1000);
  //set high
  digitalWrite(drill_en, HIGH);
  delay(1000);  //moves for 1 second
}
