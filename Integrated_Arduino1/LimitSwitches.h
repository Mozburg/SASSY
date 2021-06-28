
#define TopLimit 18
#define BottomLimit 19
#define FrontLimit 16 //used to be20
#define BackLimit 17 //used to be 21

void LimSwInit(){
        pinMode(TopLimit, INPUT);
        pinMode(BottomLimit, INPUT);
        pinMode(FrontLimit, INPUT);
        pinMode(BackLimit, INPUT);

}
//======Limit Switches=================================
  void LimSwCntrl(){
  if (digitalRead(TopLimit)==HIGH){
    vertDOWN();
  }
  else if (digitalRead(BottomLimit)==HIGH){
    vertUP();
  }
  if (digitalRead(FrontLimit)==HIGH){
   horzREV();
  }
  else if (digitalRead(BackLimit)==HIGH){
   horzFWD();
  }
  //======================================================
}
