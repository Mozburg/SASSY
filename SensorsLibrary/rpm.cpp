#include <stdio.h>
#include "rpm.h"
#include "Arduino.h"

RPM::RPM() {

}

int RPM::GetRPM(void) {
	float val = analogRead(A5);  //Value is high if reflective tape is infront of sensor

    if(val > 225.0) {
		return 1; //Increases rotation each pulse
    }

	else {
		return 0;
	}
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