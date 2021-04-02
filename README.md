# SASSY
SASSY-ARES code repository 

LabVIEWSensorDisplayandDrillControl Rev1.0.1
How to Use
1) Load Incremental_Drill_Speed_REV1.0.0 onto the arduino
2) Boot up the VI and make sure COM port and COM rate are set to appropriate values.
3) Click the white arrow on the top left to run the code assuming previous settings have been set. CODE WILL NOT WORK IF YOU TRY TO UPDATE SETTINGS DURING RUN TIME!
4) Accelerate button speeds up drill by increasing our duty cycle by 10.
5) Decellerate slows down drill by decreasing our duty cycle by 10.
6) Emergency stop brings our duty cycle to 0

Changes
Removed VISA close from all events exceot for LabVIEW Stop as it was causing sporadic loss of control.
Added our own Emergency stop button in place of LabVIEWs as it caused loss of control as well.

What's in Progress
Sensor Visualization and appropriate means of ending LabVIEW's loop to free up the Arduino's COM port
