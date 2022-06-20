
#define showDebug false
#include<string.h>

/* this project is smart box medicine */

#include "FirebaseESP8266.h"
#include <ESP8266WiFi.h>
#include "string.h"
#include "DHT.h"

#define DHTPIN 4
#define DHTTYPE DHT11

DHT dht(DHTPIN, DHTTYPE);

//#define WIFI_SSID "BK-MAKER"
//#define WIFI_PASSWORD "04042017"

#define WIFI_SSID "LoanDien 3"
#define WIFI_PASSWORD "244466666"

#define box11 "/BigFire/box/box1/status"
#define box12 "/BigFire/box/box1/value"

#define box21 "/BigFire/box/box2/status"
#define box22 "/BigFire/box/box2/value"

#define box31 "/BigFire/box/box3/status"
#define box32 "/BigFire/box/box3/value"

#define box41 "/BigFire/box/box4/status"
#define box42 "/BigFire/box/box4/value"

#define box51 "/BigFire/box/box5/status"
#define box52 "/BigFire/box/box5/value"

#define box61 "/BigFire/box/box6/status"
#define box62 "/BigFire/box/box6/value"

#define box71 "/BigFire/box/box7/status"
#define box72 "/BigFire/box/box7/value"

#define box1 "/BigFire/box/box1"
#define box2 "/BigFire/box/box2"
#define box3 "/BigFire/box/box3"
#define box4 "/BigFire/box/box4"
#define box5 "/BigFire/box/box5"
#define box6 "/BigFire/box/box6"
#define box7 "/BigFire/box/box7"

#define morning  "/BigFire/time/morning"
#define noom "/BigFire/time/noom"
#define evening "BigFire/time/evening"

#define trackingHum "BigFire/tracking/humidity"
#define trackingTemp "BigFire/tracking/temperature"



#define FIREBASE_HOST "https://flutteriot-11699-default-rtdb.asia-southeast1.firebasedatabase.app/" //Without http:// or https:// schemes
#define FIREBASE_AUTH "z3sBTQbz1ChBm4LsJW40qQiHRpXfQEAVdOxClJ6m"

//Define FirebaseESP8266 data object
FirebaseData firebaseData;

FirebaseJson json;

// there are vector store name path for each box
String path[7][2] = {{box11, box12}, {box21, box22}, {box31, box32}, {box41, box41}, {box52, box52}, {box61, box62}, {box71, box72}} ;
String pathBox[7] = {box1 , box2, box3 , box4, box5 , box6, box7 } ;
String pathTime[3] = {morning, noom , evening};
String pathTracking[2] = {trackingHum , trackingTemp }  ;


// there are vector store data for each box, when your module receive
String boxReceive[7][2] ; /* box[0][0] = value , box[0][1] = status */
String alarmReceive[3] ;  /* */

// there are vector store data for each box, when your module is transmitted for module Adruino mega
char boxTransmitted[7][2][100] ;
char alarmTransmitted[3][100] ;


void printResult(FirebaseData &data);
void printResultOk(FirebaseData &data , String result[10][2]);

void printResultOk(FirebaseData &data , String result[10][2]  )
{
  if (data.dataType() == "json")
  {

    FirebaseJson &json = data.jsonObject();

    String jsonStr;
    json.toString(jsonStr, true);
    size_t len = json.iteratorBegin();
    String key, value = "";
    int type = 0;

    for (size_t i = 0; i < len; i++)
    {
      json.iteratorGet(i, type, key, value);
      if (type == FirebaseJson::JSON_OBJECT)
      {
        result[i][0] =  key  ;
      }
      result[i][1] = value ;
    }
    json.iteratorEnd();

  }
}



void printResult(FirebaseData &data   )
{
  if (data.dataType() == "json")
  {
    //Serial.println();
    FirebaseJson &json = data.jsonObject();
    //Print all object data
    //Serial.println("Pretty printed JSON data:");
    String jsonStr;
    json.toString(jsonStr, true);
    //Serial.println(jsonStr);
    //Serial.println();
    //Serial.println("Iterate JSON data:");
    //Serial.println();
    size_t len = json.iteratorBegin();
    String key, value = "";
    int type = 0;

    for (size_t i = 0; i < len; i++)
    {
      json.iteratorGet(i, type, key, value);
      if (type == FirebaseJson::JSON_OBJECT)
      {
        int ben = 1 ;
      }
    }
    json.iteratorEnd();

  }
}

