/* 
 * File:   current.h
 * Author: rtmar
 *
 * Created on February 19, 2021, 5:22 PM
 */

#ifndef STEPPERDRIVER_H
#define	STEPPERDRIVER_H

class StepperDriver 
{
	public:
		StepperDriver(int MicroSteps ,int DIR,int EN,int TMR);
		void RotateCW(void);		//Direction of the rotation CW
		void RotateCCW(void);		//Direction of the rotation CCW
		void StepperEN(void);		//Enable Driver
		void StepperDIS(void);		//Disable Driver
		void StartRotation(void);
		void StopRotation(void);
		void SetRPM(double);
		void UpdateNumberOfSteps(void);
		void ClearNumberOfSteps(void);
		
		int PulsesPerRevolution;
		int dir;
		long NumberOfSteps;
		float RPMSpeed;
	private:
		int _MicroSteps;
		int _TopSwitch;
		int _BotSwitch;
		int _DIR;
		int _EN;
		int _MaxFreq;
		int _MinFreq;
		long _FOSC;
		int _TMR;
		
};
#endif

