#include "Arduino.h"
#include "PCF8575.h"  // https://github.com/xreef/PCF8575_library


// minute 0x27 -- sekunde 0x23

PCF8575 domaciExpander(0x20);
PCF8575 gostiExpander(0x27);
PCF8575 krugExpander(0x22);//22 na drugom

//a=5 b=4 c=7 d=6 gnd=3 LOW=ON tipkalo (prve 4 su tipke zadnja je ground)
//const int tipkalo[5]={8,6,7,5,4};
const int tipkalo[5]={10,8,9,7,6};

const int sirena = 8;

//a=11 b=9 c=12 d=10 HIGH=ON daljinski
const int kratkiDebounce=10;
const int dugiDebounce=167;

//brojevi na displayu
byte brojevib[10]={0b00111111,0b00000110,0b01011011,0b01001111,0b01100110,0b01101101,0b01111101,0b00000111,0b01111111,0b01101111};
byte brojevi[10]={0b11111100,0b01100000,0b11011010,0b11110010,0b01100110,0b10110110,0b10111110,0b11100000,0b11111110,0b11110110};

void setup()
{
  //Serial.begin(9600);


  //tipkalo pinovi (LOW)=ON
  pinMode(tipkalo[4],OUTPUT);
  digitalWrite(tipkalo[4],LOW);
  pinMode(tipkalo[0],INPUT_PULLUP);
  pinMode(tipkalo[1],INPUT_PULLUP);
  pinMode(tipkalo[2],INPUT_PULLUP);
  pinMode(tipkalo[3],INPUT_PULLUP);




  for(int i=0;i<16;i++) {
    domaciExpander.pinMode(i, OUTPUT);
  }
  domaciExpander.begin();

  for(int i=0;i<16;i++) {
    gostiExpander.pinMode(i, OUTPUT);
  }
  gostiExpander.begin();

  for(int i=0;i<16;i++) {
    krugExpander.pinMode(i, OUTPUT);
  }
  krugExpander.begin();

}

int pressTime[4];
int daljinskiPressTime[4];

int domaci=0;
int gosti=0;
int krug=0;
int last=-1;

void loop()
{


  for (int i=0; i<=3; i++){
    (digitalRead(tipkalo[i])) ? pressTime[i]=0 : pressTime[i]++;
  }


  //domaci kratko +
  if(pressTime[0] == kratkiDebounce){
    if(domaci<99){
      domaci++;
      last=1;
    }
  }

  if(pressTime[0] == dugiDebounce){
      domaci=0;
      last=-1;
  }


  //gosti kratko +
  if(pressTime[1] == kratkiDebounce){
    if(gosti<99){
      gosti++;
      last=2;
    }
  }
  if(pressTime[1] == dugiDebounce){
      gosti=0;
      last=-1;
  }

  //krug kratko plus
  if(pressTime[2] == kratkiDebounce){
    if(krug<8){
      krug++;
      last=3;
    }else{
      krug=0;
    }
  }

  if(pressTime[2] == dugiDebounce){
      krug=0;
      last=-1;
  }

  
//undo
  if(pressTime[3] == kratkiDebounce){
    switch(last){
      case 1:
        domaci--;
        last=-1;
        break;
      case 2:
        gosti--;
        last=-1;
        break;
      case 3:
        krug--;
        last=-1;
        break;
      default:
        break;
    }
  }


  drawNumbers(domaci,gosti,krug);
}


void drawNumbers(int domaci,int gosti, int krug){
  

//domaci

if(domaci<10){
  for (int i = 7; i>=0; i--){
    ((0b00000000 >> i)&1) ? domaciExpander.digitalWrite(7-i, HIGH) : domaciExpander.digitalWrite(7-i, LOW);
  }

  for (int i = 7; i>=0; i--){
    ((brojevi[domaci%10] >> i)&1) ? domaciExpander.digitalWrite(15-i, HIGH) : domaciExpander.digitalWrite(15-i, LOW);
  }
}else{
  for (int i = 7; i>=0; i--){
    ((brojevib[domaci/10] >> i)&1) ? domaciExpander.digitalWrite(7-i, HIGH) : domaciExpander.digitalWrite(7-i, LOW);
  }

  for (int i = 7; i>=0; i--){
    ((brojevi[domaci%10] >> i)&1) ? domaciExpander.digitalWrite(15-i, HIGH) : domaciExpander.digitalWrite(15-i, LOW);
  }
}

if(gosti<10){
    for (int i = 7; i>=0; i--){
    ((brojevib[gosti%10] >> i)&1) ? gostiExpander.digitalWrite(7-i, HIGH) : gostiExpander.digitalWrite(7-i, LOW);
  }

  for (int i = 7; i>=0; i--){
    ((0b00000000 >> i)&1) ? gostiExpander.digitalWrite(15-i, HIGH) : gostiExpander.digitalWrite(15-i, LOW);
  }
}else{
  for (int i = 7; i>=0; i--){
    ((brojevib[gosti/10] >> i)&1) ? gostiExpander.digitalWrite(7-i, HIGH) : gostiExpander.digitalWrite(7-i, LOW);
  }

  for (int i = 7; i>=0; i--){
    ((brojevi[gosti%10] >> i)&1) ? gostiExpander.digitalWrite(15-i, HIGH) : gostiExpander.digitalWrite(15-i, LOW);
  }
}

//krug
if(krug==0){
  for (int i = 7; i>=0; i--){
    ((0b00000000 >> i)&1) ? krugExpander.digitalWrite(7-i, HIGH) : krugExpander.digitalWrite(7-i, LOW);
  }

  for (int i = 7; i>=0; i--){
    ((0b00000000 >> i)&1) ? krugExpander.digitalWrite(15-i, HIGH) : krugExpander.digitalWrite(15-i, LOW);
  }
}else{
  for (int i = 7; i>=0; i--){
    ((brojevib[krug] >> i)&1) ? krugExpander.digitalWrite(7-i, HIGH) : krugExpander.digitalWrite(7-i, LOW);
  }

  for (int i = 7; i>=0; i--){
    ((brojevi[krug] >> i)&1) ? krugExpander.digitalWrite(15-i, HIGH) : krugExpander.digitalWrite(15-i, LOW);
  }
}


}
