#include <avr/io.h>
#define f_CLK 16000000
#define timers_0_2_reg_size 256
#define timers_1_3_4_reg_size 1024
//#define pre_scale_value 8
//the prescalar will be decided by the user, an INPUT. Based on INPUT we calculate top count.
//INPUTS timer number, pre-scale value, copy comments regarding pin to timer
//user can look in the README and see what pins belong to what timer
//pinmode pin and number we assign pwm mode 7
//duty cycle is an input to the function
float top_count_float  =0;
int top_count_int;
float bottom_count_float = 0;
int bottom_count_int = 0;

int test_val = 0;
int i = 0;
bool flag = LOW;
//The timer is prescaled to 7.8125kHz
//The initialization value must be lower than 7.8125kHz otherwise IT WONT WORK
//input duty cycle as a decimal value between 0 and 1
//====PRE-SCALE VALUES ACCEPTABLE BY INIT FUNCTION=======
//8, 64, 256, 1024
//The closer the init value is to the pre-scaled value, the lower the resolutio
// 7.6Hz per jump
void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  timer_init(0, 600, .3, 8);
  timer_init(1, 600, .3, 8);
  timer_init(2, 600, .3, 8);
  timer_init(3, 600, .3, 8);
  timer_init(4, 600, .3, 8);
}

