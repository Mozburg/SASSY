
# SASSY
SASSY-ARES READY FOR DEPLOYMENT BUILD
Changed by Jonathan Maravilla on 6-28-21
This Branch is for the core of the code, and any code ready to be used on the drill in testing 

all code in this branch works and tells you how it works
===============================================================================================
Integrated_Arduino2.ino contains the following functionalities:
-drill speed control
-heater and thermo couple control
-current consumption DAQ

How to Use It
1) Load integrated_arduino2 to its respective arduino
2) Ensure serial communication is functional by opening the port and sending a command
	Note: Serial commands must be packaged in the following format <char>.
		ex) <a> will make the speed up rpm
			<f> will make the slow down rpm
			<s> will make the drill stop spinning
3) CLOSE the serial port. If it remains open, LabVIEW cannot connect to the Arduino.
4) Next load LabVIEWArduinoControls.VI.
5) Data should be displayed. Additionally, all control commands will be sent through LabVIEW from here on out.
========================================================================================
