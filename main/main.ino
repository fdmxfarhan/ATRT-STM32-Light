int sensor[7];
int buff[8];
int counter = 0;
int GY;
int cx1,cy1,cx2,cy2;
int bx,by;
int tsop_min, tsop_num;

int v = 255;
void setup() {
  att_init();
}

void loop() {
  read_all();
  print_all();
  if(tsop_num==0)       motor(v,v,-v,-v);     //0
  else if(tsop_num==1)  motor(v,v/2,-v,-v/2); //1
  else if(tsop_num==2)  motor(v,-v/2,-v,v/2); //3
  else if(tsop_num==3)  motor(v/2,-v,-v/2,v); //5
  else if(tsop_num==4)  motor(0,-v,0,v);      //6
  else if(tsop_num==5)  motor(-v/2,-v,v/2,v); //7
  else if(tsop_num==6)  motor(-v,-v,v,v);     //8
  else if(tsop_num==7)  motor(-v,-v/2,v,v/2); //9
  else if(tsop_num==8)  motor(-v,0,v,0);      //10
  else if(tsop_num==9)  motor(-v/2,-v,v/2,v); //7
  else if(tsop_num==10) motor(-v,-v,v,v);     //8
  else if(tsop_num==11) motor(-v,-v/2,v,v/2); //9
  else if(tsop_num==12) motor(-v,0,v,0);      //10
  else if(tsop_num==13) motor(-v,v/2,v,-v/2); //11
  else if(tsop_num==14) motor(-v/2,v,v/2,-v); //13
  else if(tsop_num==15) motor(v/2,v,-v/2,-v); //15
  
  else                  motor(0,0,0,0);       //STOP

}
