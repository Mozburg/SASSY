#include <stdio.h>
#include "current.h"
#include "Arduino.h"

	float va1l = 512; // variable to store the value read
	float Vmap1 = 0; //Map to voltage
	float Amps1 = 0; //Conversion to amps
	float val2 = 512; // variable to store the value read
	float Vmap2 = 0; //Map to voltage
	float Amps2 = 0; //Conversion to amps
Current::Current() {

}

float Current::GetDrillCurrent(void) {
    val1 = analogRead(A1); // read the input pin
    Vmap1 = ((5.0 * val1 / 1023.0) - 2.5); //map analog val
    Amps1 = Vmap1 * 10.0; //map amp val
    return Amps1;
}

float Current::GetSystemCurrent(void) {
    val2 = analogRead(A2); // read the input pin
    Vmap2 = ((5.0 * val2 / 1023.0) - 2.5); //map analog val
    Amps2 = Vmap2 * 10.0; //map amp val
    return Amps2;
}
/*
inline void __attribute__ ((constructor)) initLibrary(void) {
 //
 // Function that is called when the library is loaded
 //
    printf("Library is initialized\n"); 
}
inline void __attribute__ ((destructor)) cleanUpLibrary(void) {
 //
 // Function that is called when the library is »closed«.
 //
    printf("Library is exited\n"); 
}*/