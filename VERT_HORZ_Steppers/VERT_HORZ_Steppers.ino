
#include <avr/io.h>
#include <loadcell.h>
//STILL NEEDS TO INTEGRATE TIMER FUNCTION CODE TO ADD SECOND TIMER
//POSSIBLY USE TOGGLE PINS THAT WAY WE CAN USE SAME TIMERS
/* defines for vertDirection variable, controlling stepper vertDirection */
#define GO_UP LOW
#define GO_DOWN HIGH
/*--------------------------------------------------------------*/
/*This code uses Arduino Uno digital pwm pin 3 and gnd to output a pwm signal through the solidstate relay that controls the drill speed.
Can substitute an LED or probably a servo motor for low power testing. Need to take rpm data normalized between integers 0 - 120 to establish 
current and newSpeed values. 
*/
//has incremental speed controls
int topCount = 255; int On;
//==============Top Count and Bottom count calculation variables======
int newSpeed = 0; //on initialization, top count is set to newSpeed. Top count of 0 makes steppers not move.
float tempFloat = 0;
int tempInt = 0; //used for bottom count of OCR2B calculation
//==============WOB calculation variables ====================
float wobRead;
bool wobOverLoad = false;
Loadcell wobSensor;
uint8_t vertDirection = GO_DOWN;
uint8_t horzDirection = GO_DOWN;
void setup() {
          fastp_init(); //set registers for fast pwm, mode 7, non-inverting
          drill_init(); //Set top value for 65Hz operation, initialize at 0% duty cycle, 
          sensor_init();
          Serial.begin(115200); // setup serial
          pinMode(36, OUTPUT);
          //pinMode(x, OUTPUT); //preparing for horizontal integration
          //digitalWrite(x, horzDirection);
          digitalWrite(36,vertDirection);
        }

void loop() {
  /* updates frequently */
    if(Serial.available()>0)
  {
    char incomingByte = Serial.read();
    Serial.println(incomingByte);
    //vertical stepper motor commands
    if(incomingByte == 'A')speedUpVert();
    else if(incomingByte == 'F')slowDownVert();
    else if(incomingByte == 'S')stopMotorVert();
    else if(incomingByte == 'M')maxSpeedVert();
    else if(incomingByte == 'B')slowestSpeedVert();
    else if(incomingByte == 'U')upVert();
    else if(incomingByte == 'D')downVert();
    //Horizontal stepper motor commands
    else if(incomingByte == 'Q')speedUpHorz();
    else if(incomingByte == 'W')slowDownHorz();
    else if(incomingByte == 'E')stopMotorHorz();
    else if(incomingByte == 'R')maxSpeedHorz();
    else if(incomingByte == 'T')slowestSpeedHorz();
    else if(incomingByte == 'Y')upHorz();
    else if(incomingByte == 'I')downHorz();
  }
  Serial.println(newSpeed);
    //Serial.println(wobRead);
    OCR2A = newSpeed; //Top value for this button
    tempFloat = OCR2A/2;
    tempInt = int(tempFloat);
    OCR2B = tempInt; //Ontime of the 65Hz signal Initialized to zero
}
//====VERTICAL MOTOR FUNCTIONS===
void slowDownVert(){
  Serial.println("Testing");
  if(newSpeed >= 255) newSpeed = 255;
  else newSpeed = newSpeed + 20;
}
void speedUpVert(){
  if(newSpeed <= 20) newSpeed = 20;
  else newSpeed = newSpeed - 20;
}
void stopMotorVert(){
  newSpeed = 0;
}
void maxSpeedVert(){
  newSpeed = 20;
}
void slowestSpeedVert(){
  newSpeed = 255;
}
void downVert(){
  digitalWrite(36,GO_UP); 
}
void upVert(){
  digitalWrite(36, GO_DOWN);
}
//===HORIZONTAL MOTOR FUNCTIONS===
void slowDownHorz(){
  if(newSpeed >= 255) newSpeed = 255;
  else newSpeed = newSpeed + 20;
}
void speedUpHorz(){
  Serial.println("I'm in");
  if(newSpeed <= 20) newSpeed = 20;
  else newSpeed = newSpeed - 20;
}
void stopMotorHorz(){
  newSpeed = 0;
}
void maxSpeedHorz(){
  newSpeed = 20;
}
void slowestSpeedHorz(){
  newSpeed = 255;
}
void downHorz(){
  digitalWrite(36,GO_UP); 
}
void upHorz(){
  digitalWrite(36, GO_DOWN);
}
    



  void fastp_init (void){
  //This specifically uses PD3 as the PWM output pin.
  pinMode(9,OUTPUT); //Set Port D pin 3 to output. Connecting OC2B to the pin (Speaker Fast PWM pin).
  TCCR2A |= (1<<WGM21) | (1<<WGM20); //Setting up wave generation mode for fast PWM. Still need to set WGM22
  TCCR2B |= (1<<WGM22); //Fast PWM wave generation mode 15 set. OCR2A is TOP.
  TCCR2A |= (1<<COM2B1); //COM2B0 is default 0. COM2B1 set for non-inverting operation.
}

void drill_init (void){
    OCR2A = topCount; //Top value for this button
    tempFloat = topCount/2;
    tempInt = int(tempFloat);
    OCR2B = tempInt; //Ontime of the 65Hz signal Initialized to zero
    TCCR2B |= (1<<CS21); //Pre-scale = 8   16M/(8*256) = 7.8125kHz and start the clock
  }
void sensor_init(void)
{
  TCCR0A=0;
  TCCR0B=0;
  TCNT0=0;
  OCR0A = 24;                       //Top count is 16M/(1024*640), for 650Hz sampling at 1024 prescale value (rounded down).
  TIMSK0 |= (1<<OCIE0A);            //enable timer mask interrupts
  TCCR0A |= (1<<WGM01);             //CTC mode
  TCCR0B |= (1<<CS02) | (1<<CS00);  //Prescale value 1024 
}

ISR(TIMER0_COMPA_vect){
    cli();
    wobRead = wobSensor.GetLoad();
//    if (wobRead >= 120){
//      up();
//      wobOverLoad = true;
//    }
//    else if(wobOverLoad == true && wobRead <= 100){
//      down();
//      wobOverLoad = false; //wobOverLoad flag ensures this command won't overwrite our current controls.
//    }
    
    sei();
}
 