void loop() {
  // put your main code here, to run repeatedly:
  
}
//if pre_scale_valeu doesnt work with define, we will use an int instead
//duty_cycle_multiplier is a number between 0 and 1 that will control the final duty cycle of our PWM output
//we take duty_cycle_multiplier and then multiply it by our top_count to obtain OCRnB
void timer_init(int timer_number, float freq_op, float duty_cycle_multiplier, int pre_scale_value){
  switch(timer_number) {
      case 0 :
        TCCR0A |= (1<<WGM01) | (1<<WGM00); //Setting up wave generation moe for fast PWM. Still need to set WGM22
        TCCR0B |= (1<<WGM02); //Fast PWM wave generation mode 7 set. OCR2A is TOP.
        TCCR0A |= (1<<COM0B1); //COM2B0 is default 0. COM2B1 set for non-inverting operation.
        
        TCCR0B |= (1<<CS01); //Pre-scale = 8   16M/(8*256) ,= 7.8125kHz and start the clock
        pinMode(4, OUTPUT);
        //top_count_float will calculate a top count that will output a frequency specified by timer_init
        top_count_float = f_CLK/(freq_op)/(pre_scale_value*timers_0_2_reg_size);
        //bottom_count will be the value given to OCR2B for pwm generation
        bottom_count_float = top_count_float*duty_cycle_multiplier;
        //convert floating values to integer as OCRnA and OCRnB only accepts integers
        top_count_int = int(top_count_float);
        bottom_count_int = int(bottom_count_float);
        Serial.println(top_count_int);
        Serial.println(bottom_count_int);
        Serial.println("---");
        OCR0A = top_count_int;
        OCR0B = bottom_count_int;

        break;
      case 1 :
        TCCR1A |= (1<<WGM11) | (1<<WGM10); //Setting up wave generation mode for fast PWM. Still need to set WGM22
        TCCR1B |= (1<<WGM12); //Fast PWM wave generation mode 7 set. OCR2A is TOP.
        TCCR1A |= (1<<COM1B1); //COM2B0 is default 0. COM2B1 set for non-inverting operation.
        TCCR1B |= (1<<CS11); //Pre-scale = 8   16M/(8*256) = 7.8125kHz and start the clock
        pinMode(12,OUTPUT);
        //top_count_float will calculate a top count that will output a frequency specified by timer_init
        top_count_float = f_CLK/(freq_op)/(pre_scale_value*timers_1_3_4_reg_size);
        //bottom_count will be the value given to OCR2B for pwm generation
        bottom_count_float = top_count_float*duty_cycle_multiplier;
        //convert floating values to integer as OCRnA and OCRnB only accepts integers
        top_count_int = int(top_count_float);
        bottom_count_int = int(bottom_count_float);
        Serial.println(top_count_int);
        Serial.println(bottom_count_int);
        Serial.println("---");
        OCR1A = top_count_int;
        OCR1B = bottom_count_int;
        break;
      case 2 :
        TCCR2A |= (1<<WGM21) | (1<<WGM20); //Setting up wave generation mode for fast PWM. Still need to set WGM22
        TCCR2B |= (1<<WGM22); //Fast PWM wave generation mode 7 set. OCR2A is TOP.
        TCCR2A |= (1<<COM2B1); //COM2B0 is default 0. COM2B1 set for non-inverting operation.
        TCCR2B |= (1<<CS21); //Pre-scale = 8   16M/(8*256) = 7.8125kHz and start the clock
        pinMode(9,OUTPUT);
        //top_count_float will calculate a top count that will output a frequency specified by timer_init
        top_count_float = f_CLK/(freq_op)/(pre_scale_value*timers_0_2_reg_size);
        //bottom_count will be the value given to OCR2B for pwm generation
        bottom_count_float = top_count_float*duty_cycle_multiplier;
        //convert floating values to integer as OCRnA and OCRnB only accepts integers
        top_count_int = int(top_count_float);
        bottom_count_int = int(bottom_count_float);
        Serial.println(top_count_int);
        Serial.println(bottom_count_int);
        Serial.println("---");
        OCR2A = top_count_int;
        OCR2B = bottom_count_int;
        break;
      case 3 :
        TCCR3A |= (1<<WGM31) | (1<<WGM30); //Setting up wave generation mode for fast PWM. Still need to set WGM22
        TCCR3B |= (1<<WGM32); //Fast PWM wave generation mode 7 set. OCR2A is TOP.
        TCCR3A |= (1<<COM3B1); //COM2B0 is default 0. COM2B1 set for non-inverting operation.
        TCCR3B |= (1<<CS31); //Pre-scale = 8   16M/(8*256) = 7.8125kHz and start the clock
        pinMode(2,OUTPUT);
        //top_count_float will calculate a top count that will output a frequency specified by timer_init
        top_count_float = f_CLK/(freq_op)/(pre_scale_value*timers_1_3_4_reg_size);
        //bottom_count will be the value given to OCR2B for pwm generation
        bottom_count_float = top_count_float*duty_cycle_multiplier;
        //convert floating values to integer as OCRnA and OCRnB only accepts integers
        top_count_int = int(top_count_float);
        bottom_count_int = int(bottom_count_float);
        Serial.println(top_count_int);
        Serial.println(bottom_count_int);
        Serial.println("---");
        OCR3A = top_count_int;
        OCR3B = bottom_count_int;
        break;
      case 4 :
        TCCR4A |= (1<<WGM41) | (1<<WGM40); //Setting up wave generation mode for fast PWM. Still need to set WGM22
        TCCR4B |= (1<<WGM42); //Fast PWM wave generation mode 7 set. OCR2A is TOP.
        TCCR4A |= (1<<COM4B1); //COM2B0 is default 0. COM2B1 set for non-inverting operation.
        TCCR4B |= (1<<CS41); //Pre-scale = 8   16M/(8*256) = 7.8125kHz and start the clock
        pinMode(7,OUTPUT);
        //top_count_float will calculate a top count that will output a frequency specified by timer_init
        top_count_float = f_CLK/(freq_op)/(pre_scale_value*timers_1_3_4_reg_size);
        //bottom_count will be the value given to OCR2B for pwm generation
        bottom_count_float = top_count_float*duty_cycle_multiplier;
        //convert floating values to integer as OCRnA and OCRnB only accepts integers
        top_count_int = int(top_count_float);
        bottom_count_int = int(bottom_count_float);
        Serial.println(top_count_int);
        Serial.println(bottom_count_int);
        Serial.println("---");
        OCR4A = top_count_int;
        OCR4B = bottom_count_int;
        break;
      default :
        Serial.println("Invalid timer number");
  }  
}
