#include "Arduino.h"
#include "PCF8575.h"  // https://github.com/xreef/PCF8575_library
#include "RTClib.h"
RTC_DS1307 RTC;

// domaci 0x20 -- gosti 0x24 -- minute 0x27 -- sekunde 0x23
PCF8575 domaciExpander(0x24);
PCF8575 gostiExpander(0x20);

PCF8575 minuteExpander(0x27);
PCF8575 sekundeExpander(0x23);

//a=5 b=4 c=7 d=6 gnd=3 LOW=ON tipkalo (prve 3 su tipke zadnja je ground)
const int tipkalo[5]={5,4,7,6,3};

const int sirena = 8;

//a=11 b=9 c=12 d=10 HIGH=ON daljinski
const int daljinski[4]={11,9,12,10};
const int daljinskiDebounce=1;
const int kratkiDebounce=10;
const int dugiDebounce=500;

//brojevi na displayu
byte brojevi[10]={0b00111111,0b00000110,0b01011011,0b01001111,0b01100110,0b01101101,0b01111101,0b00000111,0b01111111,0b01101111};
byte brojevib[10]={0b11111100,0b01100000,0b11011010,0b11110010,0b01100110,0b10110110,0b10111110,0b11100000,0b11111110,0b11110110};

void setup()
{
  Serial.begin(9600);

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

  pinMode(sirena,OUTPUT);
  digitalWrite(sirena,LOW);  

  // Set pinMode to OUTPUT
  for(int i=0;i<16;i++) {
    domaciExpander.pinMode(i, OUTPUT);
  }
  domaciExpander.begin();

  for(int i=0;i<16;i++) {
    gostiExpander.pinMode(i, OUTPUT);
  }
  gostiExpander.begin();

  for(int i=0;i<16;i++) {
    minuteExpander.pinMode(i, OUTPUT);
  }
  minuteExpander.begin();

  for(int i=0;i<16;i++) {
    sekundeExpander.pinMode(i, OUTPUT);
  }
  sekundeExpander.begin();

  RTC.begin();
  if (! RTC.isrunning()) {
    Serial.println("RTC is NOT running!");
  } 
  RTC.adjust(DateTime(0,0,0,0,0,0));

}

int pressTime[4];
int daljinskiPressTime[4];

DateTime zadano=DateTime(0,0,0,1,15,0);
DateTime spremljeno=DateTime(0,0,0,0,0,0);
TimeSpan konacno;

bool pause=true;
int mode=0;
int domaci=0;
int gosti=0;

