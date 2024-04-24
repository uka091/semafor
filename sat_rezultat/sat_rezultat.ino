
#include <Wire.h>
#include "RTClib.h"
RTC_DS1307 RTC;

//a=5 b=4 c=7 d=6 gorund=3 LOW=ON tipkalo
const int tipkalo[5]={5,4,7,6,3};
//a=11 b=9 c=12 d=10 HIGH=ON daljinski
const int daljinski[4]={11,9,12,10};

//duljina drzanja tipke na tipkalu
const int duljinaKratki=300;
const int duljinaDugi=1000;

//brojevi na displayu
byte brojevi[10]={0b00111111,0b00000110,0b01011011,0b01001111,0b01100110,0b01101101,0b01111101,0b00000111,0b01111111,0b01101111};
byte brojevib[10]={0b11111100,0b01100000,0b11011010,0b11110010,0b01100110,0b10110110,0b10111110,0b11100000,0b11111110,0b11110110};

//adrese gpio expandera
byte expander[4]={0x20,0x21,0x22,0x23};//0,1-sekunde 2,3-minute

bool kraj=false;

void setup () {

  RTC.begin();

  //daljinski pinovi (HIGH)=ON
  pinMode(daljinski[0],INPUT);
  pinMode(daljinski[1],INPUT);
  pinMode(daljinski[2],INPUT);
  pinMode(daljinski[3],INPUT);

  //tipkalo pinovi (LOW)=ON
  pinMode(tipkalo[4],OUTPUT);
  digitalWrite(tipkalo[4],LOW);
  pinMode(tipkalo[0],INPUT_PULLUP);
  pinMode(tipkalo[1],INPUT_PULLUP);
  pinMode(tipkalo[2],INPUT_PULLUP);
  pinMode(tipkalo[3],INPUT_PULLUP);
    
  if (! RTC.isrunning()) {
    Serial.println("RTC is NOT running!");
  }
   
  RTC.adjust(DateTime(0,0,0,0,0,0));
}


bool pressed=false;
DateTime zadano=DateTime(0,0,0,1,15,0);
DateTime spremljeno=DateTime(0,0,0,0,0,0);
TimeSpan konacno;

bool pause=true;
int mode=0;

int tipka1=0;
int tipka2=0;
int tipka3=0;
int tipka4=0;

