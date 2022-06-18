#include<string.h>
#include <SD.h> 
#define SD_ChipSelectPin 53
#include <TMRpcm.h>   // Thư viện loa
#include <SPI.h> 
#include "RTClib.h"
#include <SoftwareSerial.h>

SoftwareSerial mySerial(0, 1);
RTC_DS1307 rtc;
TMRpcm tmrpcm;

#define latchPin 4 //chân ST_CP của 74HC595
#define clockPin 5 //chân SH_CP của 74HC595
#define dataPin 6  //Chân DS của 74HC595

#define led7seg1 7 //Chân điều khiển led7--1
#define led7seg2 8 //Chân điều khiển led7--2

#define button1 9  //Chân nhận tín hiệu nút Trừ    47
#define button2 10  //Chân nhận tín hiệu nút Cộng   45
#define button3 11  //Chân nhận tín hiệu nút Cài đặt 43
#define button4 12  //Chân nhận tín hiệu nút preview

////Chân nhận tín hiệu mở nắp ngăn 1-->7
//#define signal_1 22 // ngăn 1
//#define signal_2 9
//#define signal_3 26
//#define signal_4 28
//#define signal_5 30
//#define signal_6 32
//#define signal_7 34

//Chân điều khiển đèn ngăn 1-->7
#define latchPin1 A3 //chân ST_CP của 74HC595
#define clockPin1 A4 //chân SH_CP của 74HC595
#define dataPin1 A5  //Chân DS của 74HC595

//Chaân điều khiển đèn Sáng-Trưa-Tối
#define led_sang A0
#define led_trua A1
#define led_toi A2

#define soLuongKyTuData 12
int ledOn;
// Các bit được đánh số thứ tự (0-7) từ phải qua trái (tương ứng với DP,A-F)
// Vì dùng LED 7 đoạn chung cực dương, nên với các bit 0 thì đoạn đó của LED sẽ sáng
//mảng có 10 số (từ 0-9)
const int Seg[10] = { // không có dấu chấm- dùng cho hàng đơn vị
  0b11000000,//0 
  0b11111001,//1 
  0b10100100,//2
  0b10110000,//3
  0b10011001,//4
  0b10010010,//5
  0b10000010,//6
  0b11111000,//7
  0b10000000,//8
  0b10010000,//9
};
const int Seg2[10] = { // có dấu chấm - dùng cho hàng chục
  0b01000000,//0 
  0b01111001,//1 
  0b00100100,//2
  0b00110000,//3
  0b00011001,//4
  0b00010010,//5
  0b00000010,//6
  0b01111000,//7
  0b00000000,//8
  0b00010000,//9
};

const int Led[7] = {
  0b00000001,
  0b00000010,
  0b00000100,
  0b00001000,
  0b00010000,
  0b00100000,
  0b01000000,
};

//-------------------------
unsigned long thoiGian;
int soLuongx2_sang[8] = {0, 0, 0, 0, 0, 0, 0, 0};
int soLuongx2_trua[8] = {0, 0, 0, 0, 0, 0, 0, 0};
int soLuongx2_toi[8] = {0, 0, 0, 0, 0, 0, 0, 0};

int box[8][4] =  {0, 0, 0, 0,     // box[i][0]: cột trạng thái đóng-0 mở-1
                  0, 10, 25, 30,  // box[i][1]: cột số lượng sáng  {
                  0, 0, 0, 0,     // box[i][2]: cột số lượng trưa { ba cột số lượng để x10 
                  0, 0, 0, 0,     // box[i][3]: cột số lượng tối   {
                  0, 0, 0, 0,
                  0, 0, 0, 0,
                  0, 0, 0, 0,
                  0, 0, 0, 0};
int daLayThuoc[8] = {0, 0, 0, 0, 0, 0, 0, 0};
int dangCaiDat = 0;

int sesson = 0; // 1 - buoi sang , 2-buoi trua , 3-buoi toi

int clockTime_sang = 7;  // 07h00m
int clockTime_trua = 12;
int clockTime_toi = 19;

void setup(){
  mySerial.begin(9600);
  Serial.begin(9600);
  
  pinMode(latchPin, OUTPUT);
  pinMode(clockPin, OUTPUT);
  pinMode(dataPin, OUTPUT);
  pinMode(led7seg1, OUTPUT);
  pinMode(led7seg2, OUTPUT);
  pinMode(latchPin1, OUTPUT);
  pinMode(clockPin1, OUTPUT);
  pinMode(dataPin1, OUTPUT);
  pinMode(led_sang, OUTPUT);
  pinMode(led_trua, OUTPUT);
  pinMode(led_toi, OUTPUT);
  
  pinMode(button1, INPUT);
  pinMode(button2, INPUT);
  pinMode(button3, INPUT);
//  
//  pinMode(signal_1, INPUT);
//  pinMode(signal_2, INPUT);
//  pinMode(signal_3, INPUT);
//  pinMode(signal_4, INPUT);
//  pinMode(signal_5, INPUT);
//  pinMode(signal_6, INPUT);
//  pinMode(signal_7, INPUT);
  
  digitalWrite(led7seg1, HIGH);
  digitalWrite(led7seg2, HIGH);

  rtc.begin();
  if (! rtc.begin()) {
    Serial.println("Couldn't find RTC");
    Serial.flush();
    while (1) delay(10);
  }
  
}

