#include <stdio.h>
#include "current.h"
#include "Arduino.h"

	float val = 512; // variable to store the value read
	float Vmap = 0; //Map to voltage
	float Amps = 0; //Conversion to amps
Current::Current() {

}

float Current::GetCurrent(void) {
    val = analogRead(A1); // read the input pin
    Vmap = ((5.0 * val / 1023.0) - 2.5); //map analog val
    Amps = Vmap * 10.0; //map amp val
    return Amps;
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