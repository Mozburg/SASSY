
#include <avr/io.h>
#define PI 3.1415926535897932384626433832795

/*This code uses Arduino Uno digital pwm pin 9 and gnd to output a pwm signal through the solidstate relay that controls the drill speed.
Can substitute an LED or probably a servo motor for low power testing. Need to take rpm data normalized between integers 0 - 120 to establish 
current and newSpeed values. 
*/
//4.0.0 CHANGES
//ADDED INTEGER PARSING FUNCTION TO ADJUST DRILL SPEED
//A1 IS DRILL CURRENT
//A2 IS SYSTEM CURRENT
float Duty = 3*PI/2; //Initializes the Duty Cycle at 0%
float OnCount; int topCount; int On;
//======== Simulating RPM Sensor Feedback ================
float rpmMax = 1000; //arbitrary value
float rpm = 0; //current value (arbitrary)
//float rpmDes = 350; //Desired value (arbitrary)
//========================================================
int newSpeed = 0; 
int current; 
int DcShift;
int comPortInitialization;
//======Current sensor variables======
float val1 = 512; // variable to store the value read
float val2 = 512;
float Vmap = 0; //Map to voltage
float Vmap2 = 0;
float drillAmps = 0; //Conversion to amps
float systemAmps = 0;
char incomingByte = 0;
int integerValue = 0;
int finalspeedValue = 0;
void setup() {
          current_sense_init();
          fastp_init(); //set registers for fast pwm, mode 7, non-inverting
          drill_init(); //Set top value for 65Hz operation, initialize at 0% duty cycle, 
          Serial.begin(115200); // setup serial
        }

void loop() {
  if (Serial.available() > 0) 
  {   // something came across serial
    integerValue = 0;         // throw away previous integerValue
    while(1) 
    {            // force into a loop until 'n' is received
      incomingByte = Serial.read();
      if (incomingByte == 'c'){
        if(integerValue >= 65)
          integerValue = 65;
        else if( integerValue <= 0)
          integerValue = 0;
        newSpeed = integerValue;
        break;
      }
      if (incomingByte == 's'){
        stopDrilling();
        break;
      }
      if (incomingByte == 'a'){
        SpeedUp();
        break;
      }
      if (incomingByte == 'f'){
        SlowDown();
        break;
      }
      if (incomingByte == -1) continue;  // if no characters are in the buffer read() returns -1
      if(incomingByte >= 48 && incomingByte <= 57){
        integerValue *= 10;  // shift left 1 decimal place
        // convert ASCII to integer, add, and shift left 1 decimal place
        integerValue = ((incomingByte - 48) + integerValue); 
      }
    }
    Serial.println(newSpeed);
//    newSpeed = finalspeedValue;
    // put your main code here, to run repeatedly:
    current = speedCorrect(current, newSpeed);
    //Serial.println(systemAmps);
  }
}
void SpeedUp(){
  if(newSpeed >= 65)
  {
    newSpeed = 65;
  }
  else{
    newSpeed = newSpeed + 10;
  }
  //incomingByte = NULL;
}
void SlowDown(){
  if(newSpeed <= 0){
    newSpeed = 0;
  }
  else{
    newSpeed = newSpeed - 10;
  }
  
}
void stopDrilling(){
  newSpeed = 0;
}
    
  void fastp_init (void){
  //This specifically uses PD3 as the PWM output pin.
  pinMode(9,OUTPUT); //Set Port D pin 9 to output. Connecting OC2B to the pin (Speaker Fast PWM pin).
  TCCR2A |= (1<<WGM21) | (1<<WGM20); //Setting up wave generation mode for fast PWM. Still need to set WGM22
  TCCR2B |= (1<<WGM22); //Fast PWM wave generation mode 7 set. OCR2A is TOP.
  TCCR2A |= (1<<COM2B1); //COM2B0 is default 0. COM2B1 set for non-inverting operation.
}
 void current_sense_init (void){
  TCCR0A=0;
  TCCR0B=0;
  TCNT0=0;
  
  OCR0A = 46;  //Top count is 16M/(1024*338), for 338Hz sampling at 1024 prescale value (rounded down).
  TIMSK0 |= (1<<OCIE0A); //enable timer mask interrupts
  TCCR0A |= (1<<WGM01); //CTC mode
  TCCR0B |= (1<<CS02) | (1<<CS00); //Prescale value 1024
  pinMode(A1, INPUT);
  pinMode(A2, INPUT); // input init
}

void drill_init (void){
    current=0;
    topCount = 65;  //for 120Hz min Nyquist rate for a 60Hz signal
    OCR2A = topCount; //Top value for this button
    OCR2B = 0; //Ontime of the 65Hz signal Initialized to zero
    TCCR2B |= (1<<CS21); //Pre-scale = 8   16M/(8*256) = 7.8125kHz and start the clock
  }

int speedCorrect(int current, int newSpeed){ 
    //=============== Speed Up ===================
    if(current < newSpeed){
        DcShift = (newSpeed-current)/2;
        Duty = 3*PI/2;
        while (Duty < 5*PI/2){
        Duty = Duty+0.01309;
        OnCount = DcShift*sin(Duty)+current+DcShift; //Shifted and scaled sinwave for soft transition
        On = OnCount;
        OCR2B = On;
        }
        current = newSpeed;
        return(current); //****************later plan to use RPM data to hold current speed
       }
    //=============== Slow Down ==================
    if(current > newSpeed){
        DcShift = (current-newSpeed)/2;
        Duty = PI/2;
        while (Duty > -PI/2){
        Duty = Duty-0.01309;
        OnCount = DcShift*sin(Duty)+current-DcShift; //Shifted and scaled sinwave for soft transition
        On = OnCount;
        OCR2B = On;
        }
        current = newSpeed;
        return(current); //****************later plan to use RPM data to hold current speed
       }
   }
 ISR(TIMER0_COMPA_vect){
    val1 = analogRead(A1); // read the input pin
    Vmap = ((5 * val1 / 1024) - 2.5); //map analog val
    drillAmps = Vmap * 10; //map amp val
    //Serial.println(drillAmps);
    
    val2 = analogRead(A2); // read the input pin
    Vmap2 = ((5 * val2 / 1024) - 2.5); //map analog val
    systemAmps = Vmap2 * 10; //map amp val
    //Serial.println(systemAmps);
}
