#include <avr/io.h>
#define f_CLK 16000000
//VALUES NOT USED
#define timers_0_2_reg_size 256
#define timers_1_3_4_reg_size 1024
//#define pre_scale_value 8
//the prescalar will be decided by the user, which will calculate top count.

float top_count_float  =0;
int top_count_int;
float bottom_count_float = 0;
int bottom_count_int = 0;

int val = 0;
int analogPin = A0;
bool flag = LOW;
//The initialization value must be greater than 7.8125kHz otherwise IT WONT WORK
//input duty cycle as a decimal value between 0 and 1
//====PRE-SCALE VALUES ACCEPTABLE BY INIT FUNCTION=======
//8, 64, 256, 1024
//The closer the init value is to the pre-scaled value, the lower the resolutio
// 7.6Hz per jump
void setup() {
  // put your setup code here, to run once:
    Serial.begin(115200);
  timer_init(0,1000,.5,256); // pin 4
  timer_init(1, 1000, .5, 64);//pin 12
  timer_init(2, 1000, .5, 256);//pin 9 MIN FREQ = 244, MAX = 62.5Khz
  timer_init(3, 1000, .5, 64);//pin 2
  timer_init(4, 1000, .5, 64);//pin 7
}

void loop() {
  // put your main code here, to run repeatedly:
  //Serial.println("still running");
          //Serial.println(top_count_int);
}
//if pre_scale_valeu doesnt work with define, we will use an int instead
//duty_cycle_multiplier is a number between 0 and 1 that will control the final duty cycle of our PWM output
//we take duty_cycle_multiplier and then multiply it by our top_count to obtain OCRnB
void timer_init(int timer_number, float freq_op, float duty_cycle_multiplier, float pre_scale_value){
  switch(timer_number) {
      case 0 :
        TCCR0A |= (1<<WGM01) | (1<<WGM00); //Setting up wave generation moe for fast PWM. Still need to set WGM22
        TCCR0B |= (1<<WGM02); //Fast PWM wave generation mode 7 set. OCR2A is TOP.
        TCCR0A |= (1<<COM0B1); //COM2B0 is default 0. COM2B1 set for non-inverting operation.
        //these if statements are responsible for setting the correct pre-scalar depending on the pre_scalar parameter
        pinMode(4, OUTPUT);
        //top_count_float will calculate a top count that will output a frequency specified by timer_init
        top_count_float = ((f_CLK/freq_op)/pre_scale_value)-1;
        Serial.println(top_count_float);
        //bottom_count will be the value given to OCR2B for pwm generation
        bottom_count_float = top_count_float*duty_cycle_multiplier;
        //convert floating values to integer as OCRnA and OCRnB only accepts integers
        top_count_int = int(top_count_float);
        Serial.println(top_count_int);
        bottom_count_int = int(bottom_count_float);
        OCR0A = top_count_int;
        Serial.println("OCR2A");
        Serial.println(OCR0A);
        OCR0B = bottom_count_int;
        //Serial.println(OCR1A);
        //Serial.println(OCR1B);
        TCCR0B &=~(1<<CS00);
        TCCR0B &=~ (1<<CS01); 
        TCCR0B &=~(1<<CS02);
        if(pre_scale_value == 0)
          TCCR0B &= TCCR0B; //Pre-scale = 0   16M/256 = 62.5kHz and start the clock ***I think this or 16MHz
        else if(pre_scale_value == 8)
        {
          Serial.println("hacker voie: I'm in");
          TCCR0B |= (1<<CS01); //Pre-scale = 8   16M/(8*256) = 7.8125kHz and start the clock
        }
        else if(pre_scale_value ==64){
          Serial.println("I NEED HELP I SHOULDNT BE HERE!");
          TCCR0B |= (1<<CS01)| (1<<CS00); //Pre-scale = 64   16M/(64*256) = 976.56Hz and start the clock
        }
        else if (pre_scale_value == 256)
          TCCR0B |= (1<<CS02); //Pre-scale = 64   16M/(64*256) = 976.5625Hz and start the clock
        else if (pre_scale_value == 1024)
          TCCR0B |= (1<<CS02)|(1<<CS00); //Pre-scale = 8   16M/(1024*256) = 61Hz and start the clock
        //TCCR0B |= (1<<CS01); //Pre-scale = 8   16M/(8*256) ,= 7.8125kHz and start the clock

        break;
      case 1 :
        TCCR1A |= (1<<WGM11) | (1<<WGM10); //Setting up wave generation mode for fast PWM. Still need to set WGM22
        TCCR1B |= (1<<WGM12)|(1<<WGM13); //Fast PWM wave generation mode 7 set. OCR2A is TOP.
        TCCR1A |= (1<<COM1B1); //COM2B0 is default 0. COM2B1 set for non-inverting operation.
        pinMode(12,OUTPUT);
        //top_count_float will calculate a top count that will output a frequency specified by timer_init
        top_count_float = (f_CLK/((freq_op)*(pre_scale_value)))-1;
        //bottom_count will be the value given to OCR2B for pwm generation
        bottom_count_float = top_count_float*duty_cycle_multiplier;
        //convert floating values to integer as OCRnA and OCRnB only accepts integers
        top_count_int = int(top_count_float);
        bottom_count_int = int(bottom_count_float);
        OCR1A = top_count_int;
        OCR1B = bottom_count_int;
        TCCR1B &=~(1<<CS00);
        TCCR1B &=~ (1<<CS01); 
        TCCR1B &=~(1<<CS02);
        if(pre_scale_value == 0)
          TCCR1B |= (1<<CS10); //Pre-scale = 0   16M/256 = 62.5kHz and start the clock ***I think this or 16MHz
        else if(pre_scale_value ==8)
          TCCR1B |= (1<<CS11); //Pre-scale = 8   16M/(8*256) = 7.8125kHz and start the clock
        else if(pre_scale_value ==64)
          TCCR1B |= (1<<CS11)| (1<<CS10); //Pre-scale = 64   16M/(64*256) = 976.56Hz and start the clock
        else if (pre_scale_value == 256)
          TCCR1B |= (1<<CS12); //Pre-scale = 256   16M/(64*256) = 244.14Hz and start the clock
        else if (pre_scale_value == 1024)
          TCCR1B |= (1<<CS12)|(1<<CS10); //Pre-scale = 8   16M/(8*256) = 7.8125kHz and start the clock
        //TCCR1B |= (1<<CS11); //Pre-scale = 8   16M/(8*256) = 7.8125kHz and start the clock
        break;
      case 2 :
        TCCR2A |= (1<<WGM21) | (1<<WGM20); //Setting up wave generation mode for fast PWM. Still need to set WGM22
        TCCR2B |= (1<<WGM22); //Fast PWM wave generation mode 7 set. OCR2A is TOP.
        TCCR2A |= (1<<COM2B1); //COM2B0 is default 0. COM2B1 set for non-inverting operation.
        TCCR2B &=~(1<<CS00);
        TCCR2B &=~ (1<<CS01); 
        TCCR2B &=~(1<<CS02);
        if(pre_scale_value == 0)
          TCCR2B |= (1<<CS20); //Pre-scale = 0   16M/256 = 62.5kHz and start the clock ***I think this or 16MHz
        else if(pre_scale_value ==8)
          TCCR2B |= (1<<CS21); //Pre-scale = 8   16M/(8*256) = 7.8125kHz and start the clock
        else if(pre_scale_value ==32)
          TCCR2B |= (1<<CS21)| (1<<CS20); //Pre-scale = 32   16M/(32*256) = 1.953kHz and start the clock
        else if (pre_scale_value == 64)
          TCCR2B |= (1<<CS22); //Pre-scale = 64   16M/(64*256) = 976.5625Hz and start the clock
        else if (pre_scale_value == 128)
          TCCR2B |= (1<<CS22)|(1<<CS20); //Pre-scale = 128   16M/(128*256) =     Hz and start the clock
        else if (pre_scale_value == 256)
          TCCR2B |= (1<<CS22)|(1<<CS21); //Pre-scale = 256   16M/(256*256) =     Hz and start the clock
        else if (pre_scale_value == 1024)
          TCCR2B |= (1<<CS22)|(1<<CS20)|(1<<CS21); //Pre-scale = 1024   16M/(1024*256) =     Hz and start the clock
        //TCCR2B |= (1<<CS21); //Pre-scale = 8   16M/(8*256) = 7.8125kHz and start the clock
        pinMode(9,OUTPUT);
        top_count_float = (f_CLK/((freq_op)*(pre_scale_value)))-1;
        //bottom_count will be the value given to OCR2B for pwm generation
        bottom_count_float = top_count_float*duty_cycle_multiplier;
        //convert floating values to integer as OCRnA and OCRnB only accepts integers
        top_count_int = int(top_count_float);
        bottom_count_int = int(bottom_count_float);
        OCR2A = top_count_int;
        OCR2B = bottom_count_int;
        break;
      case 3 :
        TCCR3A |= (1<<WGM31) | (1<<WGM30); //Setting up wave generation mode for fast PWM. Still need to set WGM22
        TCCR3B |= (1<<WGM32)|(1<<WGM33); //Fast PWM wave generation mode 15 set. OCR2A is TOP.
        TCCR3A |= (1<<COM3B1); //COM2B0 is default 0. COM2B1 set for non-inverting operation.
        TCCR3B &=~(1<<CS00);
        TCCR3B &=~ (1<<CS01); 
        TCCR3B &=~(1<<CS02);
        if(pre_scale_value == 0)
          TCCR3B |= (1<<CS30); //Pre-scale = 0   16M/256 = 62.5kHz and start the clock ***I think this or 16MHz
        else if(pre_scale_value ==8)
          TCCR3B |= (1<<CS31); //Pre-scale = 8   16M/(8*256) = 7.8125kHz and start the clock
        else if(pre_scale_value ==64)
          TCCR3B |= (1<<CS31)| (1<<CS30); //Pre-scale = 64   16M/(64*256) = 976.56Hz and start the clock
        else if (pre_scale_value == 256)
          TCCR3B |= (1<<CS32); //Pre-scale = 256   16M/(64*256) = 244.14Hz and start the clock
        else if (pre_scale_value == 1024)
          TCCR3B |= (1<<CS32)|(1<<CS30); //Pre-scale = 8   16M/(8*256) = 7.8125kHz and start the clock
        //TCCR3B |= (1<<CS31); //Pre-scale = 8   16M/(8*256) = 7.8125kHz and start the clock
        pinMode(2,OUTPUT);
        //top_count_float will calculate a top count that will output a frequency specified by timer_init
        top_count_float = (f_CLK/((freq_op)*(pre_scale_value)))-1;
        //bottom_count will be the value given to OCR2B for pwm generation
        bottom_count_float = top_count_float*duty_cycle_multiplier;
        //convert floating values to integer as OCRnA and OCRnB only accepts integers
        top_count_int = int(top_count_float);
        bottom_count_int = int(bottom_count_float);
        OCR3A = top_count_int;
        OCR3B = bottom_count_int;
        break;
      case 4 :
        TCCR4A |= (1<<WGM41) | (1<<WGM40); //Setting up wave generation mode for fast PWM. Still need to set WGM22
        TCCR4B |= (1<<WGM42)|(1<<WGM43); //Fast PWM wave generation mode 7 set. OCR2A is TOP.
        TCCR4A |= (1<<COM4B1); //COM2B0 is default 0. COM2B1 set for non-inverting operation.
        TCCR4B &=~(1<<CS00);
        TCCR4B &=~ (1<<CS01); 
        TCCR4B &=~(1<<CS02);
        if(pre_scale_value == 0)
          TCCR4B |= (1<<CS40); //Pre-scale = 0   16M/256 = 62.5kHz and start the clock ***I think this or 16MHz
        else if(pre_scale_value ==8)
          TCCR4B |= (1<<CS41); //Pre-scale = 8   16M/(8*256) = 7.8125kHz and start the clock
        else if(pre_scale_value ==64)
          TCCR4B |= (1<<CS41)| (1<<CS40); //Pre-scale = 64   16M/(64*256) = 976.56Hz and start the clock
        else if (pre_scale_value == 256)
          TCCR4B |= (1<<CS42); //Pre-scale = 256   16M/(64*256) = 244.14Hz and start the clock
        else if (pre_scale_value == 1024)
          TCCR4B |= (1<<CS42)|(1<<CS40); //Pre-scale = 8   16M/(8*256) = 7.8125kHz and start the clock
        //TCCR4B |= (1<<CS41); //Pre-scale = 8   16M/(8*256) = 7.8125kHz and start the clock
        pinMode(7,OUTPUT);
        //top_count_float will calculate a top count that will output a frequency specified by timer_init
        top_count_float = (f_CLK/((freq_op)*(pre_scale_value)))-1;
        //bottom_count will be the value given to OCR2B for pwm generation
        bottom_count_float = top_count_float*duty_cycle_multiplier;
        //convert floating values to integer as OCRnA and OCRnB only accepts integers
        top_count_int = int(top_count_float);
        bottom_count_int = int(bottom_count_float);
        OCR4A = top_count_int;
        OCR4B = bottom_count_int;
        break;
      default :
        Serial.println("Invalid timer number");
  }  
}
