
# SASSY
SASSY-ARES READY FOR DEPLOYMENT BUILD

This Branch is for the core of the code, and any code ready to be used on the drill in testing 

all code in this branch works and tells you how it works
===============================================================================================

#PUMP CODE 
 Pump Control with analog comparator, interrupt & ACO flag

Compares voltage sensed at pins 6 (AIN0) and 7 (AIN1).  AIN0 is the positve input and AIN1 is the negative input to the comparator. Once the voltage on AIN0(+) rises above the voltage
sensed at AIN1(-), the comparator output becomes high and triggers the interrupt to run. The Pump control output is set to match the ACO flag in the interrupt.

The circuit:

Reference voltage can be set with a voltage divider, but will result in leakage and excess power consumption. The water sensor bias will suffice, and an option would be to add a potentiometer to
change the threshold voltage of the transistor if we need a different refference. 

5V, GND (X2), Pin 7 unconnected (assumed value 1000 as comparator reference), Pin 6 Water Sensor input to arduino.

NOTE: This code still needs the PSI sensor integrated to trigger another pin to control the purge valve.
========================================================================================
Download the Library branch for header files
