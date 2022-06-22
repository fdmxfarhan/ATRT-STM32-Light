#include <Adafruit_SH1106_STM32.h>
Adafruit_SH1106 display(-1);

void att_init(){
  pinMode(PA11,OUTPUT);
  pinMode(PA12,OUTPUT);  
  pinMode(PC13,OUTPUT);  
  pinMode(PC15,OUTPUT);  
  pinMode(PC14,OUTPUT);  
  pinMode(PB0,OUTPUT);  
  pinMode(PB1,OUTPUT);  

  
  pinMode(PB12,OUTPUT);
  pinMode(PB13,OUTPUT);
  pinMode(PB14,OUTPUT);
  pinMode(PB15,OUTPUT);

  pinMode(PA8,PWM);
  pinMode(PB8,PWM);
  pinMode(PB7,PWM);
  pinMode(PB6,PWM);
  
  digitalWrite(PC13,1);
  pinMode(PB5,INPUT);  
  pinMode(PB4,INPUT);  
  pinMode(PA15,INPUT);  
  
  motor(0,0,0,0);
  display.begin(0x2, 0x3C);
  display.clearDisplay();
  display.setTextColor(WHITE);
  display.setTextSize(4);
  display.setCursor(15,10);
  display.print("FDMX");
  display.display();
  for(int i=0; i<4; i++){
    digitalWrite(PA11, 0);
    digitalWrite(PA12, 1);
    delay(100);
    digitalWrite(PA11, 1);
    digitalWrite(PA12, 0);
    delay(100);
  }
  digitalWrite(PA11, 0);
  digitalWrite(PA12, 0);
  display.setTextSize(1);
  display.clearDisplay();
  display.display();
  delay(500); 
  Serial.begin(115200);
  Serial.write(0XA5);
  Serial.write(0X54);
  delay(500);
  Serial.write(0XA5);
  Serial.write(0X51);
}

void read_all(){
  ///////////////////-----Keys
  if(digitalRead(PB4) || digitalRead(PB5) || digitalRead(PA15))  {
    digitalWrite(PC13,0);
    if(digitalRead(PB5)){
      Serial.write(0XA5);
      Serial.write(0X55);
      while(digitalRead(PB5));
    }
  }
  else digitalWrite(PC13,1);
  ///////////////////-----Sensors
  sensor[0] = analogRead(PA1);
  sensor[1] = analogRead(PA2);
  sensor[2] = analogRead(PA3);
  sensor[3] = analogRead(PA4);
  sensor[4] = analogRead(PA5);
  sensor[5] = analogRead(PA6);
  sensor[6] = analogRead(PA7);
  for(int i = 0; i<7; i++) sensor[i] /= 10;
  ///////////////////-----GY-25
  Serial.write(0XA5);
  Serial.write(0X51);
  while (true) {   
    buff[counter] = Serial.read();
    if(counter == 0 && buff[0] != 0xAA) break;
    counter++;       
    if(counter==8)
    {   
    counter=0;                 
    if(buff[0]==0xAA && buff[7]==0x55)
      {         
       GY=(int16_t)(buff[1]<<8|buff[2])/100.00; 
      } 
    }    
  }
  ///////////////////-----TSOP sensors
  tsop_min = 4095;
  for(int i = 0; i<16; i++){
    digitalWrite(PB1, (i / 1) % 2);
    digitalWrite(PB0, (i / 2) % 2);
    digitalWrite(PC14,(i / 4) % 2);
    digitalWrite(PC15,(i / 8) % 2);
    if(analogRead(PA0) < tsop_min){
      tsop_min = analogRead(PA0);
      tsop_num = i;
    }
  }
  if(tsop_min > 4090){
    tsop_num = 16;
    digitalWrite(PA11,0);
    digitalWrite(PA12,1);
  }
  else {
    digitalWrite(PA11,1);
    digitalWrite(PA12,0);
  }
}

void print_all(){
  display.clearDisplay();
  ///////////////////-----Sensors
  for(int i = 0; i<7 ; i++){
    display.setCursor(0,i*9);
    display.print(i);
    display.print(":");
    display.print(sensor[i]);
  }
  ///////////////////-----GY-25
  display.setCursor(40,0);
  display.print("GY:");
  display.print(GY);
  ///////////////////-----GY-25 Compass
  display.drawCircle(105,40,13,WHITE);
  cx1 = 105 + sin((-GY*180/179)*PI/180)*12;
  cx2 = 105 - sin((-GY*180/179)*PI/180)*12;
  cy1 = 40  + cos((-GY*180/179)*PI/180)*12;
  cy2 = 40  - cos((-GY*180/179)*PI/180)*12;
  display.drawLine(cx1,cy1,cx2,cy2,WHITE);
  display.fillCircle(cx1,cy1,2,WHITE);
  ///////////////////-----TSOP Sensors
  display.setCursor(40,10);
  display.print(tsop_num);
  display.print(":");
  display.print(tsop_min);
  ///////////////////-----Ball
  bx = 105 + sin(tsop_num*22.5*PI/180)*19;
  by = 40 - cos(tsop_num*22.5*PI/180)*19;
  if(tsop_min<4000) display.fillCircle(bx,by,4,WHITE);
  display.display();
}

void motor(int L1, int L2, int R2, int R1){
//  if(GY>20 && GY<-20) GY*=2;
//  else                GY*=1;
  if(GY>6 && GY<=30)          GY = 40;
  else if(GY>30 && GY<=80)    GY = 80;
  else if(GY>80)              GY = 150;
  else if(GY<-6 && GY>=-30)   GY = -40;
  else if(GY<-30 && GY>=-80)  GY = -80;
  else if(GY<-80)             GY = -150;

  L1 += GY;
  L2 += GY;
  R1 += GY;
  R2 += GY;

  L1 = L1*65535/255;
  L2 = L2*65535/255;
  R1 = R1*65535/255;
  R2 = R2*65535/255;
  if(L1 > 65535) L1 = 65535;
  if(L2 > 65535) L2 = 65535;
  if(R1 > 65535) R1 = 65535;
  if(R2 > 65535) R2 = 65535;
  if(L1 < -65535) L1 = -65535;
  if(L2 < -65535) L2 = -65535;
  if(R1 < -65535) R1 = -65535;
  if(R2 < -65535) R2 = -65535;
  
  if(L1 >= 0){
    digitalWrite(PB15,0);
    pwmWrite(PA8,L1);
  }
  else{
    digitalWrite(PB15,1);
    pwmWrite(PA8,L1+65535);
  }
  if(L2 >= 0){
    digitalWrite(PB14,0);
    pwmWrite(PB8,L2);
  }
  else{
    digitalWrite(PB14,1);
    pwmWrite(PB8,L2+65535);
  }
  if(R2 >= 0){
    digitalWrite(PB13,0);
    pwmWrite(PB7,R2);
  }
  else{
    digitalWrite(PB13,1);
    pwmWrite(PB7,R2+65535);
  }
  if(R1 >= 0){
    digitalWrite(PB12,0);
    pwmWrite(PB6,R1);
  }
  else{
    digitalWrite(PB12,1);
    pwmWrite(PB6,R1+65535);
  }
}
