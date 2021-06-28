#define purge 31
#define psi 28
int psiHigh = 1;
int psiLow = 0;
int highFlag = 0;
int psiRead = 0;

void purgeInit(void){
pinMode(purge, OUTPUT);
pinMode(psi, INPUT);
}

void purgeControl(void){
psiRead = digitalRead(psi);

if (psiRead == psiHigh){
digitalWrite(purge,HIGH);
highFlag = 1;
}
else if ((psiRead == psiLow) && (highFlag == 1)){
digitalWrite(purge,LOW);
highFlag = 0;
}
}
