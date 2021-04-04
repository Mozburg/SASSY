const byte adcPin = 1;  // A0

float result;

// ADC complete ISR
ISR (ADC_vect)
  {
    result = ADC;
    result = (5*(result)/1024-2.5)*10;
    Serial.println (result);
 
  }  // end of ADC_vect
  
EMPTY_INTERRUPT (TIMER1_COMPB_vect);

void setup ()
  {
  pinMode(A1,INPUT);  
  Serial.begin(115200); // set baudrate
  TCCR1B  = bit (CS12) | bit (WGM12);  // CTC, prescaler of 256
  TIMSK1  = bit (OCIE1B); 
  OCR1A   = 14;    
  OCR1B   = 14; // 16m/(256*13*338)=14.22 ~=14 for 343Hz Sample rate.

  ADMUX   = bit (REFS0) | (adcPin & 7);
  ADCSRB  = bit (ADTS0) | bit (ADTS2);  // Timer/Counter1 Compare Match B
  ADCSRA =  bit (ADEN) | bit (ADIE) | bit (ADIF)| bit (ADPS2) | bit (ADATE); // turn ADC ON
  
}
void loop () {

   

  
}
