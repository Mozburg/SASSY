#define pump 52
int newSpeed = 0; 
int current; 
int DcShift;

//================
//4.0.0 CHANGES
//ADDED INTEGER PARSING FUNCTION TO ADJUST DRILL SPEED
//A1 IS DRILL CURRENT
//A2 IS SYSTEM CURRENT
float Duty = 3*PI/2; //Initializes the Duty Cycle at 0%
float OnCount; int topCount; int On;


void SpeedUp(){
  if(newSpeed >= 65)
  {
    newSpeed = 65;
  }
  else{
    newSpeed = newSpeed + 10;
  }
  //receivedChars = NULL;
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


//=========PUMP CONTROLS============================
void startPump(){
digitalWrite(pump,HIGH);
}
void stopPump(){
digitalWrite(pump,LOW);
}



//=========DRILL SPEED CONTROL =======================================================================
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
