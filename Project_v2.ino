//This Version Assume Messages comes from Labview, and no sensors included
//We assume that Labview communicate with sensor board and send Messages via Serial Port
//Even Limit Switch bottoms assumed to come from Labview.
//Every Message consist of only one byte in the following structure
//b7  b6  b5  b4  b3  b2  b1  b0
//if b7 = 0: the message is only a command without any data, that give a range of 128 different such command
//if b7 = 1: The message contains two fields: b6:b5 command and b4:b0 data
//Only Command Messages ('Case Sensitive'):
//P : Plunge Command
//G : Goto TOP
//C : OverCurrent
//W : Mean WOB is greater than WOBerr = 150
//w : Mean WOB is greater than WOBmx = 145
//S : Stop Command
//i : When Ice is reached
//b : When Botton is reached
//t : When top is reached
//A : Abnormal Stop to enter in error Stop State
//Command and data Message (Till now only Out of aligment)
//111x-xxxx: where the five data bits are signed and show the difference in mm between the two stepper


#include "Arduino.h"
#include "AnalogSensor.h"
#include "StepperDriver.h"
//Pin Mapping of Arduino Mega
//A0 for Current Sensor
//A1 for Distance Sensor Stepper 1
//A2 for Distance Sensor Stepper 2
//A3 for Ice Sensor
//A4 for load Sensor
//12 OC1B for Stepper 1
//02 OC3B for stepper 2
//34 DIR1 Direction pin for stepper1
//35 EN1  Enable pin for stepper1
//36 DIR2 Direction pin for stepper2
//37 EN2  Enable pin for stepper2
//30 TOPSW Top Switch
//32 BOTSW Bottom Limitter Switch

//Fixed definition
#define TIMESLOT    50    //Time Slot for program Scheduler
#define ARRAYLENGTH 4     //Fixed Length for Measuring Analog values
#define CUR_SENS    A0    //Cuurent pin sensor input
#define N_CURSEN    4     //Sampling of current sensor = f_tmr0/N = 2000/4 = 500Hz
#define CUR_TH      5     //OverCurrent Threshhold = 5A
#define DIST1_SENS  A1    //Distance Sensor of Stepper 1
#define DIST2_SENS  A2    //Distance Sensor of Stepper 2
#define ICE_SENS    A3    //Ice Sensor
#define LOAD_SENS   A4    //Load Sensor
#define N_LOADSEN   20    //Sampling Frequency of Load Sensor at 2000/20 = 100 Hz
#define LOAD_TH1    145   //Threshold 1 of load sensor
#define LOAD_TH2    150   //Threshold 2 of load sensor
#define TOP_SW      A15   //pin 30: Assumed to be 0 and 1 when pressed
#define BOT_SW      A13   //pin 32: Assumed to be 0 and 1 when pressed
#define MicroSteps  200   //Number of Microsteps of the stepper Motor
#define DIR1        34    //Direction pin for stepper1
#define EN1         35    //Enable    pin for stepper1
#define DIR2        36    //Direction pin for stepper2    
#define EN2         37    //Enable    pin for stepper2
#define TOPSW       30    //Top Limitter Switch
#define BOTSW       32    //Bottom Limitter Switch
#define DefaultRPM  50    //DefaultRPM of the stepper

//Main Variables
unsigned long prevTime, currTime; // Used to timing the follow of the project

float CurrentValues [ARRAYLENGTH];
float CurrentAvg;


//Sensors and librairies 
AnalogSensor  CurrentSensor(CUR_SENS,0,1023,-2.5,2.5,0.1);      // Current Sensor Instance
byte          CountN_CurrSens;
AnalogSensor  DistanceStepper1(DIST1_SENS,0,1023,-2.5,2.5,5);   // Distance Sensor Stepper 1(assumed)
AnalogSensor  DistanceStepper2(DIST2_SENS,0,1023,-2.5,2.5,5);   // Distance Sensor Stepper 2(assumed)
AnalogSensor  IceSensor(ICE_SENS,0,1023,-2.5,2.5,1);            // ICE Sensor               (assumed)
AnalogSensor  LoadSensor(LOAD_SENS,0,1023,0,5.0,1/455);         // Load Sensor
byte          CountN_LoadSens;

StepperDriver Steper1(MicroSteps,DIR1,EN1,1);
StepperDriver Steper2(MicroSteps,DIR2,EN2,3);

//Messages Buffer
byte MessageBuff[2];
int  RdBuff,WtBuff;
byte Message,data;

//////////////
//////////////

//States of the project
enum States {
  Idle,     //Idle waiting for a command
  GoTop,    //Vertical Motors Go Backward
  Freeze,   //Enter this state after a stop command
  EmFreeze, //Enter this state due Error
  Plunge};  //Vertical Motors Go Forward