void changeTru(int *p);
void changeCong(int *p);
void tinhSoLuong(int *p, int *q);
void shiftout_hangChuc(int point);
void shiftout_hangDonVi(int point);
void hienThiMotLed(int so);
void hienThiHaiLed(int so);
void batLed(int);
void tatLed(int);
void turnOffAllLed();
void turnOnAllLed();

int stringToDigit(String);
void inputDataBox(int, String, int);
void getData();
void sendData(int);

int nhanDienMoNap();
void caiDatSoLuong_sang(int);
void caiDatSoLuong_trua(int);
void caiDatSoLuong_toi(int);
void caiDatBaBuoi(int);
void caiDat(int*);
void hienThiSoLuong(int, int);
int checkTime();
//int checkSignal();
//void loaBaoSoLuong(int);
//void searchAndShow();
bool chuaLayHetThuoc();
void suDung(int*);

void loop() { 
  if(digitalRead(button3)){
    while(digitalRead(button3)){}
    dangCaiDat = 1;
  }
  while(dangCaiDat){
      caiDat(&dangCaiDat);
  }
  turnOffAllLed();
//  getData();
  while(!dangCaiDat){
    suDung(&dangCaiDat);
  }
  turnOffAllLed();
}

//----------------------------------------function----------------------------------------------------
void changeTru(int *p){
  *p = *p - 1;
}
void changeCong(int *p){
  *p = *p + 1;
}
void tinhSoLuong(int *p, int *q){
  *p = (*(q))*10/2;
}
//-------------------------
void shiftout_hangChuc(int point){
  //shiftout - start
  digitalWrite(latchPin, LOW);
  //Xuất bảng ký tự ra cho LED
  shiftOut(dataPin, clockPin, MSBFIRST, Seg2[point]);  
  digitalWrite(latchPin, HIGH);
  //shiftout - end
}
//-------------------------
void shiftout_hangDonVi(int point){
  //shiftout - start
  digitalWrite(latchPin, LOW);
  //Xuất bảng ký tự ra cho LED
  shiftOut(dataPin, clockPin, MSBFIRST, Seg[point]);  
  digitalWrite(latchPin, HIGH);
  //shiftout - end
}
//-------------------------
void hienThiMotLed(int so){
    so /= 10;
    digitalWrite(led7seg2, HIGH);
    shiftout_hangDonVi(so);
    digitalWrite(led7seg1, LOW); 
}
//-------------------------
void hienThiHaiLed(int so){
  int hangChuc = so / 10;
  int hangDv = so % 10;
  if(!(millis() % 2)){
    digitalWrite(led7seg2, HIGH);
    shiftout_hangChuc(hangChuc);
    digitalWrite(led7seg1, LOW);
  }
  else{
    digitalWrite(led7seg1, HIGH);
    shiftout_hangDonVi(hangDv);
    digitalWrite(led7seg2, LOW);
  }
}
//-------------------------
void shiftOutLed(int i){
   //shiftout - start
  digitalWrite(latchPin1, LOW);
  shiftOut(dataPin1, clockPin1, MSBFIRST, Led[i]);  
  digitalWrite(latchPin1, HIGH);
}
void batLed(int i){
  int n = ledOn | Led[i];
    //shiftout - start
  digitalWrite(latchPin1, LOW);
  shiftOut(dataPin1, clockPin1, MSBFIRST, n);  
  digitalWrite(latchPin1, HIGH);
  //shiftout - end
}
void tatLed(int i){
  int n = ledOn - Led[i];
    //shiftout - start
  digitalWrite(latchPin1, LOW);
  shiftOut(dataPin1, clockPin1, MSBFIRST, n);  
  digitalWrite(latchPin1, HIGH);
  //shiftout - end
}

void turnOffAllLed(){
  digitalWrite(latchPin1, LOW);
  shiftOut(dataPin1, clockPin1, MSBFIRST, 0);  
  digitalWrite(latchPin1, HIGH);
  //shiftout - end
   digitalWrite(led_sang, LOW);
  digitalWrite(led_trua, LOW);
  digitalWrite(led_toi, LOW);
  digitalWrite(led7seg1, HIGH);
  digitalWrite(led7seg2, HIGH);
}