void loop()
{

    DateTime now = RTC.now();

  if(pause!=1){
    konacno=zadano-now;
  }
  else{
    konacno=zadano-spremljeno;
  }

  for (int i=0; i<=3; i++){
    (digitalRead(tipkalo[i])) ? pressTime[i]=0 : pressTime[i]++;
  }

  for (int i=0; i<=3; i++){
    (digitalRead(daljinski[i])) ? daljinskiPressTime[i]++ : daljinskiPressTime[i]=0;
  }


  //domaci kratko +
  if(pressTime[0] == kratkiDebounce || daljinskiPressTime[0] == daljinskiDebounce){
    domaci++;
    domaci = domaci % 100;
  }
  //domaci dugo reset
  if(pressTime[0] == dugiDebounce){
    domaci =0;
  }


  //gosti kratko +
  if(pressTime[1] == kratkiDebounce || daljinskiPressTime[1] == daljinskiDebounce){
    gosti ++;
    gosti = gosti % 100;    
  }
  //gosti dugo reset
  if(pressTime[1] == dugiDebounce){
    gosti =0;
  }

  //tipkalo start stop
  if(pressTime[2] == kratkiDebounce){

    if(pause==true){
      pause=false;
      RTC.adjust(spremljeno);
    }
    else{
      spremljeno=RTC.now();
      pause=true;
    } 

  }
  //daljinski samo start
  if(daljinskiPressTime[2] == daljinskiDebounce){
      if(pause==true){
        pause=false;
        RTC.adjust(spremljeno);
      }
  }

  if(pressTime[3] == kratkiDebounce || daljinskiPressTime[3] == daljinskiDebounce){
    if (pause==true){
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
  }

//1 sekunda timer za testiranje
    if(daljinskiPressTime[1] == dugiDebounce){
      if(pause==true){
       zadano=DateTime(0,0,0,0,0,1);
      }
  }

  

  //KRAJ
  drawScore(domaci,gosti);
  drawTime(konacno);

  if(konacno.totalseconds()==0 && pause==false){
    pause=true;
    zadano=DateTime(0,0,0,0,0,0);
    spremljeno=DateTime(0,0,0,0,0,0);
    //konacno=zadano-spremljeno;
    minuteExpander.digitalWrite(0, HIGH);
    minuteExpander.digitalWrite(15, HIGH);
    //sekundeExpander.digitalWrite(0, HIGH);
    //sekundeExpander.digitalWrite(15, HIGH);

    digitalWrite(sirena,HIGH);  
    delay(1000);
    minuteExpander.digitalWrite(0, LOW);
    minuteExpander.digitalWrite(15, LOW);
    //sekundeExpander.digitalWrite(0, LOW);
    // sekundeExpander.digitalWrite(15, LOW);
    
    digitalWrite(sirena,LOW);
  }
}

void drawScore(int a , int b){

  if (a<=9){

    for (int i = 7; i>=0; i--){
      ((brojevi[a] >> i)&1) ? domaciExpander.digitalWrite(7-i, HIGH) : domaciExpander.digitalWrite(7-i, LOW);
    }

    for (int i = 7; i>=0; i--){
      domaciExpander.digitalWrite(15-i, LOW);
    }

  }
  else{

    for (int i = 7; i>=0; i--){
      ((brojevi[a/10] >> i)&1) ? domaciExpander.digitalWrite(7-i, HIGH) : domaciExpander.digitalWrite(7-i, LOW);
    }

    for (int i = 7; i>=0; i--){
      ((brojevib[a%10] >> i)&1) ? domaciExpander.digitalWrite(15-i, HIGH) : domaciExpander.digitalWrite(15-i, LOW);
    }
    
  }

  if (b<=9){

    for (int i = 7; i>=0; i--){
      ((brojevib[b] >> i)&1) ? gostiExpander.digitalWrite(15-i, HIGH) : gostiExpander.digitalWrite(15-i, LOW);
    }

    for (int i = 7; i>=0; i--){
      gostiExpander.digitalWrite(7-i, LOW);
    }

  }else{

    for (int i = 7; i>=0; i--){
      ((brojevi[b/10] >> i)&1) ? gostiExpander.digitalWrite(7-i, HIGH) : gostiExpander.digitalWrite(7-i, LOW);
    }

    for (int i = 7; i>=0; i--){
      ((brojevib[b%10] >> i)&1) ? gostiExpander.digitalWrite(15-i, HIGH) : gostiExpander.digitalWrite(15-i, LOW);
    }

  }


}


void drawTime(TimeSpan time){
  
  int minute = time.minutes() + time.hours()*60;


  for (int i = 7; i>=0; i--){
    ((brojevi[time.seconds()/10] >> i)&1) ? sekundeExpander.digitalWrite(7-i, HIGH) : sekundeExpander.digitalWrite(7-i, LOW);
  }

  for (int i = 7; i>=0; i--){
    ((brojevib[time.seconds()%10] >> i)&1) ? sekundeExpander.digitalWrite(15-i, HIGH) : sekundeExpander.digitalWrite(15-i, LOW);
  }

  if (minute <=9){

    for (int i = 7; i>=0; i--){
      ((brojevib[minute%10] >> i)&1) ? minuteExpander.digitalWrite(15-i, HIGH) : minuteExpander.digitalWrite(15-i, LOW);
    }

    for (int i = 7; i>=0; i--){
      minuteExpander.digitalWrite(7-i, LOW);
    }      
  }else{

    for (int i = 7; i>=0; i--){
      ((brojevi[minute/10] >> i)&1) ? minuteExpander.digitalWrite(7-i, HIGH) : minuteExpander.digitalWrite(7-i, LOW);
    }

    for (int i = 7; i>=0; i--){
      ((brojevib[minute%10] >> i)&1) ? minuteExpander.digitalWrite(15-i, HIGH) : minuteExpander.digitalWrite(15-i, LOW);
    }

  }


}
