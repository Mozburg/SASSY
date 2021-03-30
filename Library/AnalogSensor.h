/* 
 * File:   current.h
 * Author: rtmar
 *
 * Created on February 19, 2021, 5:22 PM
 */

#ifndef ANALOGSENSOR_H
#define	ANALOGSENSOR_H

class AnalogSensor 
{

	public:
		AnalogSensor(int ,int ,int ,float ,float ,float);
		float ReadAnalog(void);
	private:
		int _pin;
		int _DMIN,_DMAX;
		float _VMIN,_VMAX,a,b,_GAIN;
		
};
#endif	/* ANALOGSENSOR_H */

