#include <stdio.h>
#include "AnalogSensor.h"
#include "Arduino.h"


AnalogSensor::AnalogSensor(int pin,int DMIN,int DMax,float VMIN,float VMAX,float Gain) {
	//pin: The Analog pin on Arduino
	//To convert digital to voltage we need two points of the curve defined by the analog to digital curve.
	//To convert to Physical, A gain is used assumed response is linear.
	pinMode(pin, INPUT);
	
	_pin = pin;
	_DMIN = DMIN;
	_DMAX = DMax;
	_VMIN = VMIN;
	_VMAX = VMAX;
	_GAIN = Gain;
	a = (_VMAX - _VMIN)/(_DMAX - _DMIN);
	b = _VMIN - a*_DMIN;
}

float AnalogSensor::ReadAnalog(void) {
	float Voltage;
	int Digital;
	float Physical;
	float a,b;
	Digital = analogRead(_pin); // read the input pin
	Voltage = a*Digital + b;
	Physical = _GAIN*Voltage;
    return Physical;
}