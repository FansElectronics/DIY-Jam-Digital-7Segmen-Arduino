/*------------------------------------------------------------------------------------------------------------------------------------------------------------
 *  Project Name : Jam Digital 7 Segmen 4 Digit Shift Register 74HC595
 *  Author       : Irfan Indra Kurniawan
 *  Website      : www.FansElectronics.com
 *  Email        : admin@fanselectronics.com
 -----------------------------------------------------------------------------------------------------------------------------------------------------------*/
//============================================================================================================================================================ 
                                                                       // File Library //
#include <FansElectronics_SevenSegmen595.h>                                                                                 
#include <TimerOne.h>      
#include <RTClib.h>                                                                                      
//============================================================================================================================================================ 
                                                                  // Define Pin Konfigurasi //
#define bOK             2                                                                                       
#define bLF             1                                                                                      
#define bRG             0      
                                                                                                                                                               
#define Data            9                                                                                        
#define Clock           11                                                                                     
#define Latch           10                                                                                   
#define Dig4            8                                                                                       
#define Dig3            7                                                                                    
#define Dig2            5                                                                                      
#define Dig1            3     
#define point           6               
                                                                    
//============================================================================================================================================================ 
                                                                      // Global Varible //
int jam, menit, detik, tanggal, bulan, tahun;
bool sett = false;
bool ledBlink = false; 
double lol, hold;

SevenSegmen595 segmen(Data,Clock,Latch,Dig1,Dig2,Dig3,Dig4,TRAINER);
RTC_DS1307 rtc;

//============================================================================================================================================================ 
                                                                          // Setup //
void setup() {
  rtc.begin();
  //rtc.adjust(DateTime(F(__DATE__),F(__TIME__)));      // Singkron rtc dgn PC jangan aktifkan jika tidak digunakan

  Timer1.initialize(2000);          
  Timer1.attachInterrupt(blink_point);  
  attachInterrupt(digitalPinToInterrupt(bOK),setting,FALLING);
  
  pinMode(bOK,INPUT_PULLUP);
  pinMode(bLF,INPUT_PULLUP);
  pinMode(bRG,INPUT_PULLUP);  
  pinMode(point, OUTPUT);
}

//============================================================================================================================================================ 
                                                                         // Looping //
void loop() {
  DateTime now = rtc.now();
//========================================================== 
  hold = millis();
  while((hold + 20000) > millis()) {    // Jeda 20 Detik
    now = rtc.now();
    jam = now.hour();
    menit = now.minute();
    ledBlink = true;
    if(sett) {menuSetting();}
    // Tampilkan display. Jangan pernah diberi delay
    segmen.setNumber(waktu(jam, menit));
  }
  ledBlink = false;
  segmen.clear();
//==========================================================   
  hold = millis();
  while((hold + 5000) > millis()) {     // Jeda 5 Detik
    now = rtc.now();
    bulan = now.month();
    tanggal = now.day();
    digitalWrite(point,HIGH);
    if(sett) {return;}
    // Tampilkan display. Jangan pernah diberi delay
    segmen.setNumber(waktu(tanggal, bulan));
  }
  segmen.clear();
//========================================================== 
  hold = millis();
  while((hold + 5000) > millis()) {     // Jeda 5 Detik
    now = rtc.now();
    tahun = now.year();
    digitalWrite(point,LOW);
    if(sett) {return;}
    // Tampilkan display. Jangan pernah diberi delay
    segmen.setNumber(tahun);
  }
  segmen.clear(); 
}

//============================================================================================================================================================ 
                                                               // Fungsi ISR Interrupt Tombol OK //
void setting() {
  sett = true;
}

//============================================================================================================================================================ 
                                                                  // Fungsi Interrupt Timer 1 //
void blink_point() {
  if(ledBlink) {
    if(millis() > (lol+500)) {digitalWrite(point, HIGH);}
    if(millis() < (lol+1000) && millis() > (lol+500)) {digitalWrite(point, LOW);}
    if(millis() > (lol+1000)) {lol = millis();}   
  }
}

//============================================================================================================================================================ 
                                                                // Variable Konversi Nilai Jam //
int waktu(int jm, int mnt){
  jm = jm * 100;
  return jm + mnt;
}

//============================================================================================================================================================ 
                                                                    // Fungsi Menu Setting //
void menuSetting() {
  DateTime now = rtc.now();
  jam = now.hour();
  menit = now.minute();
  ledBlink = false;
  detachInterrupt(digitalPinToInterrupt(bOK));
  while(digitalRead(bOK) == LOW) {}
//===========================================================================================  
  setWaktu:
  hold = millis();
  while((hold + 500) > millis()) {
    digitalWrite(point, HIGH);
    now = rtc.now();
    if(digitalRead(bLF) == LOW) {
      delay(10);  jam++;
      if(jam == 24) {jam = 0;}
      while(digitalRead(bLF) == LOW){}
    }
    if(digitalRead(bRG) == LOW) {
      delay(10); menit++;
      if(menit == 60) menit = 0;
      while(digitalRead(bRG) == LOW) {}
    }
    if(digitalRead(bOK) == LOW) {
      rtc.adjust(DateTime(now.year(), now.month(), now.day(), jam, menit, now.second()));
      while(digitalRead(bOK) == LOW) {}
      delay(10); goto setTglBln;
    }
    segmen.setNumber(waktu(jam,menit));
  }
  segmen.clear();
  goto setWaktu;
//===========================================================================================
  setTglBln:
  hold = millis();
  while((hold + 500) > millis()) {
    digitalWrite(point, HIGH);
    now = rtc.now();
    if(digitalRead(bLF) == LOW) {
      delay(10);  tanggal++;
      if(tanggal > 31) {tanggal = 1;}
      while(digitalRead(bLF) == LOW){}
    }
    if(digitalRead(bRG) == LOW) {
      delay(10); bulan++;
      if(bulan > 12) bulan = 1;
      while(digitalRead(bRG) == LOW) {}
    }
    if(digitalRead(bOK) == LOW) {
      rtc.adjust(DateTime(now.year(), bulan, tanggal, now.hour(), now.minute(), now.second()));
      while(digitalRead(bOK) == LOW) {}
      delay(10); goto setTahun;
    }
    segmen.setNumber(waktu(tanggal,bulan));
  }
  segmen.clear();
  goto setTglBln;
//===========================================================================================
  setTahun:
  hold = millis();
  while((hold + 500) > millis()) {
    digitalWrite(point, LOW);
    now = rtc.now();
    if(digitalRead(bLF) == LOW) {
      delay(10);  tahun--;
      if(tahun < 2017) {tahun = 2050;}
      while(digitalRead(bLF) == LOW){}
    }
    if(digitalRead(bRG) == LOW) {
      delay(10);  tahun++;
      if(tahun > 2050) {tahun = 2017;}
      while(digitalRead(bRG) == LOW) {}
    }
    if(digitalRead(bOK) == LOW) {
      rtc.adjust(DateTime(tahun, now.month(), now.day(), now.hour(), now.minute(), now.second()));
      while(digitalRead(bOK) == LOW) {}
      delay(10); goto endSetting;
    }
    segmen.setNumber(waktu(tanggal,bulan));
  }
  segmen.clear();
  goto setTahun;
//===========================================================================================  
  endSetting:
    delay(500);
    while(digitalRead(bOK) == LOW) {}
    attachInterrupt(digitalPinToInterrupt(bOK),setting,FALLING);
    sett = false;
}
