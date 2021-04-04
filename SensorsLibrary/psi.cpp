#include <stdio.h>
#include "psi.h"
#include "Arduino.h"


PSI::PSI() {

}

float PSI::GetPSI(void) {
	int val;
    val = analogRead(A4);                    //Value read from PSI
    float voltage = ((val) * 5.0 / 1023.0);       //Voltage seen at output of the PSI switch
    return voltage;
}