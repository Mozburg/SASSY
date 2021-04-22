#include <stdio.h>
#include "StepperDriver.h"
#include "Arduino.h"

StepperDriver::StepperDriver(int MicroSteps,int DIR,int EN,int TMR) {
	pinMode(DIR, OUTPUT);//Direction of rotation
	pinMode(EN , OUTPUT);//Enable Pin
	
	_MicroSteps = MicroSteps;
	_DIR = DIR;
	_EN  = EN ;
	_TMR = TMR;
	_MaxFreq = 6900;
	_MinFreq = 40;
	_FOSC = 16000000;
	
	PulsesPerRevolution = 200*MicroSteps;
	dir = 0;
	NumberOfSteps = 0;
	RPMSpeed = 0;
	
	//PWM Output pins taken on B
	//A is used for the top value
	//We will use Timer1 and Timer 3
	//Corresponding pins (OC1B-12- and OC3B-2-) are set to output
	if (_TMR == 1){
		pinMode(12, OUTPUT);
	}
	if (_TMR == 3){
		pinMode(2, OUTPUT);
	}
}

void StepperDriver::RotateCW(void){
	digitalWrite(_DIR,0);
	dir = 1;
	return;
}
void StepperDriver::RotateCCW(void){
	digitalWrite(_DIR,1);
	dir = -1;
	return;
}

void StepperDriver::StepperEN(void){
	digitalWrite(_EN,1);	
	return;
}

void StepperDriver::StepperDIS(void){
	digitalWrite(_EN,0);
	return;
}

void StepperDriver::ClearNumberOfSteps(void){
	NumberOfSteps = 0;	
	return;
}

void StepperDriver::UpdateNumberOfSteps(void){
	NumberOfSteps += dir;	
	return;
}

void StepperDriver::StartRotation(void){
	//Prescaler = 8 => TimerClock = 16000000/8=2000000 = 2MHz
	if (_TMR==1)
	{
		TCCR1B = _BV(CS11);
				
		TCCR1A =  _BV(WGM11) | _BV(WGM10) ;		//Timer1:Setting Mode Fast PWM TOP OCR1A 
		TCCR1B |=  (_BV(WGM13) | _BV(WGM12)) ;
		
		TCCR1A |= (_BV(COM1B1) | _BV(COM1B0)); 	// Set on compare Match (Inverting Mode)
		
		//Clear Counters
		TCNT1 = 0;
	}
	
	if (_TMR == 3)
	{
		TCCR3B = _BV(CS31);
	
		TCCR3A =  _BV(WGM31) | _BV(WGM30) ;		//Timer3:Setting Mode Fast PWM TOP OCR3A 
		TCCR3B |=  (_BV(WGM33) | _BV(WGM32)) ;
	
		TCCR3A |= (_BV(COM3B1) | _BV(COM3B0)); 	// Set on compare Match (Inverting Mode)
	
		//Clear Counters
		TCNT3 = 0;
	}
	
	return;
}

void StepperDriver::StopRotation(void){
	//Stop Timer
	if (_TMR == 1)
	{
		TCCR1B = 0;
		TCCR1A = 0;
	}
	if (_TMR == 3)
	{
		TCCR3B = 0;
		TCCR3A = 0;
	}

	return;
}

void StepperDriver::SetRPM(double rpm){
	int freq;
	double temp;
	
	//Calculate the corresponding Frequency
	temp = rpm*PulsesPerRevolution;
	temp = temp/60.0;
	freq = (int)temp;
	if (freq > _MaxFreq){
		freq = _MaxFreq;
	}
	if (freq < _MinFreq){
		freq = _MinFreq;
	}
	//Set the correponding value of OCRxA and OCRxB
	temp = _FOSC >> 3;
	temp = temp/freq;
	if (_TMR == 1)
	{
		//Clear timer to ensure it is less than OCRxA
		TCNT1 = 0;
		OCR1A = (int) temp;
		OCR1B = OCR1A >> 1;
	}
	if (_TMR == 3)
	{
		//Clear timer to ensure it is less than OCRxA
		TCNT3 = 0;
		OCR3A = (int) temp;
		OCR3B = OCR1A >> 1;
	}
	temp = 60.0*freq;
	temp = temp/PulsesPerRevolution;
	RPMSpeed = (int) temp;
	return;
}