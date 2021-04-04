/*
 * Code sourced by Alan, edited by Brandon Mosburg, property of the creators.
int analogPin = A1; // potentiometer wiper (middle terminal) connected to analog pin 3
 outside leads to ground and +5V 
 */

 
//==========================================================================================================

float val = 512; // variable to store the value read
float Vmap = 0; //Map to voltage
float Amps = 0; //Conversion to amps

//==========================================================================================================
void setup() {
  TCCR0A=0;
  TCCR0B=0;
  TCNT0=0;
  
  OCR0A = 46;  //Top count is 16M/(1024*338), for 338Hz sampling at 1024 prescale value (rounded down).
  TIMSK0 |= (1<<OCIE0A); //enable timer mask interrupts
  TCCR0A |= (1<<WGM01); //CTC mode
  TCCR0B |= (1<<CS02) | (1<<CS00); //Prescale value 1024
  
  pinMode(A1, INPUT); // input init
  Serial.begin(115200); // setup serial
  //delay(1); // delay for arduino serial init
}
//=========================================================================================================
void loop() {
}


ISR(TIMER0_COMPA_vect){

    val = analogRead(A1); // read the input pin
    Vmap = ((5 * val / 1024) - 2.5); //map analog val
    Amps = Vmap * 10; //map amp val
    Serial.print("Amps: ");
    Serial.print(Amps, 4); // send val to serial
    Serial.print("\n");
}
