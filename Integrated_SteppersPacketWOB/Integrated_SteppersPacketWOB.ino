// TIMER ASSIGNMENTS
/*
 * timer 2 = steppers
 * timer 4 = horz
 * timer 3 = winch
 */
#include <avr/io.h>
#define f_CLK 16000000
#define PI 3.1415926535897932384626433832795
//==== Serial Data
const byte numChars = 32;
char receivedChars[numChars];
//===
boolean newData = false;
///HORZ VARIABLES
int horzTopCount = 255;
float horzTempFloat = 0;
int horzTempInt = 0;
int horzNewSpeed = 0;
//===
//WINCH VARIABLES
int winchTopCount = 255;
float winchTempFloat = 0;
int winchTempInt = 0;
int winchNewSpeed = 0;
//===
/*This code uses Arduino Uno digital pwm pin 3 and gnd to output a pwm signal through the solidstate relay that controls the drill speed.
Can substitute an LED or probably a servo motor for low power testing. Need to take rpm data normalized between integers 0 - 120 to establish 
current and vertNewSpeed values. 
*/
//====TIMER VARIABLES===

float top_count_float  =0;
int top_count_int;
float bottom_count_float = 0;
int bottom_count_int = 0;
//===
//has incremental speed controls
float Duty = 3*PI/2; //Initializes the Duty Cycle at 0%
float OnCount; int TopCount = 255; int On;
//======== Simulating RPM Sensor Feedback ================
float rpmMax = 1000; //arbitrary value
float rpm = 0; //current value (arbitrary)
//float rpmDes = 350; //Desired value (arbitrary)
//========================================================
int vertNewSpeed = 0;
int current; 
int DcShift;
float tempFloat = 0;
int comPortInitialization;
uint8_t direction = LOW;
int load_cell_callibration = 0;
int tempInt = 0;
int top_switch = 0;
int bottom_switch = 0;
float wobRead;
bool wobOverLoad = false;
const unsigned long eventInterval = 5000;
unsigned long previousTime = 0;
//WOB SERNSORS
int val; float voltage, weight, force, VtoLBS, Vload;
#define gain 455    //Voltage gain of instrumentation amplifier
#define maxWeight 100
#define sensitivity 0.03
//Averaging Filter for LoadCell Variables
const int numReadings = 200;
int readings[numReadings];      // the readings from the analog input
int readIndex = 0;              // the index of the current reading
int total = 0;                  // the running total
float average = 0;                // the average

int inputPin = A3;
//PIN 9 FOR PWM
void setup() {
          timer_init(2, 1000, .5, 256);//pin 9 MIN FREQ = 244, MAX = 62.5Khz
          timer_init(3, 1000, .5, 64);//pin 2 
          timer_init(4, 1000, .5, 64);//pin 9 MIN FREQ = 244, MAX = 62.5Khz 
          sensor_init();
          Serial.begin(115200); // setup serial
          pinMode(36, OUTPUT);
          digitalWrite(36,direction);
          pinMode(38, OUTPUT);
          digitalWrite(38,direction);
          pinMode(40, OUTPUT);
          digitalWrite(40,HIGH) ;//HIGH MEANS DROP DOWN FOR WINCH
          //load_cell_callibration = wobSensor.GetLoad();
        }

