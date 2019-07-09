//Library for Http and MKR 1010
#include <ArduinoHttpClient.h>
#include <WiFiNINA.h>
//scheduler library
//#include <Scheduler.h>

//include SSID and password
#include "secrets.h"
char ssid[] = SECRET_SSID;
char pass[] = SECRET_PASS;

const char serverAddress[] = "tigoe.io";  // server address
int port = 443; //https port

WiFiSSLClient wifi;
HttpClient client = HttpClient(wifi, serverAddress, port);
int status = WL_IDLE_STATUS;
unsigned long timeStamp=0;
unsigned long timeAir=0;

//set pins
//digi
const int airLED = 12;
const int coolingLED = 2;
const int heatingLED = 3;
const int goodTempLED = 4;
//const int tempClick = 7;
const int greenLED = 7;
const int redLED = 6;
//ana
const int airPing = A3;
const int tempPing = A1;
const int knobPing = A2;
//vari
int count = 0; 
int brightness =0;

//set values
float tempVal = 0.00;
float airVal = 0.00;
float tempSet = 0.00;
int intervals = 0;

void setup() {
Serial.begin(9600);
//  while(!Serial);
checkConnection();

  pinMode(airLED,OUTPUT);
  pinMode(coolingLED,OUTPUT);
  pinMode(heatingLED,OUTPUT);
//  pinMode(tempClick,INPUT);
  pinMode(greenLED,OUTPUT);
  pinMode(redLED,OUTPUT);


  intervals = 1000*60*55;
//  intervals = 10000;

  timeStamp = millis();
  timeAir = millis();
}

void loop() {

    // reconnect
  if ( status != WL_CONNECTED) {
   checkConnection();
   } else {
    onGreen();
   offRed();
}

   //get and send data
   collectReadings();
   sendReadings();

   //get data locally every 0.5 s
   delay(500);

   //led brightness
   count++;
   brightness = (sin((count * PI / 180)  + 1) * 124);
  }

void collectReadings() {

  digitalWrite(airLED,LOW);
  float airRead = analogRead(airPing);
  
  float airVolt = airRead*(5.0/1024.0);
   airVal = 0.17*airVolt-0.10;
 
  //get temperture reading
  float tempRead = analogRead(tempPing);
  float voltIn = tempRead*(3.0 / 1024);
   tempVal = (voltIn - 0.5)*100; 

  //set temperture to
   tempSet = analogRead(knobPing);
  tempSet = -tempSet/30 + 40;

  //compare
  if (tempVal - tempSet > 1){
      //cooling
       cooling();
    }else if ( tempSet - tempVal > 1){
       //heating
       heating();
    }else{
      //good
      goodTemp();
    };
}


void cooling(){
   analogWrite(heatingLED, 0 );
   analogWrite(goodTempLED, 0 );
   analogWrite(coolingLED, brightness);
  }
 
void heating(){
  analogWrite(coolingLED , 0);
  analogWrite( goodTempLED , 0);
   analogWrite(heatingLED, brightness);
  }  
void goodTemp(){
  analogWrite(coolingLED , 0);
   analogWrite(heatingLED, 0 );
   analogWrite(goodTempLED, brightness);
  }
void sendReadings() {
  if (millis() - timeStamp > intervals){
   blinkGreen();
  String MACAdd = "84:0D:8E:34:64:A0";
  String sessionKey = "c8072ab1-331d-44c9-923e-686748870998";
  String path = "/data";
  String contentType = "application/json";

  String postData = "{\"macAddress\":\"";
  postData += MACAdd;
  postData += "\", \"sessionKey\" : \"";
  postData += sessionKey;
  postData += "\", \"data\" : \"{ \'temperature\' : ";
  postData += tempVal;
  postData += ", \'dustLevel\' : ";
  postData += airVal;
  postData += "}\"}";

  Serial.println("making POST request");
  client.post(path, contentType, postData);  // send the POST request

   // read the status code and body of the response
  int statusCode = client.responseStatusCode();
  String response = client.responseBody();

  Serial.print("Status code: ");
  Serial.println(statusCode);
  Serial.print("Response: ");
  Serial.println(response);
//  Serial.print("time: ");
//  Serial.println(timeStamp/1000);
//  Serial.print("data: ");
  Serial.println(postData);
timeStamp = millis();
  }

}
void checkConnection(){
  Serial.print("Attempting to connect to Network named: ");
    Serial.println(ssid);
    blinkRed();
    while ( WiFi.status() != WL_CONNECTED) {
    blinkRed();
//    blinkGreen();
    Serial.print("Attempting to connect to Network named: ");
    Serial.println(ssid);
    
   // Connect to WPA/WPA2 network:
    status = WiFi.begin(ssid, pass);
    Serial.print("SSID: ");
  Serial.println(WiFi.SSID());
  
  // print your WiFi shield's IP address:
  IPAddress ip = WiFi.localIP();
  Serial.print("IP Address: ");
  Serial.println(ip);
    }
    if (!wifi.connected()) {
      Serial.print("connected!");
      onGreen();
      offRed();
    }
    
}
void blinkRed(){
  //connecting
  if (millis()%500 == 0){
  digitalWrite(redLED,HIGH);
}else if (millis()%500 == 200){
  digitalWrite(redLED,LOW);
  }
  }
 void blinkGreen(){
  //send data
  if (millis()%1000 == 0){
  digitalWrite(greenLED,HIGH);
}else if (millis()%1000 == 500){
  digitalWrite(greenLED,LOW);
  }
  }
  void onGreen(){
    digitalWrite(greenLED,HIGH);
    //no error
    }
void offRed(){
  digitalWrite(redLED,LOW);
    //no error
    }
  
  
