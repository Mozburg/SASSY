
# SASSY
SASSY-ARES READY FOR DEPLOYMENT BUILD

Integrated_Arduino1.ino

File contains
-stepper motor controls
-load cell sensor DAQ
-LiDAR sensors DAQ
-limit switches
-rpm sensor
-data packet manager

How to Use
1) Load Integrated_Arduino1.ino to its respective arduino.
2) Confirm serial communication is functional by opening the serial port and testing stepper motor controls. For a list of commands, refer to packetManager.h located inside of the integrated_arduino1 file.
3) Serial data must be packaged with '<' markers
	ex) 
		<s> will make the steppers go down
		<d> will make the steppers move in desired direction at the SLOWEST speed
4) CLOSE the serial port. LabVIEW cannot control the Arduino if the serial port is open.
5) Load LabVIEWArduinoControls VI and run it.
6) Data should be displaying on the charts. Buttons will now control stepper motors instead of serial commands.
	

===============================================================================================