void loop() {
  /* Updates frequently */
    recvWithStartEndMarkers();
    showNewData();
    updateTimerTopCounts();
      // calculate the average:
  // subtract the last reading:
  total = total - readings[readIndex];
  // read from the sensor:
  readings[readIndex] = wobRead;
  // add the reading to the total:
  total = total + readings[readIndex];
  // advance to the next position in the array:
  readIndex = readIndex + 1;

  // if we're at the end of the array...
  if (readIndex >= numReadings) {
    // ...wrap around to the beginning:
    readIndex = 0;
  }

  // calculate the average:
  average = total / numReadings;
  // send it to the computer as ASCII digits
 
  //Serial.println(wobOverLoad);
  if (average >= 125){
    vertUP();
    wobOverLoad = true;
  }

  if (average <= 40 && wobOverLoad == true){
    vertDOWN();
    wobOverLoad = false;
  }
  sendSensorDataPacket();
}
void sendSensorDataPacket(){
  Serial.print('<');
  Serial.print(average);
  Serial.print(",");
  Serial.print(vertNewSpeed);
  Serial.println('>');
}
void updateTimerTopCounts(){
      //===CHANGE TOPCOUNT FOR VERT_STEPPER===
    OCR2A = vertNewSpeed; //Top value for this button
    tempFloat = OCR2A*.9;
    tempInt = int(tempFloat);
    OCR2B = tempInt; //Ontime of the 65Hz signal Initialized to zero
    //=====================================================
    //===CHANGE TOPCOUNT FOR horz_STEPPER===
    OCR4A = horzNewSpeed; //Top value for this button
    horzTempFloat = OCR4A/2;
    horzTempInt = int(tempFloat);
    OCR4B = horzTempInt; //Ontime of the 65Hz signal Initialized to zero
    //=====================================================
    //===CHANGE TOPCOUNT FOR WINCH_STEPPER===
    OCR3A = winchNewSpeed; //Top value for this button
    winchTempFloat = OCR3A/2;
    winchTempInt = int(winchTempFloat);
    OCR3B = winchTempInt; //Ontime of the 65Hz signal Initialized to zero
}
void recvWithStartEndMarkers() {
    static boolean recvInProgress = false;
    static byte ndx = 0;
    char startMarker = '<';
    char endMarker = '>';
    char rc;
 
    while (Serial.available() > 0 && newData == false) {
        rc = Serial.read();

        if (recvInProgress == true) {
            if (rc != endMarker) {
                receivedChars[ndx] = rc;
                ndx++;
                if (ndx >= numChars) {
                    ndx = numChars - 1;
                }
            }
            else {
                receivedChars[ndx] = '\0'; // terminate the string
                recvInProgress = false;
                ndx = 0;
                newData = true;
            }
        }

        else if (rc == startMarker) {
            recvInProgress = true;
        }
    }
}
void showNewData() {
    if (newData == true) {
        //Serial.print("This just in ... ");
        //Serial.println(receivedChars);
    if(receivedChars[0] == 'Q')vertSpeedUp();
    else if(receivedChars[0] == 'E')vertSlowDown();
    else if(receivedChars[0] == 'Z')vertStop();
    else if(receivedChars[0] == 'A')vertMaxSpeed();
    else if(receivedChars[0] == 'D')vertSlowestSpeed();
    else if(receivedChars[0] == 'W')vertUP();
    else if(receivedChars[0] == 'S')vertDOWN();
    //HORZ
    else if(receivedChars[0] == 'R')horzSpeedUp();
    else if(receivedChars[0] == 'Y')horzSlowDown();
    else if(receivedChars[0] == 'C')horzStop();
    else if(receivedChars[0] == 'F')horzMaxSpeed();
    else if(receivedChars[0] == 'H')horzSlowestSpeed();
    else if(receivedChars[0] == 'T')horzUP();
    else if(receivedChars[0] == 'G')horzDOWN();
    //WINCH
    else if(receivedChars[0] == 'U')winchSpeedUp();
    else if(receivedChars[0] == 'O')winchSlowDown();
    else if(receivedChars[0] == 'N')winchStop();
    else if(receivedChars[0] == 'J')winchMaxSpeed();
    else if(receivedChars[0] == 'L')winchSlowestSpeed();
    else if(receivedChars[0] == 'I')winchUP();
    else if(receivedChars[0] == 'K')winchDOWN();
        newData = false;
    }
}
//===vert functions
void vertSlowDown(){
  if(vertNewSpeed >= 255)vertNewSpeed = 255;
  else vertNewSpeed = vertNewSpeed + 20;
}
void vertSpeedUp(){
  if(vertNewSpeed <= 20)vertNewSpeed = 20;
  else vertNewSpeed = vertNewSpeed - 20;
  
}
void vertStop(){
  vertNewSpeed = 0;
  //receivedChars[0] = NULL 
}
void vertMaxSpeed(){
  vertNewSpeed = 20;
  //receivedChars[0] = NULL 
}
void vertSlowestSpeed(){
  vertNewSpeed = 255;
  //receivedChars[0] = NULL 
}
void vertDOWN(){
  //digitalWrite(6, LOW);
  digitalWrite(36,LOW);
  vertSlowestSpeed(); 
}
void vertUP()
{
  //digitalWrite(6, HIGH);
  digitalWrite(36, HIGH);
  vertSlowestSpeed();
}
//===HORZ FUNCTIONS====
void horzSlowDown(){
  if(vertNewSpeed >= 1024)horzNewSpeed = 1024;
  else horzNewSpeed = horzNewSpeed + 20;
  //receivedChars[0] = NULL;
}
void horzSpeedUp(){
  if(horzNewSpeed <= 20)horzNewSpeed = 20;
  else horzNewSpeed = horzNewSpeed - 20;
  //receivedChars[0] = NULL;
  
}
void horzStop(){
  horzNewSpeed = 0;
  //receivedChars[0] = NULL 
}
void horzMaxSpeed(){
  horzNewSpeed = 20;
  //receivedChars[0] = NULL 
}
void horzSlowestSpeed(){
  horzNewSpeed = 1024;
  //receivedChars[0] = NULL 
}
void horzDOWN(){
  //digitalWrite(6, LOW);
  digitalWrite(38,LOW); 
}
void horzUP()
{
  //digitalWrite(6, HIGH);
  digitalWrite(38, HIGH);
}
//=====
//WINCH======
void winchSlowDown(){
  if(winchNewSpeed >= 1024)winchNewSpeed = 1024;
  else winchNewSpeed = winchNewSpeed + 20;
  //receivedChars[0] = NULL;
}
void winchSpeedUp(){
  if(winchNewSpeed <= 20)winchNewSpeed = 20;
  else winchNewSpeed = winchNewSpeed - 20;
  
}
void winchStop(){
  winchNewSpeed = 0;
}
void winchMaxSpeed(){
  winchNewSpeed = 20;
}
void winchSlowestSpeed(){
  winchNewSpeed = 1024;
}
void winchDOWN(){
  //digitalWrite(6, LOW);
  digitalWrite(40,HIGH); 
}
void winchUP()
{
  //digitalWrite(6, HIGH);
  digitalWrite(40, LOW);
}
void timer_init(int timer_number, float freq_op, float duty_cycle_multiplier, float pre_scale_value){
  switch(timer_number) {
      case 0 :
        TCCR0A |= (1<<WGM01) | (1<<WGM00); //Setting up wave generation moe for fast PWM. Still need to set WGM22
        TCCR0B |= (1<<WGM02); //Fast PWM wave generation mode 7 set. OCR2A is TOP.
        TCCR0A |= (1<<COM0B1); //COM2B0 is default 0. COM2B1 set for non-inverting operation.
        //these if statements are responsible for setting the correct pre-scalar depending on the pre_scalar parameter
        pinMode(4, OUTPUT);
        //top_count_float will calculate a top count that will output a frequency specified by timer_init
        top_count_float = ((f_CLK/freq_op)/pre_scale_value)-1;
        //bottom_count will be the value given to OCR2B for pwm generation
        bottom_count_float = top_count_float*duty_cycle_multiplier;
        //convert floating values to integer as OCRnA and OCRnB only accepts integers
        top_count_int = int(top_count_float);
        bottom_count_int = int(bottom_count_float);
        OCR0A = top_count_int;
        OCR0B = bottom_count_int;
        TCCR0B &=~(1<<CS00);
        TCCR0B &=~ (1<<CS01); 
        TCCR0B &=~(1<<CS02);
        if(pre_scale_value == 0)
          TCCR0B &= TCCR0B; //Pre-scale = 0   16M/256 = 62.5kHz and start the clock ***I think this or 16MHz
        else if(pre_scale_value == 8)
        {
          TCCR0B |= (1<<CS01); //Pre-scale = 8   16M/(8*256) = 7.8125kHz and start the clock
        }
        else if(pre_scale_value ==64){
          TCCR0B |= (1<<CS01)| (1<<CS00); //Pre-scale = 64   16M/(64*256) = 976.56Hz and start the clock
        }
        else if (pre_scale_value == 256)
          TCCR0B |= (1<<CS02); //Pre-scale = 64   16M/(64*256) = 976.5625Hz and start the clock
        else if (pre_scale_value == 1024)
          TCCR0B |= (1<<CS02)|(1<<CS00); //Pre-scale = 8   16M/(1024*256) = 61Hz and start the clock
        //TCCR0B |= (1<<CS01); //Pre-scale = 8   16M/(8*256) ,= 7.8125kHz and start the clock

        break;
      case 1 :
        TCCR1A |= (1<<WGM11) | (1<<WGM10); //Setting up wave generation mode for fast PWM. Still need to set WGM22
        TCCR1B |= (1<<WGM12)|(1<<WGM13); //Fast PWM wave generation mode 7 set. OCR2A is TOP.
        TCCR1A |= (1<<COM1B1); //COM2B0 is default 0. COM2B1 set for non-inverting operation.
        pinMode(12,OUTPUT);
        //top_count_float will calculate a top count that will output a frequency specified by timer_init
        top_count_float = (f_CLK/((freq_op)*(pre_scale_value)))-1;
        //bottom_count will be the value given to OCR2B for pwm generation
        bottom_count_float = top_count_float*duty_cycle_multiplier;
        //convert floating values to integer as OCRnA and OCRnB only accepts integers
        top_count_int = int(top_count_float);
        bottom_count_int = int(bottom_count_float);
        OCR1A = top_count_int;
        OCR1B = bottom_count_int;
        TCCR1B &=~(1<<CS00);
        TCCR1B &=~ (1<<CS01); 
        TCCR1B &=~(1<<CS02);
        if(pre_scale_value == 0)
          TCCR1B |= (1<<CS10); //Pre-scale = 0   16M/256 = 62.5kHz and start the clock ***I think this or 16MHz
        else if(pre_scale_value ==8)
          TCCR1B |= (1<<CS11); //Pre-scale = 8   16M/(8*256) = 7.8125kHz and start the clock
        else if(pre_scale_value ==64)
          TCCR1B |= (1<<CS11)| (1<<CS10); //Pre-scale = 64   16M/(64*256) = 976.56Hz and start the clock
        else if (pre_scale_value == 256)
          TCCR1B |= (1<<CS12); //Pre-scale = 256   16M/(64*256) = 244.14Hz and start the clock
        else if (pre_scale_value == 1024)
          TCCR1B |= (1<<CS12)|(1<<CS10); //Pre-scale = 8   16M/(8*256) = 7.8125kHz and start the clock
        //TCCR1B |= (1<<CS11); //Pre-scale = 8   16M/(8*256) = 7.8125kHz and start the clock
        break;
      case 2 :
        TCCR2A |= (1<<WGM21) | (1<<WGM20); //Setting up wave generation mode for fast PWM. Still need to set WGM22
        TCCR2B |= (1<<WGM22); //Fast PWM wave generation mode 7 set. OCR2A is TOP.
        TCCR2A |= (1<<COM2B1); //COM2B0 is default 0. COM2B1 set for non-inverting operation.
        TCCR2B &=~(1<<CS00);
        TCCR2B &=~ (1<<CS01); 
        TCCR2B &=~(1<<CS02);
        OCR2A = TopCount; //Top value for this button
        tempFloat = TopCount/2;
        tempInt = int(tempFloat);
        OCR2B = tempInt; //Ontime of the 65Hz signal Initialized to zero
        if(pre_scale_value == 0)
          TCCR2B |= (1<<CS20); //Pre-scale = 0   16M/256 = 62.5kHz and start the clock ***I think this or 16MHz
        else if(pre_scale_value ==8)
          TCCR2B |= (1<<CS21); //Pre-scale = 8   16M/(8*256) = 7.8125kHz and start the clock
        else if(pre_scale_value ==32)
          TCCR2B |= (1<<CS21)| (1<<CS20); //Pre-scale = 32   16M/(32*256) = 1.953kHz and start the clock
        else if (pre_scale_value == 64)
          TCCR2B |= (1<<CS22); //Pre-scale = 64   16M/(64*256) = 976.5625Hz and start the clock
        else if (pre_scale_value == 128)
          TCCR2B |= (1<<CS22)|(1<<CS20); //Pre-scale = 128   16M/(128*256) =     Hz and start the clock
        else if (pre_scale_value == 256)
          TCCR2B |= (1<<CS22)|(1<<CS21); //Pre-scale = 256   16M/(256*256) =     Hz and start the clock
        else if (pre_scale_value == 1024)
          TCCR2B |= (1<<CS22)|(1<<CS20)|(1<<CS21); //Pre-scale = 1024   16M/(1024*256) =     Hz and start the clock
        //TCCR2B |= (1<<CS21); //Pre-scale = 8   16M/(8*256) = 7.8125kHz and start the clock
        pinMode(9,OUTPUT);
        break;
      case 3 :
        TCCR3A |= (1<<WGM31) | (1<<WGM30); //Setting up wave generation mode for fast PWM. Still need to set WGM22
        TCCR3B |= (1<<WGM32)|(1<<WGM33); //Fast PWM wave generation mode 15 set. OCR2A is TOP.
        TCCR3A |= (1<<COM3B1); //COM2B0 is default 0. COM2B1 set for non-inverting operation.
        TCCR3B &=~(1<<CS00);
        TCCR3B &=~ (1<<CS01); 
        TCCR3B &=~(1<<CS02);
        OCR3A = winchTopCount; //Top value for this button
        winchTempFloat = winchTopCount/2;
        winchTempInt = int(winchTempFloat);
        OCR3B = winchTempInt; //Ontime of the 65Hz signal Initialized to zero
        if(pre_scale_value == 0)
          TCCR3B |= (1<<CS30); //Pre-scale = 0   16M/256 = 62.5kHz and start the clock ***I think this or 16MHz
        else if(pre_scale_value ==8)
          TCCR3B |= (1<<CS31); //Pre-scale = 8   16M/(8*256) = 7.8125kHz and start the clock
        else if(pre_scale_value ==64)
          TCCR3B |= (1<<CS31)| (1<<CS30); //Pre-scale = 64   16M/(64*256) = 976.56Hz and start the clock
        else if (pre_scale_value == 256)
          TCCR3B |= (1<<CS32); //Pre-scale = 256   16M/(64*256) = 244.14Hz and start the clock
        else if (pre_scale_value == 1024)
          TCCR3B |= (1<<CS32)|(1<<CS30); //Pre-scale = 8   16M/(8*256) = 7.8125kHz and start the clock
        //TCCR3B |= (1<<CS31); //Pre-scale = 8   16M/(8*256) = 7.8125kHz and start the clock
        pinMode(2,OUTPUT);
        break;
      case 4 :
        TCCR4A |= (1<<WGM41) | (1<<WGM40); //Setting up wave generation mode for fast PWM. Still need to set WGM22
        TCCR4B |= (1<<WGM42)|(1<<WGM43); //Fast PWM wave generation mode 7 set. OCR2A is TOP.
        TCCR4A |= (1<<COM4B1); //COM2B0 is default 0. COM2B1 set for non-inverting operation.
        TCCR4B &=~(1<<CS00);
        TCCR4B &=~ (1<<CS01); 
        TCCR4B &=~(1<<CS02);
        OCR4A = horzTopCount; //Top value for this button
        horzTempFloat = horzTopCount/2;
        horzTempInt = int(horzTempFloat);
        OCR4B = horzTempInt; //Ontime of the 65Hz signal Initialized to zero
        if(pre_scale_value == 0)
          TCCR4B |= (1<<CS40); //Pre-scale = 0   16M/256 = 62.5kHz and start the clock ***I think this or 16MHz
        else if(pre_scale_value ==8)
          TCCR4B |= (1<<CS41); //Pre-scale = 8   16M/(8*256) = 7.8125kHz and start the clock
        else if(pre_scale_value ==64)
          TCCR4B |= (1<<CS41)| (1<<CS40); //Pre-scale = 64   16M/(64*256) = 976.56Hz and start the clock
        else if (pre_scale_value == 256)
          TCCR4B |= (1<<CS42); //Pre-scale = 256   16M/(64*256) = 244.14Hz and start the clock
        else if (pre_scale_value == 1024)
          TCCR4B |= (1<<CS42)|(1<<CS40); //Pre-scale = 8   16M/(8*256) = 7.8125kHz and start the clock
        //TCCR4B |= (1<<CS41); //Pre-scale = 8   16M/(8*256) = 7.8125kHz and start the clock
        pinMode(7,OUTPUT);
        break;
      default :
        Serial.println("Invalid timer number");
  }  
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
  val = analogRead(A3);                    //Value read from loadcell
  voltage = ((val) * 5.0 / 1023.0);       //Voltage seen at output of instrumentation amplifier
  Vload = voltage / gain;                         //Voltage out of load cell
  wobRead = ((Vload * maxWeight) / sensitivity) * 4.45; //Convert Voltage to Newtons
  sei();
}
 
