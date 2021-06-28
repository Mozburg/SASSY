#include "Adafruit_MAX31855.h"
#define pump 52

//=======Pump Initialization============================
void pumpInit(){
pinMode(pump, OUTPUT);
}
 //======HEATER & THERMOCOUPLE CONTROL VARIABLES=========
//pins
int tc = 39;       //serial thermocouple data output
int cs = 37;       //Active-Low Chip Select. Set CS low to enable the serial interface
int clk = 7;      //serial clock input (timer 4)
int hCtl = 2;     //PWM out to heater (timer 3)
double c = 0;

float HDC = 0.0;
float hProp = 0.0;
int hDuty = 0;
 
//smoothing
const int numReadings = 3;      //number of readings to average
int readings[numReadings];      //the readings from the analog input
int readIndex = 0;              //the index of the current reading
int total = 0;                  //the running total
int averageTemp = 0;                //the average 

//initializing values
double error = 0;
int out = 0;

 //initialize thermocouple amp board
 Adafruit_MAX31855 thermocouple(clk,cs,tc);

 //===========INITIALIZE THERMOCOUPLE AND HEATER=================================================
 void heaterAndThermoInit(void){
 
    pinMode(tc,INPUT);
    pinMode(cs,OUTPUT);
    digitalWrite(cs,LOW);
    //output pin setup
    timer_init(3, 1000, 0.0, 64);//pin 2 Heater Control/ISR
    TIMSK4 |= (1<<OCIE4A); //Enable timer 4 mask interupts
    timer_init(4, 100, 0.5, 64);//pin 7 Thermocouple Clock
    //pinMode(hCtl,OUTPUT);
    //begin reading thermocouple
    Serial.print("Initializing thermocouple...");
    if (!thermocouple.begin()) {
      Serial.println("ERROR");
    }
    Serial.println("DONE");
    //initialize smoothing matrix to 0:
    for (int thisReading = 0; thisReading < numReadings; thisReading++) {
      readings[thisReading] = 0;
    }
    
 }


//!!!I think this setup will work!!!

ISR(TIMER4_COMPA_vect) {
  //read thermocouple
  double c = thermocouple.readCelsius();
 // Serial.println(c);
   //smoothing
   
   if (!isnan(c)) {
     total = total - readings[readIndex];
     readings[readIndex] = c;
     total = total + readings[readIndex];
     readIndex = readIndex + 1;
     
   if (readIndex >= numReadings) {
   readIndex = 0;
  }

  averageTemp = total / numReadings;
//  Serial.println(average);
   }
     
  //proportional control: Adjust timer 3 duty cycle base on error from the thermocouple
  error = 200 - average; // this adjusts temperature in C
      if (error > 50) {      //error greater than 50 deg celsius turns on the heater full.
        OCR3B = OCR3A;       //= hDuty; //stored bottom count for duty cycle 
      } 
      else if (error > 0) {
          HDC = HDC + error/50;
          hProp = HDC*OCR3A; //Proportion of the top count for ontime
          hDuty = hProp;     //float to int
        //out = 120 + error/50 * 125;
        //Serial.println(out);
        OCR3B = hDuty;
      }
      else if (error <= 0) {
        OCR3B = 0;
      }
      
  //timer 3 for the ISR version and run as low frequency as possible. Trying 1sec 1st. 
  //check WOB code for ISR implementation with CTC mode but Fast PWM should work
}
