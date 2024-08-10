#define BLYNK_TEMPLATE_ID "TMPL2vmltBkK9"
#define BLYNK_TEMPLATE_NAME "ATM"
#define BLYNK_AUTH_TOKEN "QnormjHyk1IMOfyowTs9jkApFdwOVxgs"

#define BLYNK_PRINT Serial
#include <WiFi.h>
#include <WiFiClient.h>
#include <BlynkSimpleEsp32.h>

BlynkTimer timer;

//BLYNK APP AND INTENET CONNECTIONS
char auth[] = BLYNK_AUTH_TOKEN;
char ssid[] = "Galaxy";
char pass[] = "12345678";
 ////////////====
 #include<LiquidCrystal_I2C.h>
LiquidCrystal_I2C lcd(0x27, 16, 2);  // Adjust the I2C address if necessar
 
 /////////gps
#include <TinyGPS++.h>
#include <SoftwareSerial.h>
static const int RXPin = 18, TXPin = 19;// Here we make pin 4 as RX of arduino & pin 3 as TX of arduino 
static const uint32_t GPSBaud = 9600;
TinyGPSPlus gps;
SoftwareSerial gps_module(RXPin, TXPin);
//
//String longitude;
//String latitude; 
WidgetMap myMap(V5); //V0 - virtual pin for Map

// /////////////
//#include <SoftwareSerial.h>  
//SoftwareSerial SIM900(17,16); // SoftSerial( RX , TX );   //Create software serial object to communicate with SIM800L
#define green 25 // Load1 Pin Out
#define Redled 26 // Load1 Pin Out
#define GASPin 27 // gas Pin IN
#define buzzer 14 // Load3 Pin Out
int Sound_pin=12;
String message;

/////////axal///
//--------------------------------------------------------------
#define xPin 34
#define yPin 35
#define zPin 32
//--------------------------------------------------------------
// Take multiple samples to reduce noise
const int samples = 10;
unsigned int move_index = 1;  
//Variable  to store the speed, no. of satellites, direction
float gps_speed;
float no_of_satellites;
String satellite_orientation;
/////////////

void setup(){ // put your setup code here, to run once
   
  Serial.begin(115200);//Begin serial communication with Arduino and Arduino IDE (Serial Monitor)  
  gps_module.begin(GPSBaud); 
    lcd.begin();
  lcd.backlight();
  lcd.clear();
  lcd.setCursor(0,1);
  lcd.print("IOT MONITORING");
  lcd.setCursor(0,0);
  lcd.print("IOT BASED ATM SYS");  
  
pinMode(green, OUTPUT); 
pinMode(Redled, OUTPUT);  
pinMode(buzzer, OUTPUT);  
pinMode(GASPin, INPUT);
pinMode(Sound_pin,INPUT);

 Blynk.begin(auth, ssid, pass);

  WiFi.begin(ssid, pass);
  {
    delay(1000);
    Serial.print(".");
//    lcd.clear();
  }
  Serial.println("WiFi connected");
  
    Serial.println("Ready");    //Test the serial monitor
 
  timer.setInterval(300L, Accelerometer);
  timer.setInterval(100L, Gas);
  timer.setInterval(500L, displayInfo);
  timer.setInterval(100L, Sound);
  digitalWrite(green,HIGH);
  }

void loop(){
  while (gps_module.available() > 0) 
  {
    //displays information every time a new sentence is correctly encoded.
    if (gps.encode(gps_module.read()))
    displayInfo();
  }
     displayInfo();
  ///////////end gps
  Blynk.run();
  timer.run();

}// END OF VOID LOOP

void displayInfo()
{
  if (gps.location.isValid()) 
  {
    //Storing the Latitude. and Longitude
  float   latitude = (gps.location.lat());
   float  longitude = (gps.location.lng()); 
    
    //Send to Serial Monitor for Debugging
//    Serial.print("LAT:  ");
//    Serial.println(latitude, 6);  // float to x decimal places
//    Serial.print("LONG: ");
//    Serial.println(longitude, 6);
    
    Blynk.virtualWrite(V6, String(latitude, 6));   
    Blynk.virtualWrite(V7, String(longitude, 6));  
    myMap.location(move_index, latitude, longitude, "GPS_Location");
    
    //get speed
    gps_speed = gps.speed.kmph();
    Blynk.virtualWrite(V8, gps_speed);
       
    //get number of satellites
    no_of_satellites = gps.satellites.value();
    Blynk.virtualWrite(V9, no_of_satellites);

    // get the satellite orientation/direction
    satellite_orientation = TinyGPSPlus::cardinal(gps.course.value());
    Blynk.virtualWrite(V10, satellite_orientation);
  }
  
  Serial.println();
}


 void Sound(){
  int state=digitalRead(Sound_pin);
  int val=map(state,1,0,0,1);
  
//Serial.println(state);
 Blynk.virtualWrite(V13, val);
if(val==1){
Blynk.logEvent("sound","sound Detect !"); //Enter your Event Name
    digitalWrite(Redled, HIGH);
   for(int i=0; i<=18; i++){
       tone(buzzer,2222);
     delay(50);
    tone(buzzer,3333);
     delay(50);
    tone(buzzer,5555);
     delay(50);
   }
}
   else{
    noTone(buzzer);
   }
  

}// end of sound sensor

void Gas(){
   int sensorValue = digitalRead(GASPin); 
   int newSen=map(sensorValue ,1,0,0,1);
   Blynk.virtualWrite(V0, newSen);
  if(newSen==1){      
    Blynk.logEvent("gas","Gas Detect !"); //Enter your Event Name
    digitalWrite(Redled, HIGH);
      lcd.setCursor(0,1);
  lcd.print("Gas Detected!!");
  
   for(int i=0; i<=18; i++){
       tone(buzzer,2222);
     delay(50);
    tone(buzzer,3333);
     delay(50);
    tone(buzzer,5555);
     delay(50);
   }    
  }
  else{
    lcd.setCursor(0,1);
   lcd.print("  NO GAS..........!!");
    digitalWrite(Redled, LOW);
    noTone(buzzer);
  }
}// end of gas
 void Accelerometer(){
// Serial.println(sensorValue);
  //Read raw values
  int xRaw=0,yRaw=0,zRaw=0;
  for(int i=0;i<samples;i++)
  {
    xRaw+=analogRead(xPin);
    yRaw+=analogRead(yPin);
    zRaw+=analogRead(zPin);
  }
  xRaw/=samples;
  yRaw/=samples;
  zRaw/=samples;
  
    Blynk.virtualWrite(V2, xRaw);
  Blynk.virtualWrite(V3, yRaw);
    Blynk.virtualWrite(V4, zRaw);
    
  //--------------------------------------------------------------
//  Serial.print(xRaw);
//  Serial.print("\t");
//  Serial.print(yRaw);
//  Serial.print("\t");
//  Serial.print(zRaw);
//  Serial.println(); 

if((xRaw>=1900 && xRaw<=2120) && (yRaw>=1980 && yRaw<2150) && (zRaw>=2330 && zRaw<2480)){ 
    digitalWrite(Redled, LOW);
    noTone(buzzer);
}
//
 else{   
    Blynk.logEvent("acc","pushing !"); //Enter your Event Name
    digitalWrite(Redled, HIGH);
   for(int i=0; i<=18; i++){
       tone(buzzer,2222);
     delay(50);
    tone(buzzer,3333);
     delay(50);
    tone(buzzer,5555);
     delay(50);
   } 
 }
 }// END OF ACCELEROMETER
