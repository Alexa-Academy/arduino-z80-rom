#include "Arduino.h"

#define Z80_D0  4
#define Z80_D1  5
#define Z80_D2  6
#define Z80_D3  7
#define Z80_D4  8
#define Z80_D5  9
#define Z80_D6  10
#define Z80_D7  11

#define Z80_A0  A0
#define Z80_A1  A1
#define Z80_A2  A2
#define Z80_A3  A3
#define Z80_A4  A4

#define Z80_M1  12

//#define Z80_WR  A4
#define Z80_RD  3
#define Z80_IOREQ  13
#define Z80_REFRESH  A5

#define Z80_CLOCK 2

// NOP
//byte ROM[] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};

// loop
//byte ROM[] = {0x3E, 0x00, 0x3C, 0xC3, 0x02, 0x00 };

// out_wr
//byte ROM[] = { 0x3E, 0x05, 0xD3, 0x06, 0x76 };

// loop_io
//byte ROM[] = {0x3E, 0x00, 0xD3, 0x06, 0x3C, 0xC3, 0x02, 0x00 };

// blink
//byte ROM[] = {0xAF, 0xD3, 0x06, 0x06, 0x0A, 0x10, 0xFE, 0x2F, 0xD3, 0x06, 0x06, 0x0A, 0x10, 0xFE, 0xC3, 0x00, 0x00};

// blink1
byte ROM[] = {0xAF, 0xD3, 0x06, 0x06, 0x0A, 0x10, 0xFE, 0x3E, 0x01, 0xD3, 0x06, 0x06, 0x0A, 0x10, 0xFE, 0xC3, 0x00, 0x00};


int cycle=0;

void setDatabusOut(bool isOut) {
  if (isOut) {
    pinMode(Z80_D0, OUTPUT);
    pinMode(Z80_D1, OUTPUT);
    pinMode(Z80_D2, OUTPUT);
    pinMode(Z80_D3, OUTPUT);
    pinMode(Z80_D4, OUTPUT);
    pinMode(Z80_D5, OUTPUT);
    pinMode(Z80_D6, OUTPUT);
    pinMode(Z80_D7, OUTPUT);
  } else {
    pinMode(Z80_D0, INPUT);
    pinMode(Z80_D1, INPUT);
    pinMode(Z80_D2, INPUT);
    pinMode(Z80_D3, INPUT);
    pinMode(Z80_D4, INPUT);
    pinMode(Z80_D5, INPUT);
    pinMode(Z80_D6, INPUT);
    pinMode(Z80_D7, INPUT);
  }
}

void setup() {
  Serial.begin(115200);

  cycle = 0;

  setDatabusOut(true);
  
  pinMode(Z80_A0, INPUT_PULLUP);
  pinMode(Z80_A1, INPUT_PULLUP);
  pinMode(Z80_A2, INPUT_PULLUP);
  pinMode(Z80_A3, INPUT_PULLUP);
  pinMode(Z80_A4, INPUT_PULLUP);

  pinMode(Z80_M1, INPUT_PULLUP);
  pinMode(Z80_REFRESH, INPUT_PULLUP);
  pinMode(Z80_IOREQ, INPUT_PULLUP);
  //pinMode(Z80_WR, INPUT_PULLUP);
  pinMode(Z80_RD, INPUT_PULLUP);
  pinMode(Z80_CLOCK, INPUT_PULLUP);

  attachInterrupt(digitalPinToInterrupt(Z80_CLOCK), ClockTrigger, RISING);
  attachInterrupt(digitalPinToInterrupt(Z80_RD), ReadTrigger, FALLING);
}

void ClockTrigger() {
  Serial.print("Clock #");
  Serial.print(cycle++);

  Serial.print("  Indirizzo: ");
  byte add = decodeAddress();
  Serial.print(add);
  Serial.print(" (");  

  Serial.print(digitalRead(Z80_A4));
  Serial.print(digitalRead(Z80_A3));
  Serial.print(digitalRead(Z80_A2));
  Serial.print(digitalRead(Z80_A1));
  Serial.print(digitalRead(Z80_A0));
  Serial.print(")");  

  Serial.print("  Dati: ");
  bool d7 = digitalRead(Z80_D7);
  bool d6 = digitalRead(Z80_D6);
  bool d5 = digitalRead(Z80_D5);
  bool d4 = digitalRead(Z80_D4);
  bool d3 = digitalRead(Z80_D3);
  bool d2 = digitalRead(Z80_D2);
  bool d1 = digitalRead(Z80_D1);
  bool d0 = digitalRead(Z80_D0);

  Serial.print(d7);
  Serial.print(d6);
  Serial.print(d5);
  Serial.print(d4);
  Serial.print(d3);
  Serial.print(d2);
  Serial.print(d1);
  Serial.print(d0);

  byte data_bus = d7<<7 | d6<<6 | d5<<5 | d4<<4 | d3<<3 | d2<<2 | d1<<1 | d0; 
  Serial.print(" (");
  Serial.print(data_bus, HEX);
  Serial.print(")  ");  

  byte mem_rd = digitalRead(Z80_RD);   
  Serial.print("   R:");
  Serial.print(mem_rd);

  /*
  byte mem_wr = digitalRead(Z80_WR);   
  Serial.print("   W:");
  Serial.print(mem_wr); */

  byte io_req = digitalRead(Z80_IOREQ); 
  Serial.print("   IOREQ:");
  Serial.print(io_req);

  byte refresh = digitalRead(Z80_REFRESH);   
  Serial.print("   RFSH:");
  Serial.print(refresh);

  byte m1 = digitalRead(Z80_M1);   
  Serial.print("   M1:");
  Serial.print(m1);

  Serial.println("");
}

void ReadTrigger() {
  byte add = decodeAddress();

  if (add >= 0 && add < sizeof(ROM)) {
    writeByte(ROM[add]);
  } else {
    writeByte(0);
  }
}

void writeByte(byte b) {
  digitalWrite(Z80_D0, bitRead(b, 0));
  digitalWrite(Z80_D1, bitRead(b, 1));
  digitalWrite(Z80_D2, bitRead(b, 2));
  digitalWrite(Z80_D3, bitRead(b, 3));
  digitalWrite(Z80_D4, bitRead(b, 4));
  digitalWrite(Z80_D5, bitRead(b, 5));
  digitalWrite(Z80_D6, bitRead(b, 6));
  digitalWrite(Z80_D7, bitRead(b, 7));
}

byte decodeAddress() {
  byte add = 0;

  bitWrite(add, 0, digitalRead(Z80_A0)); 
  bitWrite(add, 1, digitalRead(Z80_A1)); 
  bitWrite(add, 2, digitalRead(Z80_A2)); 
  bitWrite(add, 3, digitalRead(Z80_A3)); 
  bitWrite(add, 4, digitalRead(Z80_A4)); 

  return add;
}

void loop() {
  delay(10);
}

