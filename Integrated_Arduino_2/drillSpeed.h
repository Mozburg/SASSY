
int rpmValue = 0;
float vMapRPM = 0;
void drill_init (void){
    current=0;
    topCount = 65;  //for 120Hz min Nyquist rate for a 60Hz signal
    OCR2A = topCount; //Top value for this button
    OCR2B = 0; //Ontime of the 65Hz signal Initialized to zero
    TCCR2B |= (1<<CS21); //Pre-scale = 8   16M/(8*256) = 7.8125kHz and start the clock
  }
void fastp_init (void){
  //This specifically uses PD3 as the PWM output pin.
  pinMode(9,OUTPUT); //Set Port D pin 9 to output. Connecting OC2B to the pin (Speaker Fast PWM pin).
  TCCR2A |= (1<<WGM21) | (1<<WGM20); //Setting up wave generation mode for fast PWM. Still need to set WGM22
  TCCR2B |= (1<<WGM22); //Fast PWM wave generation mode 7 set. OCR2A is TOP.
  TCCR2A |= (1<<COM2B1); //COM2B0 is default 0. COM2B1 set for non-inverting operation.
}
void rpm_init(void)
{
  TCCR1A=0;
  TCCR1B=0;
  TCNT1=0;
  OCR1A = 5000;                       //Top count is 16M/(64*50), where 4 is 50hz (250ms sample rate) and 54 is the pre-scale value
  TIMSK1 |= (1<<OCIE1A);            //enable timer mask interrupts
  TCCR1A |= (1<<COM1A1);            //Clear on compare match mode
  TCCR1B |= (1<<WGM12);             //CTC mode FOR TIMER 1
  TCCR1B |= (1<<CS11) | (1<<CS10);  //Prescale value 64 
}
ISR(TIMER1_COMPA_vect){
  rpmValue = analogRead(A5);
  vMapRPM = (5.0 * rpmValue / 1023.0) - 2.5; //map analog val
}