void updateData( String path , char* key , String data  )
{
  json.clear();

  json.set( key  , data );

  if (Firebase.updateNode(firebaseData, path , json))
  {
    if ( showDebug)
    {
      Serial.println("PASSED");
      Serial.println("PATH: " + firebaseData.dataPath());
      Serial.println("TYPE: " + firebaseData.dataType());
      //No ETag available
      Serial.print("VALUE: ");
      Serial.println("------------------------------------");
      Serial.println();

    }

    printResult(firebaseData);

  }
  else
  {
    if ( showDebug )
    {
      Serial.println("FAILED");
      Serial.println("REASON: " + firebaseData.errorReason());
      Serial.println("------------------------------------");
      Serial.println();
    }

  }

}



void getData( String path , String result[2][2] )
{

  //Also can use Firebase.get instead of Firebase.setInt
  if (Firebase.getJSON(firebaseData, path ))
  {
    if ( showDebug )
    {
      Serial.println("PASSED");
      Serial.println("PATH: " + firebaseData.dataPath());
      Serial.println("TYPE: " + firebaseData.dataType());
      Serial.println("ETag: " + firebaseData.ETag());
      Serial.print("VALUE: ");
      Serial.println("------------------------------------");
      Serial.println();

    }

    printResultOk(firebaseData , result );

  }
  else if ( showDebug )
  {
    Serial.println("FAILED");
    Serial.println("REASON: " + firebaseData.errorReason());
    Serial.println("------------------------------------");
    Serial.println();
  }
}



void pushData( String path, String data )
{
  json.set("value", data );
  String arrStr;
  json.toString(arrStr, true);
  //Serial.println ( String (arrStr)  ) ;
  //  json.clear().add("DataLoveYou", data  );

  //Also can use Firebase.push instead of Firebase.pushJSON
  //Json string is not support in v 2.6.0 and later, only FirebaseJson object is supported.
  if (Firebase.pushJSON(firebaseData, path , json ))
  {
    if ( showDebug )
    {
      Serial.println("PASSED");
      Serial.println("PATH: " + firebaseData.dataPath());
      Serial.print("PUSH NAME: ");
      Serial.println(firebaseData.pushName());
      Serial.println("ETag: " + firebaseData.ETag());
      Serial.println("------------------------------------");
      Serial.println();
    }

  }
  else if ( showDebug )
  {
    Serial.println("FAILED");
    Serial.println("REASON: " + firebaseData.errorReason());
    Serial.println("------------------------------------");
    Serial.println();
  }
}



void SETData( String path, int data )
{
  //Also can use Firebase.push instead of Firebase.pushJSON
  //Json string is not support in v 2.6.0 and later, only FirebaseJson object is supported.
  if (Firebase.setInt(firebaseData, path , data))
  {
    if (showDebug)
    {
      Serial.println("PASSED");
      Serial.println("PATH: " + firebaseData.dataPath());
      Serial.print("PUSH NAME: ");
      Serial.println(firebaseData.pushName());
      Serial.println("ETag: " + firebaseData.ETag());
      Serial.println("------------------------------------");
      Serial.println();
    }

  }
  else if (showDebug)
  {
    Serial.println("FAILED");
    Serial.println("REASON: " + firebaseData.errorReason());
    Serial.println("------------------------------------");
    Serial.println();
  }
}


void tracking(String value[2] )
{
  float h = dht.readHumidity();
  float t = dht.readTemperature();
  value[0] = String(h);
  value[1] = String(t);
  
  updateData( pathTracking[0] , "value", value[0] ) ;
  updateData( pathTracking[1] , "value", value[1] ) ;
  if ( showDebug )
  {
    Serial.print("Humidity: ");
    Serial.print(h);
    Serial.print("%  Temperature: ");
    Serial.print(t);
    Serial.print("°C ");
    Serial.println() ;

  }

}

void receive(String path , String result[4] )
{ // Nhận một chuỗi ký tự từ ESP8266 rồi xử lý ---- vd: box1 0 05 10 20 (viết liền) thì: 0-trạng thái đóng mở, 05- sáng uống 0.5 viên, ...
  String str;
  if (Serial.available()) {
    str = Serial.readString() ; 
    //str = Serial.readStringUntil('\n');    // nhận ký tự
    Serial.println(str);
    
    int viTriBox1 = str.indexOf(path);
    Serial.println(viTriBox1) ; 
    //Serial.println() ;
    //Serial.println(str) ;
    if (1)
    {
      result[0] = str.substring(viTriBox1 + 5, viTriBox1 + 6);
      result[1] = str.substring(viTriBox1 + 6, viTriBox1 + 8);
      result[2] = str.substring(viTriBox1 + 8, viTriBox1 + 10);
      result[3] = str.substring(viTriBox1 + 10, viTriBox1 + 12);
    }
  }
}