States state;
float tempf;



void setup() {
  //Initialize Buffer pointers
  RdBuff = 0;
  WtBuff = 0;
  //Clear the values of Analog Sampling Frequency Control Registers
  CountN_CurrSens = 0;
  CountN_LoadSens = 0;

  //Get the actual State of the system
  if (TOP_SW){
    state = Idle;
  }
  else {
    //Rotate Backward
    StartRotation(-1,DefaultRPM,DefaultRPM);
    state = GoTop;
  }
  
  //Open Serial
  Serial.begin(115200); // opens serial port, sets data rate to 115200 bps

  Serial.print("Arduino Stepper Motor Board Ready, Waiting for Commands...\n");

}

void loop() {

  if (RdBuff != WtBuff)
  {
    noInterrupts();
    Message = MessageBuff[RdBuff];
    Message = DispatchMessage(Message,&data);
    RdBuff = WtBuff;
    switch (state) 
    {
      case Idle:
        if (Message == 'P'){
          StartRotation(1,DefaultRPM,DefaultRPM);
        }
        state = Plunge;
        break;
      case GoTop:
        switch (Message)
        {
          case 'O': //Out of aligment do not change state
            if (data >0){
              DecreaseSpeed(Steper1,1.0);
            }
            else{
              DecreaseSpeed(Steper2,1.0);
            }
            break;
          case 'C': //OverCurrent
            DecreaseSpeed(Steper1,1.0);
            DecreaseSpeed(Steper2,1.0);
            break;
          case 'S': //Stop Command
            Steper1.StopRotation();
            Steper1.StepperDIS();
            Steper2.StopRotation();
            Steper2.StepperDIS();
            state = Freeze;
            break;
          case 'A': //Abnormal Stop
            Steper1.StopRotation();
            Steper1.StepperDIS();
            Steper2.StopRotation();
            Steper2.StepperDIS();
            state = EmFreeze;
            Serial.print("Abnormal Stop Unique Error Message Out of Alignment\n");
            break; 
          case 't': //Top Limitter Switch reached
            Steper1.StopRotation();
            Steper1.StepperDIS();
            Steper2.StopRotation();
            Steper2.StepperDIS();
            state = Idle;
            break;       
          default:
            break;
        }
        break;
      case Freeze:
        switch (Message)
        {
          case 'G': //Go TOP Command
            StartRotation(-1,DefaultRPM,DefaultRPM);
            state = GoTop;
            break;
          case 'P': //Plunge Command
            StartRotation(1,DefaultRPM,DefaultRPM);
            state = Plunge;
            break;
          default:
            break;
        }
        break;
      case EmFreeze:
        Steper1.StopRotation();
        Steper1.StepperDIS();
        Steper2.StopRotation();
        Steper2.StepperDIS();
        // statements
        break;
      case Plunge:
        switch (Message)
        {
          case 'C': //OverCurrent
            DecreaseSpeed(Steper1,1.0);
            DecreaseSpeed(Steper2,1.0);
            break;
          case 'W': //WOB > 150
            Steper1.StopRotation();
            Steper1.StepperDIS();
            Steper2.StopRotation();
            Steper2.StepperDIS();
            state = EmFreeze;
            Serial.print("Abnormal Stop Unique Error Message WOB > 150\n");
            break;
          case 'w': //WOB > 145
            DecreaseSpeed(Steper1,2.0);
            DecreaseSpeed(Steper2,2.0);
            break;
          case 'O': //Out of aligment
            if (data >0){
              DecreaseSpeed(Steper1,1.0);
            }
            else{
              DecreaseSpeed(Steper2,1.0);
            }
            break;
          case 'S': //Stop Command
            Steper1.StopRotation();
            Steper1.StepperDIS();
            Steper2.StopRotation();
            Steper2.StepperDIS();
            state = Freeze;
            break;
          case 'i': //Reach Ice
            Steper1.StopRotation();
            Steper1.StepperDIS();
            Steper2.StopRotation();
            Steper2.StepperDIS();
            StartRotation(-1,DefaultRPM,DefaultRPM);
            state = GoTop;
            Serial.print("Ice Reached Go Back...\n");
            break;
          case 'b': //Reach Bottom
            Steper1.StopRotation();
            Steper1.StepperDIS();
            Steper2.StopRotation();
            Steper2.StepperDIS();
            StartRotation(-1,DefaultRPM,DefaultRPM);
            state = GoTop;
            Serial.print("Bottom Limit Switch Reached, Go Back...\n");
            break;
          case 'A': //Abnormal Stop
            Steper1.StopRotation();
            Steper1.StepperDIS();
            Steper2.StopRotation();
            Steper2.StepperDIS();
            state = EmFreeze;
            Serial.print("Abnormal Stop Unique Error Message Out of Alignment\n");
            break;
          case 'G': //Go TOP
            Steper1.StopRotation();
            Steper1.StepperDIS();
            Steper2.StopRotation();
            Steper2.StepperDIS();
            StartRotation(-1,DefaultRPM,DefaultRPM);
            state = GoTop;
            break;                                                         
          default:
            break;
        }
        break;
      default:
        // statements
        break;
    }
    //interrupts();
  }
  if (Serial.available() > 0)
  {
    MessageBuff[WtBuff] = Serial.read();
    WtBuff = 1 - WtBuff;
  }
}