void turnOnAllLed(){
  digitalWrite(led_sang, HIGH);
  digitalWrite(led_trua, HIGH);
  digitalWrite(led_toi, HIGH);
  digitalWrite(led7seg1, HIGH);
  digitalWrite(led7seg2, HIGH);
} 

int stringToDigit(String str1){
  int len = str1.length();
  int num = 0;
  for(int i = len-1; i >= 0; --i){
    num +=  (int)(str1[i]-'0')*pow(10, len-i-1) ;
  }
  return num;
} 

void inputDataBox(int cases, String str, int viTri){
  String str1;
  str1 = str.substring(viTri+5, viTri+6);
  box[cases][0] = stringToDigit(str1);  // ghi trạng thái box-n
  str1 = str.substring(viTri+6, viTri+8);
  box[cases][1] = stringToDigit(str1);  // ghi số lượng sáng cho box-cases
  str1 = str.substring(viTri+8, viTri+10);
  box[cases][2] = stringToDigit(str1);  // ghi số lượng trưa cho box-cases
  str1 = str.substring(viTri+10, viTri+12);
  box[cases][3] = stringToDigit(str1);  // ghi số lượng tối cho box-cases
}

void getData(){
  String str;
  if(mySerial.available()){
    str = mySerial.readStringUntil('\n');  // nhận ký tự 
    Serial.println(str);
  }
}

void sendData (int soNganThuoc){
  String text = "";
  switch(soNganThuoc){
    case 1: text = "BOX1 ";
      break;
    case 2: text = "BOX2 ";
      break;
    case 3: text = "BOX3 ";
      break;
    case 4: text = "BOX4 ";
      break;
    case 5: text = "BOX5 ";
      break;
    case 6: text = "BOX6 ";
      break;
    default: text = "BOX7 ";
      break;
  }
  for(int i = 0; i <=3; i++){
    text += String(box[soNganThuoc][i]);
  }
  mySerial.println(text);
}

void caiDatSoLuong_sang(int soNganThuoc){
  if (digitalRead(button1) && soLuongx2_sang[soNganThuoc] > 0){
    while(digitalRead(button1)){
    }
    changeTru(&soLuongx2_sang[soNganThuoc]);
    if(soLuongx2_sang[soNganThuoc] > 4){
      changeTru(&soLuongx2_sang[soNganThuoc]);
    }
  }
  else if (digitalRead(button2) && soLuongx2_sang[soNganThuoc] <= 16){
    while(digitalRead(button2)){
    }
    changeCong(&soLuongx2_sang[soNganThuoc]);
    if(soLuongx2_sang[soNganThuoc] > 4){
      changeCong(&soLuongx2_sang[soNganThuoc]);
    }
  }
  //soLuong_sang[soNganThuoc] = soLuongx2_sang[soNganThuoc]*10/2;
  tinhSoLuong(&box[soNganThuoc][1], &soLuongx2_sang[soNganThuoc]);
  if(box[soNganThuoc][1] % 10 == 0)
    hienThiMotLed(box[soNganThuoc][1]);
  else hienThiHaiLed(box[soNganThuoc][1]);
}
void caiDatSoLuong_trua(int soNganThuoc){
  if (digitalRead(button1) && soLuongx2_trua[soNganThuoc] > 0){
    while(digitalRead(button1)){
    }
    //soLuongx2_trua[soNganThuoc]--;
    changeTru(&soLuongx2_trua[soNganThuoc]);
    if(soLuongx2_trua[soNganThuoc] > 4){
      changeTru(&soLuongx2_trua[soNganThuoc]);
    }
  }
  else if (digitalRead(button2) && soLuongx2_trua[soNganThuoc] < 18){
    while(digitalRead(button2)){
    }
    changeCong(&soLuongx2_trua[soNganThuoc]);
    if(soLuongx2_trua[soNganThuoc] > 4){
      changeCong(&soLuongx2_trua[soNganThuoc]);
    }
  }
  //soLuong_trua[soNganThuoc] = soLuongx2_trua[soNganThuoc]*10/2;
  tinhSoLuong(&box[soNganThuoc][2], &soLuongx2_trua[soNganThuoc]);
  if(box[soNganThuoc][2] % 10 == 0)
    hienThiMotLed(box[soNganThuoc][2]);
  else hienThiHaiLed(box[soNganThuoc][2]);
}
void caiDatSoLuong_toi(int soNganThuoc){
  if (digitalRead(button1) && soLuongx2_toi[soNganThuoc] > 0){
     while(digitalRead(button1)){
     }
     changeTru(&soLuongx2_toi[soNganThuoc]);
     if(soLuongx2_toi[soNganThuoc] > 4){
        changeTru(&soLuongx2_toi[soNganThuoc]);
     }
  }
  else if (digitalRead(button2) && soLuongx2_toi[soNganThuoc] < 18){
          while(digitalRead(button2)){
          }
          changeCong(&soLuongx2_toi[soNganThuoc]);
          if(soLuongx2_toi[soNganThuoc] > 4){
            changeCong(&soLuongx2_toi[soNganThuoc]);
          }
       }
  //soLuong_toi[soNganThuoc] = soLuongx2_toi[soNganThuoc]*10/2;
  tinhSoLuong(&box[soNganThuoc][3], &soLuongx2_toi[soNganThuoc]);
  if(box[soNganThuoc][3] % 10 == 0)
    hienThiMotLed(box[soNganThuoc][3]);
  else hienThiHaiLed(box[soNganThuoc][3]);
}

