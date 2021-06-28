// TIMER ASSIGNMENTS
/*
 * timer 2 = steppers
 * timer 4 = horz
 * timer 3 = winch
 * hello
 */
#include "LIDARS.h"
#include <avr/io.h>
#include "Actuation.h"
//#include "ArduinoTimersSetup.h"
#include "LimitSwitches.h"
#include "LoadCell.h"
#include "packetManager.h"

#define f_CLK 16000000          
#define PI 3.1415926535897932384626433832795

//=============Potentially Redundant Variables========================================
//has incremental speed controls
float Duty = 3*PI/2; //Initializes the Duty Cycle at 0%
float OnCount; int TopCount = 255; int On;


int current; 
int DcShift;

int comPortInitialization;

int load_cell_callibration = 0;

int top_switch = 0;
int bottom_switch = 0;

const unsigned long eventInterval = 5000;
unsigned long previousTime = 0;
int inputPin = A3;
//PIN 9 FOR PWM
//==================End Of Potentially Redundant Variables===========================

void setup() {
          //=====Start Serial Communication=======
          Serial.begin(115200); 
          //==== LIDAR SETUP=====
         liDARInit();
         setID();
         lidarConfig();
          //=====Configure Actuators============
          vertConfig();
          horzConfig();
          winchConfig();
//          lidarConfig();
          
          //======Initialize Load Cell===========
          sensor_init();
          
          //load_cell_callibration = wobSensor.GetLoad();
          
          //=====Configure Limit Switches=========
          LimSwInit();
         
        }
void loop() {
  /* Updates frequently */
    //Load Cell averaging and pecking operation
    if(readLiDAR == true){ // timer1 ISR toggles this flag at an interval of 250 ms
        read_dual_sensors(); //ISR has been triggered, meaning it is time to read the LiDAR sensors
        readLiDAR = false; //after new data is acquired, set value to false to stop reading from LiDAR
        //will wait until ISR triggers reading of LiDAR again
      }
    wobAveAndPecking();
    //Limit Switch Dependancies
    //LimSwCntrl();
    //Look for remote commands, packet management, remote control
    recvWithStartEndMarkers();
    showNewData();
    updateTimerTopCounts();
    sendSensorDataPacket(); // sends <wobData, motorSpeed,LiDAR 1> to LabVIEW  
  }
