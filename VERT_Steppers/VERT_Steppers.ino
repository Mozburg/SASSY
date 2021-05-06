
#include <avr/io.h>
#include <loadcell.h>
/* defines for direction variable, controlling stepper direction */
#define GO_up LOW
#define GO_down HIGH
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
uint8_t direction = go_down;
void setup() {
          fastp_init(); //set registers for fast pwm, mode 7, non-inverting
          drill_init(); //Set top value for 65Hz operation, initialize at 0% duty cycle, 
          sensor_init();
          Serial.begin(115200); // setup serial
          pinMode(36, OUTPUT);
          //pinMode(x, OUTPUT); //preparing for horizontal integration
          digitalWrite(36,direction);
        }

void loop() {
  /* updates frequently */
    if(Serial.available()>0)
  {
    char incomingByte = Serial.read();
    Serial.println(incomingByte);
    
    if(incomingByte == 'A')
    {
      speedup();
    }
    if(incomingByte == 'F')
    {
      Slowdown();
    }
    if(incomingByte == 'S')
    {
      Stop();
    }
    if(incomingByte == 'M')
    {
      maxSpeed();
    }
    if(incomingByte == 'B')
    {
      slowestSpeed();
    }
    if(incomingByte == 'U')
    {
      up();
    }
    if(incomingByte == 'D')
    {
      down();
    }
  }
    //Serial.println(wobRead);
    OCR2A = newSpeed; //Top value for this button
    tempFloat = OCR2A/2;
    tempInt = int(tempFloat);
    OCR2B = tempInt; //Ontime of the 65Hz signal Initialized to zero
}
void Slowdown(){
  if(newSpeed >= 255)
  {
    newSpeed = 255;
  }
  else{
    newSpeed = new_speed + 20;
  }
  //incomingByte = NULL;
}
void speedup(){
  if(newSpeed <= 20){
    newSpeed = 20;
  }
  else{
    newSpeed = newSpeed - 20;
  }
  //incomingByte = NULL;
  
}
void Stop(){
  newSpeed = 0;
  //incomingByte = NULL 
}
void maxSpeed(){
  newSpeed = 20;
  //incomingByte = NULL 
}
void slowestSpeed(){
  newSpeed = 255;
  //incomingByte = NULL 
}
void down(){
  //digitalWrite(6, LOW);
  digitalWrite(36,GO_up); 
}
void up()
{
  //digitalWrite(6, HIGH);
  digitalWrite(36, GO_down);
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
 