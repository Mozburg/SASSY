#include <avr/io.h>
#define f_CLK 16000000


//the prescalar will be decided by the user, an INPUT. Based on INPUT we calculate top count.
//INPUTS timer number, pre-scale value, copy comments regarding pin to timer
//user can look in the README and see what pins belong to what timer
//pinmode pin and number we assign pwm mode 7
//duty cycle is an input to the function
float top_count_float  =0;
int top_count_int;
float duty_float = 0;
int duty_int= 0;
int test_val = 0;
int i = 0;
bool flag = LOW;
float dcf = 0.0;


float TempCountFloat = 0;
int TempCountInt = 0;
float bottom_float = 0;


  //timer_init(0,60,0.1,8);//Pin4    Probably good


void timer_init(int timer_number, float freq_op, float duty_cycle, int pre_scale_value){
  //if pre_scale_valeu doesnt work with define, we will use an int instead
  switch(timer_number) {
    
      case 0 : // 8-bit timer has 0,8,64,256,1024 prescale values
        TCCR0A |= (1<<WGM01) | (1<<WGM00); //Setting up wave generation moe for fast PWM. Still need to set WGM22
        TCCR0B |= (1<<WGM02); //Fast PWM wave generation mode 7 set. OCR2A is TOP.
        TCCR0A |= (1<<COM0B1); //COM2B0 is default 0. COM2B1 set for non-inverting operation.
        pinMode(4, OUTPUT);
        
        //========Top Count and Duty Cycle=============================
        top_count_float = 16000000/(pre_scale_value*256*freq_op);
        top_count_int = top_count_float;
        OCR0A = top_count_int;
        duty_float = top_count_int*duty_cycle;
        duty_int = duty_float;
        OCR0B = duty_int;
        
        //==========Timer 0 prescale====================================
        //Note: Timer zero is 8 bit and does NOT have a 32 or 128 prescale value
        if(pre_scale_value == 0)
         TCCR0B |= (1<<CS00); //Pre-scale = 0   16M/256 = 62.5kHz and start the clock ***I think this or 16MHz 
        else if(pre_scale_value ==8)
         TCCR0B |= (1<<CS01); //Pre-scale = 8   16M/(8*256) = 7.8125kHz and start the clock
        else if(pre_scale_value ==64)
          TCCR0B |= (1<<CS01)| (1<<CS00); //Pre-scale = 64   16M/(64*256) = 976.56Hz and start the clock
        else if (pre_scale_value == 256)
          TCCR0B |= (1<<CS02); //Pre-scale = 256   16M/(64*256) = 244.14Hz and start the clock
        else if (pre_scale_value == 1024)
          TCCR0B |= (1<<CS02)|(1<<CS00); //Pre-scale = 8   16M/(8*256) = 7.8125kHz and start the clock
          //==================================================================
        break;


        
      case 1 :
        TCCR1A |= (1<<WGM11) | (1<<WGM10); //Setting up wave generation mode for fast PWM. Still need to set WGM22
        TCCR1B |= (1<<WGM12) | (1<<WGM13); //Fast PWM wave generation mode 15 set. OCR1A is TOP.
        TCCR1A |= (1<<COM1B1); //COM2B0 is default 0. COM2B1 set for non-inverting operation.
        pinMode(12,OUTPUT);
        
      //========Top Count and Duty Cycle=============================*********************************************************************************
        top_count_float = (16000000/(pre_scale_value*1024))/freq_op;
         //Serial.println(top_count_float);
        top_count_int = top_count_float;
        dcf = duty_cycle;
        OCR1A = top_count_int;
        duty_float = top_count_int*duty_cycle;
        duty_int = duty_float;
        OCR1B = duty_int;
        
       //=============Timer 1 prescale================================================****************************************************************
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
      //============================================================================
        break;


        
      case 2 :
        TCCR2A |= (1<<WGM21) | (1<<WGM20); //Setting up wave generation mode for fast PWM. Still need to set WGM22
        TCCR2B |= (1<<WGM22); //Fast PWM wave generation mode 7 set. OCR2A is TOP.
        TCCR2A |= (1<<COM2B1); //COM2B0 is default 0. COM2B1 set for non-inverting operation.
        pinMode(9,OUTPUT);
           //========Top Count and Duty Cycle=============================*********************************************************************************
        top_count_float = (16000000/(pre_scale_value*256))/freq_op;
         //Serial.println(top_count_float);
        top_count_int = top_count_float;
       
        OCR2A = top_count_int;
        duty_float = top_count_int*duty_cycle;
        duty_int = duty_float;
        OCR2B = duty_int;
         //=============Timer 2 prescale=================================================  
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
        //===============================================================================
        break;


        
      case 3 : 
        TCCR3A |= (1<<WGM31) | (1<<WGM30); //Setting up wave generation mode for fast PWM. Still need to set WGM22
        TCCR3B |= (1<<WGM32) | (1<<WGM33); //Fast PWM wave generation mode 7 set. OCR2A is TOP.
        TCCR3A |= (1<<COM3B1); //COM2B0 is default 0. COM2B1 set for non-inverting operation.
        pinMode(2,OUTPUT);
           //========Top Count and Duty Cycle=============================*********************************************************************************
        top_count_float = (16000000/(pre_scale_value*1024))/freq_op;
         //Serial.println(top_count_float);
        top_count_int = top_count_float;
       
        OCR3A = top_count_int;
        duty_float = top_count_int*duty_cycle;
        duty_int = duty_float;
        OCR3B = duty_int;
        //=============Timer 3 prescale================================================
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
        //=================================================================================
        break;


        
      case 4 : 
        TCCR4A |= (1<<WGM41) | (1<<WGM40); //Setting up wave generation mode for fast PWM. Still need to set WGM22
        TCCR4B |= (1<<WGM42) | (1<<WGM43); //Fast PWM wave generation mode 7 set. OCR2A is TOP.
        TCCR4A |= (1<<COM4B1); //COM2B0 is default 0. COM2B1 set for non-inverting operation.
        pinMode(7,OUTPUT);
          //========Top Count and Duty Cycle=============================*********************************************************************************
        top_count_float = (16000000/(pre_scale_value*1024))/freq_op;
         //Serial.println(top_count_float);
        top_count_int = top_count_float;
       
        OCR4A = top_count_int;
        duty_float = top_count_int*duty_cycle;
        duty_int = duty_float;
        OCR4B = duty_int;
        //=============Timer 4 prescale================================================
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
        //=================================================================================  
        break;

        
      default :
        Serial.println("Invalid timer number");
  }
}
