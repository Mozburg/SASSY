# SASSY
SASSY-ARES code repository 
Updated by Jonathan Maravilla 6-28-21
LabVIEWArduinoControls IS NOW THE MAIN LABVIEW VI
How to Use
1) Load integrated_arduino1 and integrated_Arduino2 to their respective arduinos
2) Verify their functionality by communicating with the arduinos through the serial port
3) After functionality has been confirmed, run LabVIEWArduinoControls VISA
4) Assign COM ports to their respective arduino. To find COM port value, look at the COM port assigned to each Arduino in the Arduino IDE
5) Run the VI. Data should begin to load after approximately 3 seconds

Changes
Integrated controls for Arduino 1 and Arduino 2 into one VI
What's in Progress
LabVIEW requires sequential start up and a transition into a producer and consumer module. This is on the bottom of my priorities, however.