void caiDatBaBuoi(int soNganThuoc){
      digitalWrite(led_sang, HIGH);
      digitalWrite(led_trua, LOW);
      digitalWrite(led_toi, LOW);
      while (!digitalRead(button3)){ 
        caiDatSoLuong_sang(soNganThuoc);
      }
      while(digitalRead(button3)){
      }
      digitalWrite(led_sang, LOW);
      digitalWrite(led_trua, HIGH);
      digitalWrite(led_toi, LOW);
      while(!digitalRead(button3)){
        caiDatSoLuong_trua(soNganThuoc);
      }
      while(digitalRead(button3)){
      }
      digitalWrite(led_sang, LOW);
      digitalWrite(led_trua, LOW);
      digitalWrite(led_toi, HIGH);
      while(!digitalRead(button3)){
        caiDatSoLuong_toi(soNganThuoc);
      }
      while(digitalRead(button3)){
      }
}
void caiDatBox(int i){
  turnOffAllLed();
  shiftOutLed(i);
  caiDatBaBuoi(i);
  sendData(i);
}

void caiDat (int *cd) {
  
  if(digitalRead(button3)){
    while(digitalRead(button3)){};
    *cd = 0;
  } else if (!digitalRead(button3)){
    turnOnAllLed();
    digitalWrite(led7seg1, HIGH);
    digitalWrite(led7seg2, HIGH);
  } else for (int i = 1; i < 8; i++){
    if(digitalRead(button3)){
      while(digitalRead(button3)){};
      caiDatBox(i);
      i++;
    }
  } 
}

void hienThiSoLuong(int soNganThuoc, int buoi){
    if(box[soNganThuoc][buoi] % 10 == 0)
      hienThiMotLed(box[soNganThuoc][buoi]);
    else hienThiHaiLed(box[soNganThuoc][buoi]);
}

int checkTime(){
  DateTime now = rtc.now();

    int timeHour;
    timeHour = int(now.hour());
  if (timeHour = clockTime_sang){
    sesson = 1;
    for(int i=1; i<=7; i++){
        if(box[i][1] > 0){
          daLayThuoc[i] = 0;
        }
    }
  } else if (timeHour = clockTime_trua){
      sesson = 2;
        for(int i=1; i<=7; i++){
           if(box[i][2] > 0){
           daLayThuoc[i] = 0;
        }
     }
  } else if (timeHour = clockTime_toi){
    sesson = 3;
         for(int i=1; i<=7; i++){
            if(box[i][3] > 0){
            daLayThuoc[i] = 0;
         }
      }
   } else sesson = 0;
   return sesson;
}

bool chuaLayHetThuoc(){
  bool chuaLay = 0;
  for(int i=1; i<=7; i++){
    if(box[i][sesson] > 0 && daLayThuoc[i] == 0) chuaLay = 1; 
  }
  return chuaLay;
}

void suDung(int *cd){
  int tg = checkTime();
  unsigned long time1 = millis();
  while(millis()-time1 < 60000 && chuaLayHetThuoc()){              // Báo trong 3 phút, nếu không có nắp nào mở
    for(int i=1; i <= 7; i++){
        if(box[i][tg] > 0 && daLayThuoc[i] == 0){
          batLed(i);
        }
        else tatLed(i);
    }
    tmrpcm.play ("ring.wav");
    delay (1000) ;
  }
  turnOffAllLed();
  while(chuaLayHetThuoc()){
    for(int i=1; i<=7; i++){                    // bật Led các ngăn chưa lấy thuốc
      if(box[i][tg] > 0 && daLayThuoc[i] == 0){
        batLed(i);
      }
      else tatLed(i);
    }    
    int openedCase = checkSignal();
    if(openedCase != 0 && box[openedCase][tg] > 0 && daLayThuoc[openedCase] == 0){  //nếu mở nắp 1 trong các ngăn cần lấy thuốc
      turnOffAllLed();
      batLed(openedCase);  // sáng đèn ngăn đó
      searchAndShow();      // loa và hiển thi số lượng đến khi đóng nắp
      daLayThuoc[openedCase] = 1;
    }
  }
  if(digitalRead(button3)){
    while(digitalRead(button3)){};
    *cd = 1;
  }
}
