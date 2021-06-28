
//==== Serial Data
const byte numChars = 32;
char receivedChars[numChars];
//===
boolean newData = false;
//extern float wobRead;

void recvWithStartEndMarkers() {
    static boolean recvInProgress = false;
    static byte ndx = 0;
    char startMarker = '<';
    char endMarker = '>';
    char rc;
 
    while (Serial.available() > 0 && newData == false) {
        rc = Serial.read();

        if (recvInProgress == true) {
            if (rc != endMarker) {
                receivedChars[ndx] = rc;
                ndx++;
                if (ndx >= numChars) {
                    ndx = numChars - 1;
                }
            }
            else {
                receivedChars[ndx] = '\0'; // terminate the string
                recvInProgress = false;
                ndx = 0;
                newData = true;
            }
        }

        else if (rc == startMarker) {
            recvInProgress = true;
        }
    }
}
void showNewData() {
    if (newData == true) {
        //Serial.print("This just in ... ");
        //Serial.println(receivedChars);
    if(receivedChars[0] == 'Q')vertSpeedUp();
    else if(receivedChars[0] == 'E')vertSlowDown();
    else if(receivedChars[0] == 'Z')vertStop();
    else if(receivedChars[0] == 'A')vertMaxSpeed();
    else if(receivedChars[0] == 'D')vertSlowestSpeed();
    else if(receivedChars[0] == 'W')vertUP();
    else if(receivedChars[0] == 'S')vertDOWN();
    //HORZ
    else if(receivedChars[0] == 'R')horzSpeedUp();
    else if(receivedChars[0] == 'Y')horzSlowDown();
    else if(receivedChars[0] == 'C')horzStop();
    else if(receivedChars[0] == 'F')horzMaxSpeed();
    else if(receivedChars[0] == 'H')horzSlowestSpeed();
    else if(receivedChars[0] == 'T')horzFWD();
    else if(receivedChars[0] == 'G')horzREV();
    //WINCH
    else if(receivedChars[0] == 'U')winchSpeedUp();
    else if(receivedChars[0] == 'O')winchSlowDown();
    else if(receivedChars[0] == 'N')winchStop();
    else if(receivedChars[0] == 'J')winchMaxSpeed();
    else if(receivedChars[0] == 'L')winchSlowestSpeed();
    else if(receivedChars[0] == 'I')winchUP();
    else if(receivedChars[0] == 'K')winchDOWN();
        newData = false;
    }
}


void sendSensorDataPacket(){
  Serial.print('<');
  Serial.print(wobRead); //wob average
  Serial.print(",");
  Serial.print(vertNewSpeed); //vertical motor speed int
  Serial.print(",");
  Serial.print(distance1); //returns serial.print(sensordata) in
  Serial.print(",");
  Serial.print(distance2);
  Serial.println('>');
}
