#include <stdio.h>
#include "loadcell.h"
#include "Arduino.h"
#define gain 455    //Voltage gain of instrumentation amplifier
#define maxWeight 100
#define sensitivity 0.03



Loadcell::Loadcell() {

}

float Loadcell::GetLoad(void) {
	float voltage, weight, force, VtoLBS, Vload;
	int val;
    val = analogRead(A3);                    //Value read from loadcell
    voltage = ((val) * 5.0 / 1023.0);       //Voltage seen at output of instrumentation amplifier
    Vload = voltage / gain;                         //Voltage out of load cell
    weight = ((Vload * maxWeight)/ sensitivity)*4.45; //Convert Voltage to Newtons
    return weight;
}