
#include <avr/io.h>
#define PI 3.1415926535897932384626433832795

/*This code uses Arduino Uno digital pwm pin 9 and gnd to output a pwm signal through the solidstate relay that controls the drill speed.
Can substitute an LED or probably a servo motor for low power testing. Need to take rpm data normalized between integers 0 - 120 to establish 
current and newspeed values. 
*/

float Duty = 3*PI/2; //Initializes the Duty Cycle at 0%
float OnCount; int TopCount; int On;
//======== Simulating RPM Sensor Feedback ================
float rpmMax = 1000; //arbitrary value
float rpm = 0; //current value (arbitrary)
//float rpmDes = 350; //Desired value (arbitrary)
//========================================================
int newspeed = 0; 
int current; 
int DcShift;
int comPortInitialization;


void setup() {
          fastp_init(); //set registers for fast pwm, mode 7, non-inverting
          drill_init(); //Set top value for 65Hz operation, initialize at 0% duty cycle, 
          Serial.begin(115200); // setup serial
        }

void loop() {
    if(Serial.available()>0)
  {
    char incomingByte = Serial.read();
    Serial.println(incomingByte);
    
    if(incomingByte == 'T')
    {
      SpeedUp();
    }
    if(incomingByte == 'F')
    {
      SlowDown();
    }
  }
  // put your main code here, to run repeatedly:
    current = speedCorrect(current, newspeed);
}
void SpeedUp(){
  if(newspeed == 120)
  {
    newspeed = newspeed;
  }
  else{
    newspeed = newspeed + 10;
  }
  //incomingByte = NULL;
}
void SlowDown(){
  if(newspeed == 0){
    newspeed = newspeed;
  }
  else{
    newspeed = newspeed - 10;
  }
  //incomingByte = NULL;
  
}
    
  void fastp_init (void){
  //This specifically uses PD3 as the PWM output pin.
  pinMode(9,OUTPUT); //Set Port D pin 9 to output. Connecting OC2B to the pin (Speaker Fast PWM pin).
  TCCR2A |= (1<<WGM21) | (1<<WGM20); //Setting up wave generation mode for fast PWM. Still need to set WGM22
  TCCR2B |= (1<<WGM22); //Fast PWM wave generation mode 7 set. OCR2A is TOP.
  TCCR2A |= (1<<COM2B1); //COM2B0 is default 0. COM2B1 set for non-inverting operation.
}

void drill_init (void){
    current=0;
    TopCount = 65;  //for 120Hz min Nyquist rate for a 60Hz signal
    OCR2A = TopCount; //Top value for this button
    OCR2B = 0; //Ontime of the 65Hz signal Initialized to zero
    TCCR2B |= (1<<CS21); //Pre-scale = 8   16M/(8*256) = 7.8125kHz and start the clock
  }

int speedCorrect(int current, int newspeed){ 
    //=============== Speed Up ===================
    if(current < newspeed){
        DcShift = (newspeed-current)/2;
        Duty = 3*PI/2;
        while (Duty < 5*PI/2){
        Duty = Duty+0.01309;
        OnCount = DcShift*sin(Duty)+current+DcShift; //Shifted and scaled sinwave for soft transition
        On = OnCount;
        OCR2B = On;
        }
        current = newspeed;
        return(current); //****************later plan to use RPM data to hold current speed
       }
    //=============== Slow Down ==================
    if(current > newspeed){
        DcShift = (current-newspeed)/2;
        Duty = PI/2;
        while (Duty > -PI/2){
        Duty = Duty-0.01309;
        OnCount = DcShift*sin(Duty)+current-DcShift; //Shifted and scaled sinwave for soft transition
        On = OnCount;
        OCR2B = On;
        }
        current = newspeed;
        return(current); //****************later plan to use RPM data to hold current speed
       }
   }
  
