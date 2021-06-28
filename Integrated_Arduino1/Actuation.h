#include "ArduinoTimersSetup.h"


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

//===VERT VARIABLES======================================
int vertNewSpeed = 0;
float tempFloat = 0;

//===Timer Top Count Variables==========================
int tempInt = 0;

uint8_t direction = LOW;



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
  //vertSlowestSpeed(); 
}
void vertUP()
{
  //digitalWrite(6, HIGH);
  digitalWrite(36, HIGH);
  //vertSlowestSpeed();
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
void horzREV(){
  //digitalWrite(6, LOW);
  digitalWrite(38,LOW); 
}
void horzFWD()
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


void vertConfig(){
          timer_init(2, 1000, .5, 256);//pin 9 MIN FREQ = 244, MAX = 62.5Khz
          pinMode(36, OUTPUT);  
          digitalWrite(36,direction); //Vertical Direction
}

void horzConfig(){
          timer_init(4, 1000, .5, 64);//pin 7 MIN FREQ = 244, MAX = 62.5Khz 
          pinMode(38, OUTPUT);
          digitalWrite(38,direction); //Horizontal Direction
}

void winchConfig(){
          timer_init(3, 1000, .5, 64);//pin 2 
          pinMode(40, OUTPUT);
          digitalWrite(40,HIGH) ; //HIGH MEANS DROP DOWN FOR WINCH ******Why is it initialized to drop down automatically?
  }
