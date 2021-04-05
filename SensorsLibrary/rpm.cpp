#include <stdio.h>
#include "rpm.h"
#include "Arduino.h"

RPM::RPM() {

}

int RPM::GetRPM(void) {
	float val = analogRead(A5);  //Value is high if reflective tape is infront of sensor
//----------------------------------------------------------------------------------mozburg
	//buffer vars
	float max = 1;
	float min = 0;
	float mid = .5;
	
	//max set
	max = val*(val >= max) + max*(val <= max);
	
	//min set
	min = val*(val <= min) + min*(val >= min);
	
	//mid set
        mid = (max + min) / 2;

	//1-0 mid check
	return (val > mid); 
//-------------------------------------------------------------------------------------
}
	
	
/*    if(val > mid) {
		return 1; //Increases rotation each pulse
    }

	else {
		return 0;
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