void loop () {


  
  DateTime now = RTC.now();

  if(pause!=1){
    konacno=zadano-now;
  }
  else{
    konacno=zadano-spremljeno;
  }

  if (digitalRead(tipkalo[0])== LOW){tipka1++;}
  if (digitalRead(tipkalo[0])== LOW){tipka2++;}
  if (digitalRead(tipkalo[0])== LOW){tipka3++;}
  if (digitalRead(tipkalo[0])== LOW){tipka4++;}


  //tipka 1 minute +
  if (tipka1 == duljinaKratki || digitalRead(daljinski[0])== HIGH ){

    if(pause==true){
      
      int h =int(zadano.hour());
      int m =int(zadano.minute());
      int s =int(zadano.second());
      
      if((h*60)+m >= 99){
        zadano=DateTime(0,0,0,0,0,s);
      }
      else{
      zadano=DateTime(0,0,0,h,m+1,s);
      }
      
      }
    
  }

  if(tipka2 == duljinaKratki || digitalRead(daljinski[1])== HIGH){
    if(pause==true){
      
        int h =int(zadano.hour());
        int m =int(zadano.minute());
        int s =int(zadano.second());
        zadano=DateTime(0,0,0,h,m,s+1);
      }
  }  

  


if(digitalRead(daljinski[0])== HIGH || digitalRead(tipkalo[0])== LOW){
  //tipka a  dodavanje minuta
  kraj=false;
  if (pressed==false){
    if(pause==true){
      
      int h =int(zadano.hour());
      int m =int(zadano.minute());
      int s =int(zadano.second());
      
      if((h*60)+m >= 99){
        zadano=DateTime(0,0,0,0,0,s);
      }
      else{
      zadano=DateTime(0,0,0,h,m+1,s);
      }
      
      }
  pressed=true;}
  }
else{
if(digitalRead(daljinski[1])== HIGH || digitalRead(tipkalo[1])== LOW){
  if (pressed==false){
    //RTC.adjust(DateTime(0,0,0,0,0,0));
    //tipka b dodavanje sekundi
    kraj=false;
    if(pause==true){
      int h =int(zadano.hour());
      int m =int(zadano.minute());
      int s =int(zadano.second());
      zadano=DateTime(0,0,0,h,m,s+1);
      }
  pressed=true;}
    }
else{
if(digitalRead(tipkalo[2])== LOW){
  if(pressed==false){
    //tipka c pauza play
    kraj=false;
    if(pause==true){
      pause=false;
      RTC.adjust(spremljeno);
      }else{
        spremljeno=RTC.now();
        pause=true;
        }
  pressed=true;}
  }
else{
if(digitalRead(daljinski[3])== HIGH || digitalRead(tipkalo[3])== LOW){
  if(pressed==false){
    //tipka d mod(1,3,5,10,15,30,60,90 min)
    if(pause==true){
      kraj=false;
      spremljeno=DateTime(0,0,0,0,0,0);
    switch(mode){
      case 0:
      zadano=DateTime(0,0,0,0,90,0);
      mode++;
      break;
      case 1:
      zadano=DateTime(0,0,0,0,3,0);
      mode++;
      break;
      case 2:
      zadano=DateTime(0,0,0,0,5,0);
      mode++;
      break;
      case 3:
      zadano=DateTime(0,0,0,0,10,0);
      mode++;
      break;
      case 4:
      zadano=DateTime(0,0,0,0,20,0);
      mode++;
      break;
      case 5:
      zadano=DateTime(0,0,0,0,60,0);
      mode++;
      break;
      case 6:
      zadano=DateTime(0,0,0,1,15,0);
      mode=0;
      break;
      }
    }
  pressed=true;}
  }
 else{
  if(digitalRead(daljinski[2])== HIGH ){
  if(pressed==false){
    //tipka c pauza play
    kraj=false;
    if(pause==true){
      pause=false;
      RTC.adjust(spremljeno);
      }
  pressed=true;}
  }
 else{
pressed=false;}
}}}}

  if (digitalRead(tipkalo[0])== HIGH){tipka1=0;}
  if (digitalRead(tipkalo[1])== HIGH){tipka2=0;}
  if (digitalRead(tipkalo[2])== HIGH){tipka3=0;}
  if (digitalRead(tipkalo[3])== HIGH){tipka4=0;}

    //totalseconds
  Serial.println(konacno.totalseconds());
  
  Wire.beginTransmission((byte)expander[0]);
  Wire.write((byte)bank[0]);      // address bank 
  Wire.write((byte)brojevi[konacno.seconds()%10]);  // value to send
  Wire.endTransmission();
  
  Wire.beginTransmission((byte)expander[0]);
  Wire.write((byte)bank[1]);      // address bank 
  Wire.write((byte)brojevib[konacno.seconds()/10]);  // value to send
  Wire.endTransmission();

  Wire.beginTransmission((byte)expander[2]);
  Wire.write((byte)bank[0]);      // address bank 
  Wire.write((byte)brojevi[konacno.minutes()%10]);  // value to send
  Wire.endTransmission();

  Wire.beginTransmission((byte)expander[2]);
  Wire.write((byte)bank[1]);      // address bank 
  if((konacno.minutes()/10)+(konacno.hours()*6)==0){
  Wire.write((byte)0b00000000);
    }else{
  Wire.write((byte)brojevib[(konacno.minutes()/10)+(konacno.hours()*6)]);}  // value to send
  Wire.endTransmission();

if(konacno.totalseconds()==0 && pause==false){
  pause=true;

  zadano=DateTime(0,0,0,0,0,0);
  spremljeno=DateTime(0,0,0,0,0,0);
  konacno=zadano-spremljeno;
  if(kraj==false){
  Wire.beginTransmission((byte)expander[0]);
  Wire.write((byte)bank[1]);      // address bank 
  Wire.write((byte)0b11111101);  // value to send
  Wire.endTransmission();

  Wire.beginTransmission((byte)expander[0]);
  Wire.write((byte)bank[0]);      // address bank 
  Wire.write((byte)0b10111111);  // value to send
  Wire.endTransmission();

  Wire.beginTransmission((byte)expander[2]);
  Wire.write((byte)bank[0]);      // address bank 
  Wire.write((byte)0b10111111);  // value to send
  Wire.endTransmission();

  Wire.beginTransmission((byte)expander[2]);
  Wire.write((byte)bank[1]);      // address bank 
  Wire.write((byte)0b00000001);  // value to send
  Wire.endTransmission();

  
  digitalWrite(8,HIGH);
  delay(1000);
  digitalWrite(8,LOW);
  kraj=true;
  }
  }
}
