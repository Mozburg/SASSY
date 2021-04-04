#include <avr/io.h>

/*
 Pump Control with analog comparator, interrupt & ACO flag

Compares voltage sensed at pins 6 (AIN0) and 7 (AIN1).  AIN0 is the positve input and AIN1 is the negative input to the comparator. Once the voltage on AIN0(+) rises above the voltage
sensed at AIN1(-), the comparator output becomes high and triggers the interrupt to run. The Pump control output is set to match the ACO flag in the interrupt.

The circuit:
* Reference voltage can be set with a voltage divider, but will result in leakage and excess power consumption. The water sensor bias will suffice, and an option would be to add a potentiometer to
* change the threshold voltage of the transistor if we need a different refference. 
* 
* 5V, GND (X2), Pin 7 unconnected (assumed value 1000 as comparator reference), Pin 6 Water Sensor input to arduino.
*/

#define Pump 52 //GPIO pin 52

void setup() {
 // initialize serial communications at 9600 bps:
 Serial.begin(9600);
 ACSR =
   (0 << ACD) |    // Analog Comparator: Enabled
   (0 << ACBG) |   // Analog Comparator Bandgap Select: AIN0 is applied to the positive input
   (0 << ACO) |    // Analog Comparator Output: Off
   (1 << ACI) |    // Analog Comparator Interrupt Flag: Clear Pending Interrupt
   (1 << ACIE) |   // Analog Comparator Interrupt: Enabled
   (0 << ACIC) ;   // Analog Comparator Input Capture: Disabled

 pinMode(Pump, OUTPUT);   // indicates status of comparator output
}

void loop() {               
}

ISR(ANALOG_COMP_vect ) {
   digitalWrite(Pump, (ACSR & (1 << ACO)));   //Set the output at pin 9 equal to the comparator flag value 
}
