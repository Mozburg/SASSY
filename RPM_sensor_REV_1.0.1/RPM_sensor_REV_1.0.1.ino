#include <current.h>
#include <loadcell.h>
#include <rpm.h>
#include <avr/io.h>
#define PI 3.1415926535897932384626433832795
#define frequency 4 //Value in Hz
#define period 163 //Value in Hz
#define led 2

/*This code uses Arduino Uno digital pwm pin 3 and gnd to output a pwm signal through the solidstate relay that controls the drill speed.
Can substitute an LED or probably a servo motor for low power testing. Need to take rpm data normalized between integers 0 - 120 to establish
current and newspeed values.
*/

float Duty = 3*PI/2; //Initializes the Duty Cycle at 0%
float OnCount; int TopCount; int On;
//======== Simulating RPM Sensor Feedback ================
float rpmMax = 1000; //arbitrary value
float rpm = 0; //current value (arbitrary)
//========================================================
int newspeed; int currentSpeed;
int DcShift;

/* Current Sensor Variables */
float current, currentHigh;
bool highCurrent = false;

/*RPM Sensor Variables*/
float rotation, total, rate, calculatedRPM;
int t = 0, count = 0, hist = 0;

float currentRead = 0; //Conversion to amps
int rpmRead = 0;
float wobRead = 0.0;
Current currentSensor;
RPM rpmSensor;
Loadcell wobSensor;
String p1=" ";
/* Variables for LabVIEW */
/*
 * Removed PWM functionalities. Used only for sensor DAQ
 */
char command;
String string;
void setup() {
          //fastp_init(); //set registers for fast pwm, mode 7, non-inverting
          drill_init(); //Set top value for 65Hz operation, initialize at 0% duty cycle,
          sensor_init();
          string = "";
          while(!Serial){}
          pinMode(A5, INPUT); // input init
          pinMode(A3, INPUT); // input init
          pinMode(A1, INPUT); // input init
          Serial.begin(115200); // setup serial
          sei();
        }
void loop() {
  //newspeed = random(0,65); //***********simulating external command to change speed. Value is an integer 0-65 from normalized RPM values.
    //Serial.println(currentRead);
    Serial.println(wobRead);
    //Serial.println(calculatedRPM);
    //Serial.println(rpmSensor.GetRPM());
  //currentSpeed = speedCorrect(currentSpeed, newspeed); //takes current rpm data and the desired speed to establish new speed. Will not need this function to return the current value once integrated
  //with RPM sensor.
  }

void drill_init (void){
    currentSpeed=0;
    TopCount = 65;  //for 120Hz min Nyquist rate for a 60Hz signal
    OCR2A = TopCount; //Top value for this button
    OCR2B = 0; //Ontime of the 65Hz signal Initialized to zero
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
    currentRead = currentSensor.GetCurrent();
    wobRead = wobSensor.GetLoad();
    rpmRead = rpmSensor.GetRPM();
          
     //boolean alegra to replace conditionals
    rotation += 0.25 * ((count < frequency) && (t < period) &&  (rpmRead && !hist));
    hist = rpmRead * ((count < frequency) && (t < period)) + hist * ((count < frequency) && !(t < period)) + hist * !(count < frequency);
    t += ((count < frequency) && (t < period));

    t = t * ((count < frequency) && (t < period)) + t * !(count < frequency);
    rate += (rotation * 60.0) * ((count < frequency) && !(t < period));
    rotation = rotation * !((count < frequency) && !(t < period));
    count += ((count < frequency) && !(t < period));

    count = count * (count < frequency);
    calculatedRPM = rate * !(count < frequency) + calculatedRPM * (count < frequency);
    rate = rate * (count < frequency);
          
    /*   original code, tested and works
    if(count < frequency)                //Loops for 1s total
    {
      if(t < period)                             //Loops for 1s/frequency
      {
          if(rpmRead && !hist) rotation += 0.25;  //Increases rotation each time voltage surpasses threshold
          hist = rpmRead;                                         //Used to verify an increase above threshold
          t++;
      }
      else {
        t = 0;
        rate += (rotation * 60.0);
        rotation = 0.0;                     //Finds the number of roations for each period
        count++;
      }
    }
    else {
      count = 0;
      calculatedRPM = rate;
      rate = 0.0;
    }*/
    sei();
}