void setup()
{

  Serial.begin(9600);

  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  if ( showDebug )
  {
    Serial.print("Connecting to Wi-Fi");
  }

  while (WiFi.status() != WL_CONNECTED)
  {
    if ( showDebug )
      Serial.print(".");

    //delay(300);
  }

  if ( showDebug )
  {
    Serial.println();
    Serial.print("Connected with IP: ");
    Serial.println(WiFi.localIP());
    Serial.println();
  }


  Firebase.begin(FIREBASE_HOST, FIREBASE_AUTH);
  Firebase.reconnectWiFi(true);

  //Set the size of WiFi rx/tx buffers in the case where we want to work with large data.
  firebaseData.setBSSLBufferSize(1024, 1024);

  //Set the size of HTTP response buffers in the case where we want to work with large data.
  firebaseData.setResponseSize(1024);

  //Set database read timeout to 1 minute (max 15 minutes)
  Firebase.setReadTimeout(firebaseData, 1000 * 60);
  //tiny, small, medium, large and unlimited.
  //Size and its write timeout e.g. tiny (1s), small (10s), medium (30s) and large (60s).
  Firebase.setwriteSizeLimit(firebaseData, "tiny");

  //optional, set the decimal places for float and double data to be stored in database
  Firebase.setFloatDigits(2);
  Firebase.setDoubleDigits(6);
  dht.begin();
  for ( int i = 0; i < 7 ; i++ )
  {
    SETData( path[i][0] , 0 ) ;
    SETData( path[i][1] , 0 ) ;
    if (i < 3)
    {
      SETData(pathTime[i] + "/time" , 0 ) ;
    }
    if ( i < 2 )
    {
      SETData( pathTracking[i] + "/value" , 0 )   ;
    }

  }
//  //delay(100) ;

}

String result[4] ;


void loop()
{

  String dataTracking[2] ;
  tracking( dataTracking ) ;
  int count  = 0 ;
  for ( int i = 0; i < 7 ; i++ )
  {
    count += 1 ;
//    updateData( pathBox[i], "status", String(count) );
//    updateData( pathBox[i], "value" , String(count * 1000000) );
    if ( i < 3 )
    {
      updateData( pathTime[i] , "time", String (count) ) ;
    }
    //delay(20) ;

  }


  String dataOutput[7][2][2]; //7 box; value, status; 2ky tu
  String dataTime[3][1][2] ;  // 3 time: sang trua toi; 1 gia tri; 2 ky tu
  for ( int i = 0  ; i < 7 ; i++ )
  {
    getData(pathBox[i], dataOutput[i]);

  }

  for ( int i = 0; i < 3 ; i++)
  {
    getData( pathTime[i], dataTime[i] ) ;
  }

  for ( int j = 0 ; j < 7 ; j++ )
  {

    for ( int i = 0 ; i < 2 ; i++  )
    {
      dataOutput[j][i][1].toCharArray(boxTransmitted[j][i], 100);
      if (showDebug)
      {
        Serial.println("Data output from transmitted");
        Serial.println( boxTransmitted[j][i]);
      }


      //        Serial.println( dataOutput[j][i][0]);
      //
      //        Serial.println( dataOutput[j][i][1]);

    }
  }
  for ( int i = 0 ; i < 3 ; i++ )
  {

    dataTime[i][0][1].toCharArray(alarmTransmitted[i], 100) ;
    if (showDebug)
    {
      Serial.println("Data alarm Transmitted") ;
      Serial.println( alarmTransmitted[i] ) ;
    }

    //      Serial.println( dataTime[i][0][0] ) ;
    //      Serial.println( dataTime[i][0][1] ) ;
  }



  //delay(20) ;

//  Serial.println() ;
//  Serial.print("BOX1 ") ;
//  Serial.print(boxTransmitted[0][0]);
//  Serial.print(boxTransmitted[0][1]);
//  Serial.println();
  
  receive("BOX1_",  result ) ;
  String ben = result[1] + " " + result[2] + " " + result[3];
  updateData( pathBox[0], "status", result[0] );
  updateData( pathBox[0], "value",  ben );

}
