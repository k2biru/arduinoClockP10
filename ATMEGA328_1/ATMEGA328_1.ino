#include <Wire.h>
#include "Font3x5.h"
#include "minimalis.h"
#include "Comic_Sans_MS_Custom_13.h"       // Font sedang (Costom ASCII untuk menambahkan simbol derajat (" ` "= derajat)
#include <DMD.h>                    // Custom DMD library (ditambahkan setingan kecerahan)


#include "fonts/SystemFont5x7.h"
#include "fonts/angka6x13.h"
#include "fonts/angka_2.h"



#define DEBUG false                    // debug untuk menampilkan serial (true untuk debug)
#define BIT_PANJANG true                  // panjang artinya 16 bit
#define BIT_PENDEK false                  // pendek artinya 8 bit
#define MAX_SERIAL 140                 // maksimal daya tampung serial
#define MAX_ST_TXT 10
#define DISPLAYS_ACROSS 2             // panjang  DMD
#define DISPLAYS_DOWN 1               // lebar DMD


//Global var
char status, MSG[MAX_SERIAL];
float suhu;
uint8_t DHTLembab, DHTSuhu;
byte second, minute, hour, dayOfWeek, dayOfMonth, month, year;
byte disp, menitAkhirAcak = rand() % 9, detikAcak = rand() % 59;
byte menitAkhirAcak2 = rand() % 9;                             

unsigned long last  ;
DMD dmd(DISPLAYS_ACROSS, DISPLAYS_DOWN, 1);

//####################################################################


void setTanggal();
void cariHari();
void setjam();
unsigned int fscale( unsigned int, unsigned int , unsigned int , unsigned int , unsigned int , float );
void updateEEPROM (unsigned int , unsigned int, boolean );
unsigned int readEEPROM (unsigned int, boolean );
void dmdInit();
void ScanDMD();
void setKecerahan(unsigned int);
void runClockBox(byte, byte );
void jamAngka(byte , byte );
void tampilSuhu(byte , byte );
void tampilMarque(unsigned int);
void tampilkanHariTanggal();
byte decToBcd(byte );
byte bcdToDec(byte );
void setDateDS3231(byte, byte, byte, byte, byte, byte, byte);
void getDateDS3231(byte *second, byte *minute, byte *hour, byte *dayOfWeek, byte *dayOfMonth, byte *month, byte *year);
float getTemp3231Celcius();
char serialRead();
void setClock();
void setDate();
void DHTSampling();
void tampilKelembaban(byte x, byte y);
void setBrighness();
void setMarq();
void correctionTemp();
void staticTextConf();
void marqueText();
void weatherRead();
void staticText(byte x, byte y, byte sec, char text[MAX_ST_TXT]);





void setup() {
  randomSeed(analogRead(0));
  Serial.begin(2400);                  // serial ESP
  dmdInit();
  DHTSampling();
  setKecerahan(2000);                         //Set kecerahan
  //Timer1.pwm(PIN_DMD_nOE, 1); //0~1024 [10 16 25 40 65 100 160 250 400 640 1024]
  Wire.begin();
  //setDateDS3231(0, 44, 19, 2, 2, 1, 17);
  dmd.clearScreen(0);
  getDateDS3231(&second, &minute, &hour, &dayOfWeek, &dayOfMonth, &month, &year);
  digitalWrite(3, HIGH);
  staticText(4, 1, 99, "Yozora v1.1");
  staticText(15, 7, 92, "Langit Malam");
  digitalWrite(3, LOW);
  tampilkanHariTanggal();
  dmd.clearScreen(0 ); // 0 = Black
  //setDateDS3231(second, minute, hour, 6, dayOfMonth, month, year);
  disp= rand() % 10;

}

void loop() {

  while (Serial.available())
  {

    status = serialRead();
    if (status == 'C') setClock();
    else if (status == 'D') setDate();
    else if (status == 'B') setBrighness();
    else if (status == 'S') setMarq();
    else if (status == 'T') correctionTemp();
    else if (status == 'm') staticTextConf();
    else if (status == 'M') marqueText();
    else if (status == 'W') weatherRead();
    if (status == 'C' || status == 'D' || status == 'M') dmd.clearScreen(0); // 0= black
    if (status != 'm')status = 0;
  }

  if (minute == 0 && second == 0)digitalWrite(3, HIGH); else digitalWrite(3, LOW); // Buzzer

  unsigned long now = millis();
  if ((now - last) >= 999 && status == 0) {                 //timmer setiap mendekati 1000
    last = now;
    getDateDS3231(&second, &minute, &hour, &dayOfWeek, &dayOfMonth, &month, &year);

    if (hour >= 21 || hour<4)
    {
    displayClockMini();
    } else {

      if (menitAkhirAcak2 == (minute % 10) && second == detikAcak)
      {
       disp= rand() % 7;
       menitAkhirAcak2 = rand() % 9;
      }
 
//displayClockMini();
//displayClockSqrAndMini();

      if (disp=0){ displayClockSqrTemp();
      } else if (disp=1){ displayClockMini();
      } else if (disp=2){ displayClockSqrHum();
      } else if (disp=3){ displayClockBig();
      } else if (disp=4){ displayClockMid();
      } else if (disp=5){ displayTemp();
      } else if (disp=6){ displayHum();
      } else { displayClockSqrAndMini();
      }

      
      Serial.println(disp);
 
    
    }
    //dmd.selectFont(SystemFont5x7);
    //tampilSuhu(19, 1);  
    //displayClockSqrAndMini();
    //tampilKelembaban(19, 0);
    //drawTextClockBig(8,0);
    //drawTextClock(0, 0);
  }
  
  if (menitAkhirAcak == (minute % 10) && detikAcak == second) 
  {
    tampilkanHariTanggal();
    menitAkhirAcak = rand() % 9;
    detikAcak = rand() % 59;
  }


}