//DecreaseSpeed Call this function to decrease the speed of steppers by some value
void DecreaseSpeed(StepperDriver Steper,float Amount){
  float tempRPM;
  tempRPM = Steper.RPMSpeed - Amount;
  Steper.SetRPM(tempRPM);
  return;
}


//Start Rotate
void StartRotation(int dir,float RPM1,float RPM2)
{
    if (dir != 1)
    {
      if (dir !=-1)
      {
        return;
      }
    }
    Steper1.StepperEN();
    Steper2.StepperEN();
    Steper1.SetRPM(RPM1);
    Steper2.SetRPM(RPM2);
    delayMicroseconds(150);
    if (dir == -1){
        Steper1.RotateCCW();
        Steper1.RotateCCW();     
    }
    else {
        Steper1.RotateCW();
        Steper1.RotateCW();
    }
    delayMicroseconds(150);
    Steper1.StartRotation();
    Steper2.StartRotation();
  return;
}


//settings of timer 0 to have an interrupt every 500us (2KHz)
void SettingTimer0(void){
  TCCR0A = 0;// set entire TCCR0A register to 0
  TCCR0B = 0;// same for TCCR0B
  TCNT0  = 0;//initialize counter value to 0
  // set compare match register for 2khz increments
  OCR0A = 124;// = (16*10^6) / (2000*64) - 1 (must be <256)
  // turn on CTC mode
  TCCR0A |= (1 << WGM01);
  // Set CS01 and CS00 bits for 64 prescaler
  TCCR0B |= (1 << CS01) | (1 << CS00);   
  // enable timer compare interrupt
  TIMSK0 |= (1 << OCIE0A);
}

//Dispatch Message
byte DispatchMessage(byte Message,byte *data){
  byte temp,tdata;
  if (Message < 128)
  {
    *data = 0;
    return Message;
  }
  temp  = Message & 224; //224 = b1110 0000
  tdata = Message & 31;  //031 = b0001 1111
  if (tdata >= 16){
    tdata = tdata |  224; //Extend the sign bit
  }
  *data = tdata;
  return Message;
}

ISR(TIMER0_COMPA_vect){//timer0 interrupt 1kHz 
//Read Analog senseors on different sampling frequencies
//Maximum Sensors Sampling Frequencies is Timer 0 frequency (2KHz)
//and each sensot has a sampling frequency f_tmr0/N
  float SensorValue;
  //Current Sensor Job
  CountN_CurrSens ++;
  if (CountN_CurrSens>=N_CURSEN){
    CountN_CurrSens = 0;
    SensorValue = CurrentSensor.ReadAnalog();
    if (SensorValue >= CUR_TH) {
      //OverCurrent Occurs, put proper Message in the message Buffer
      MessageBuff[WtBuff] = 'C';
      //Update WtBuff, as Buff contains two elements this should toggle between 0 and 1
      WtBuff = 1 - WtBuff;
      return;
    }
  }

  //Load Sensor Job
  CountN_LoadSens ++;
  if (CountN_LoadSens>=N_LOADSEN){
    CountN_LoadSens = 0;
    SensorValue = LoadSensor.ReadAnalog();
    if (SensorValue >= LOAD_TH2) {
      //WOB is Bigger than 150
      MessageBuff[WtBuff] = 'W';
      //Update WtBuff, as Buff contains two elements this should toggle between 0 and 1
      WtBuff = 1 - WtBuff;
      return;
    }
    if (SensorValue >= LOAD_TH1) {
      // 145 =<WOB<150
      MessageBuff[WtBuff] = 'w';
      //Update WtBuff, as Buff contains two elements this should toggle between 0 and 1
      WtBuff = 1 - WtBuff;
      return;      
    }
  }
  if (BOT_SW){
    MessageBuff[WtBuff] = 'b';
    WtBuff = 1 - WtBuff;
  }
}
