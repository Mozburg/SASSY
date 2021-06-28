//WOB SERNSORS
int Wval; float voltage, weight, force, VtoLBS, Vload;
#define gain 455    //Voltage gain of instrumentation amplifier
#define maxWeight 100
#define sensitivity 0.03

//Averaging Filter for LoadCell Variables
const int numReadings = 200;
int readings[numReadings];      // the readings from the analog input
int readIndex = 0;              // the index of the current reading
int total = 0;                  // the running total
float average = 0;                // the average

float wobRead = 0;
bool wobOverLoad = false;
bool isrFlag = false;
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
  Wval = analogRead(A4);                    //Value read from loadcell
  voltage = ((Wval) * 5.0 / 1023.0);       //Voltage seen at output of instrumentation amplifier
  Vload = voltage / gain;                         //Voltage out of load cell
  wobRead = ((Vload * maxWeight) / sensitivity) * 4.45; //Convert Voltage to Newtons
  sei();
}

void wobAveAndPecking(){
 // calculate the average:
  // subtract the last reading:
  total = total - readings[readIndex];
  // read from the sensor:
  readings[readIndex] = wobRead;
  // add the reading to the total:
  total = total + readings[readIndex];
  // advance to the next position in the array:
  readIndex = readIndex + 1;

  // if we're at the end of the array...
  if (readIndex >= numReadings) {
    // ...wrap around to the beginning:
    readIndex = 0;
  }

  // calculate the average:
  average = total / numReadings;
  // send it to the computer as ASCII digits
 
  //Serial.println(wobOverLoad);
  if (average >= 125){
    vertUP();
    wobOverLoad = true;
  }

  if (average <= 60  && wobOverLoad == true){
    vertDOWN();
    wobOverLoad = false;
  }
}
