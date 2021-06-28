
//======Current sensor variables======
float val1 = 512; // variable to store the value read
float val2 = 512;
float Vmap = 0; //Map to voltage
float Vmap2 = 0;
float drillAmps = 0; //Conversion to amps
float systemAmps = 0;
int integerValue = 0;
int finalspeedValue = 0;

void current_sense_init (void){
  TCCR0A=0;
  TCCR0B=0;
  TCNT0=0;
  OCR0A = 46;  //Top count is 16M/(1024*338), for 338Hz sampling at 1024 prescale value (rounded down).
  TIMSK0 |= (1<<OCIE0A); //enable timer mask interrupts
  TCCR0A |= (1<<WGM01); //CTC mode
  TCCR0B |= (1<<CS02) | (1<<CS00); //Prescale value 1024
  pinMode(A1, INPUT);
  pinMode(A2, INPUT); // input init
}


ISR(TIMER0_COMPA_vect){
    val1 = analogRead(A1); // read the input pin
    Vmap = ((5 * val1 / 1024) - 2.5); //map analog val
    drillAmps = Vmap * 10; //map amp val
    //Serial.println(drillAmps);
    
    val2 = analogRead(A2); // read the input pin
    Vmap2 = ((5 * val2 / 1024) - 2.5); //map analog val
    systemAmps = Vmap2 * 10; //map amp val
    //Serial.println(systemAmps);
}
