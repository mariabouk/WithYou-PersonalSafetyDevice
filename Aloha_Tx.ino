#include <SPI.h>
#include <RF22.h>
#include <RF22Router.h>
#define MY_ADDRESS 12
#define DESTINATION_ADDRESS 5
RF22Router rf22(MY_ADDRESS);

#define USE_ARDUINO_INTERRUPTS true
#include <PulseSensorPlayground.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#define pi 3.1415

long randNumber;
boolean successful_packet = false;
int max_delay = 3000;

//-------------------------------------Pulse sensor------------
int myBPM = 10;
const int PulseWire = 0;
const int LED13 = 13;
int Threshold = 550;

PulseSensorPlayground pulseSensor;
//---------------------------------------------end of pulse sensor-----------

//--------------------------------------------GPS variables---------
float minA = 4037.39;
float maxA = 4037.55;
float minO = 2256.58;
float maxO = 2257.31;
float lat2 = 0, long2 = 0; //variables to count distance
float lat1 = minA, long1 = minO;    //set start of route
float distance = 0;
float d = 0;
long int d_send;
long int a_send; //latitude
long int o_send; //longitude to send
float a = 0;
float o = 0;
//-------------------------------------------------GPS end---------------

//------------------------------------------------3 Ultrasonic distance sensors---------
const int trigPin_2 = 7;
const int echoPin_2 = 6;
long duration_2;
int distance_2;

const int trigPin_3 = 10;
const int echoPin_3 = 9;
long duration_3;
int distance_3;

const int LED = 8;
const int button = 3;
const int buzzer = 4;
unsigned long currentTime = 0;
unsigned long startTime = 0;
unsigned long totalTime = 0;
//------------------------------------------------end of variables of Ultrasonic sensors----


///--------------------------------------------------------setup--------------------
void setup() {
  Serial.begin(9600);
  if (!rf22.init())
    Serial.println("RF22 init failed");
  if (!rf22.setFrequency(431.0))
    Serial.println("setFrequency Fail");
  rf22.setTxPower(RF22_TXPOW_20DBM);
  rf22.setModemConfig(RF22::GFSK_Rb125Fd125);

  rf22.addRouteTo(DESTINATION_ADDRESS, DESTINATION_ADDRESS);
  myBPM = analogRead(A0);
  randomSeed(myBPM);

  //--------------------------------------------------Pulse sensor setup-------------
  pulseSensor.analogInput(PulseWire);
  pulseSensor.blinkOnPulse(LED13);
  pulseSensor.setThreshold(Threshold);

  if (pulseSensor.begin()) {
    Serial.println("We created a pulseSensor Object !");
  }
  //--------------------------------------------------end of pulse sensor----------------

  //--------------------------------------------Ultrasonic distance setup---------------
  pinMode(trigPin_2, OUTPUT);
  pinMode(echoPin_2, INPUT);

  pinMode(trigPin_3, OUTPUT);
  pinMode(echoPin_3, INPUT);
  pinMode(LED, OUTPUT);
  pinMode(button, INPUT_PULLUP);    //pullup: button=1 when switch off, button=0 when switch on
  digitalWrite(LED, LOW);
  digitalWrite(button, HIGH);
  pinMode(buzzer, OUTPUT);
  noTone(buzzer);
  //------------------------------------------end of Ultrasonic distance setup--------
}

//-------------------------------------------------------------loop----------------

void loop() {
  startTime = millis();
  //------------------------------------------ Loop of pulse sensor Measurement--------------
  int myBPM = pulseSensor.getBeatsPerMinute();
  if (totalTime > 10000) {
    if (pulseSensor.sawStartOfBeat()) {            // Constantly test to see if "a beat happened".
      Serial.print("Your heartbeat is:");
      Serial.println(myBPM);
      //--------------------------------------------Start of Message of Pulse transmission-----
      char data_read_[RF22_ROUTER_MAX_MESSAGE_LEN];
      uint8_t data_send_[RF22_ROUTER_MAX_MESSAGE_LEN];
      memset(data_read_, '\0', RF22_ROUTER_MAX_MESSAGE_LEN);
      memset(data_send_, '\0', RF22_ROUTER_MAX_MESSAGE_LEN);
      sprintf(data_read_, "%s", "BPM");
      data_read_[RF22_ROUTER_MAX_MESSAGE_LEN - 1] = '\0';
      memcpy(data_send_, data_read_, RF22_ROUTER_MAX_MESSAGE_LEN);
      successful_packet = false;
      while (!successful_packet)
      {

        if (rf22.sendtoWait(data_send_, sizeof(data_send_), DESTINATION_ADDRESS) != RF22_ROUTER_ERROR_NONE)
        {
          Serial.println("sendtoWait failed");
          randNumber = random(200, max_delay);
          Serial.println(randNumber);
          delay(randNumber);
        }
        else
        {
          successful_packet = true;
        }
      }
      //-------------------------------------end of Message of Pulse transmission-------

      //------------------------------------------Start of Pulse Measurement transmission-------
      char data_read[RF22_ROUTER_MAX_MESSAGE_LEN];
      uint8_t data_send[RF22_ROUTER_MAX_MESSAGE_LEN];
      memset(data_read, '\0', RF22_ROUTER_MAX_MESSAGE_LEN);
      memset(data_send, '\0', RF22_ROUTER_MAX_MESSAGE_LEN);
      sprintf(data_read, "%d", myBPM);
      myBPM = atoi(data_read);
      data_read[RF22_ROUTER_MAX_MESSAGE_LEN - 1] = '\0';
      memcpy(data_send, data_read, RF22_ROUTER_MAX_MESSAGE_LEN);
      successful_packet = false;
      while (!successful_packet)
      {
        if (rf22.sendtoWait(data_send, sizeof(data_send), DESTINATION_ADDRESS) != RF22_ROUTER_ERROR_NONE)
        {
          Serial.println("sendtoWait failed");
          randNumber = random(200, max_delay);
          delay(randNumber);
        }
        else
        {
          successful_packet = true;
        }
      }
      //-------------------------------------------end Pulse sensor transmission------
    }
    totalTime = 0;
  }
  //--------------------------------------------end of pulse sensor-----------

  //-------------------------------------------GPS Measurement loop-----------------------------
  lat2 = randomFloat(minA, maxA);
  long2 = randomFloat(minO, maxO);

  d = distance_function((lat1 / 100.0), (long1 / 100.0), (lat2 / 100.0), (long2 / 100.0));
  distance = distance + d;    //total distance
  d_send = (long int)(distance * 100);
  long1 = long2;      //set current values as previous values of position, to use in next loop
  lat1 = lat2;
  //------------------------------------------------end GPS loop----------------

  //-----------------------------------------------GPS transmission--------------
  if (distance > 100) {
    Serial.print("Distance= ");
    Serial.println(distance);
    //----------------------------------------------GPS MESSAGE TRANSMISSION--------
    char data_read_1[RF22_ROUTER_MAX_MESSAGE_LEN];
    uint8_t data_send_1[RF22_ROUTER_MAX_MESSAGE_LEN];
    memset(data_read_1, '\0', RF22_ROUTER_MAX_MESSAGE_LEN);
    memset(data_send_1, '\0', RF22_ROUTER_MAX_MESSAGE_LEN);
    sprintf(data_read_1, "%s", "Distance");
    data_read_1[RF22_ROUTER_MAX_MESSAGE_LEN - 1] = '\0';
    memcpy(data_send_1, data_read_1, RF22_ROUTER_MAX_MESSAGE_LEN);
    successful_packet = false;
    while (!successful_packet)
    {
      if (rf22.sendtoWait(data_send_1, sizeof(data_send_1), DESTINATION_ADDRESS) != RF22_ROUTER_ERROR_NONE)
      {
        Serial.println("sendtoWait failed");
        randNumber = random(200, max_delay);
        delay(randNumber);
      }
      else
      {
        successful_packet = true;
      }
    }
    //---------------------------------------------END OF GPS MESSAGE TRANSMISSION---

    //-------------------------------------Transmission of Distance Value----------------
    char data_read1[RF22_ROUTER_MAX_MESSAGE_LEN];
    uint8_t data_send1[RF22_ROUTER_MAX_MESSAGE_LEN];
    memset(data_read1, '\0', RF22_ROUTER_MAX_MESSAGE_LEN);
    memset(data_send1, '\0', RF22_ROUTER_MAX_MESSAGE_LEN);
    sprintf(data_read1, "%ld", d_send);
    data_read1[RF22_ROUTER_MAX_MESSAGE_LEN - 1] = '\0';
    memcpy(data_send1, data_read1, RF22_ROUTER_MAX_MESSAGE_LEN);
    successful_packet = false;
    while (!successful_packet)
    {
      if (rf22.sendtoWait(data_send1, sizeof(data_send1), DESTINATION_ADDRESS) != RF22_ROUTER_ERROR_NONE)
      {
        Serial.println("sendtoWait failed");
        randNumber = random(200, max_delay);
        delay(randNumber);
      }
      else
      {
        successful_packet = true;
      }
    }
    distance = 0; //if user reaches 1km , distance=0;
  }
  //----------------------------------end of Distance transmission------------------------------

  //-----------------------------------------Ultrasonic distance Measurement loop----------------
  digitalWrite(trigPin_2, LOW);
  delayMicroseconds(5);
  digitalWrite(trigPin_2, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin_2, LOW);
  duration_2 = pulseIn(echoPin_2, HIGH);
  distance_2 = duration_2 * 0.034 / 2;

  //2nd sensor
  digitalWrite(trigPin_3, LOW);
  delayMicroseconds(5);
  digitalWrite(trigPin_3, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin_3, LOW);
  duration_3 = pulseIn(echoPin_3, HIGH);
  distance_3 = duration_3 * 0.034 / 2;
  //----------------------------------------------end of Ultrasonic distance loop-----------

  //---------------------------------------------Ultrasonic distance transmission-----
  if (distance_2 < 10 || distance_3 < 10) {
    digitalWrite(LED, HIGH); //LED ON to warn the user
    Serial.println("Attention! Someone near you.");
  }

  if (digitalRead(button) == LOW) {
    tone(buzzer,1000);
    delay(1000);
   noTone(buzzer);
    //------------------------------------------------ULTRASONIC MESSAGE TRANSMISSION--------
    char data_read_2[RF22_ROUTER_MAX_MESSAGE_LEN];
    uint8_t data_send_2[RF22_ROUTER_MAX_MESSAGE_LEN];
    memset(data_read_2, '\0', RF22_ROUTER_MAX_MESSAGE_LEN);
    memset(data_send_2, '\0', RF22_ROUTER_MAX_MESSAGE_LEN);
    sprintf(data_read_2, "%s", "Latitude");
    data_read_2[RF22_ROUTER_MAX_MESSAGE_LEN - 1] = '\0';
    memcpy(data_send_2, data_read_2, RF22_ROUTER_MAX_MESSAGE_LEN);
    successful_packet = false;
    while (!successful_packet)
    {
      if (rf22.sendtoWait(data_send_2, sizeof(data_send_2), DESTINATION_ADDRESS) != RF22_ROUTER_ERROR_NONE)
      {
        Serial.println("sendtoWait failed");
        randNumber = random(200, max_delay);
        Serial.println(randNumber);
        delay(randNumber);
      }
      else
      {
        successful_packet = true;
      }
    }
    //---------------------------------------------------END OF ULTRASONIC MESSAGE TRANSMISSION---
    a = randomFloat(minA, maxA);      //Calculate current latitude, when in danger
    a_send = (long int)(a*100);
    Serial.print("Lat = ");
    Serial.println(a);
    char data_read2[RF22_ROUTER_MAX_MESSAGE_LEN];
    uint8_t data_send2[RF22_ROUTER_MAX_MESSAGE_LEN];
    memset(data_read2, '\0', RF22_ROUTER_MAX_MESSAGE_LEN);
    memset(data_send2, '\0', RF22_ROUTER_MAX_MESSAGE_LEN);
    sprintf(data_read2, "%ld", a_send);
    data_read2[RF22_ROUTER_MAX_MESSAGE_LEN - 1] = '\0';
    memcpy(data_send2, data_read2, RF22_ROUTER_MAX_MESSAGE_LEN);
    successful_packet = false;
    while (!successful_packet)
    {
      if (rf22.sendtoWait(data_send2, sizeof(data_send2), DESTINATION_ADDRESS) != RF22_ROUTER_ERROR_NONE)
      {
        Serial.println("sendtoWait failed");
        randNumber = random(200, max_delay);
        delay(randNumber);
      }
      else
      {
        successful_packet = true;
      }
    }
    //---------------------------------------------TRANSMISSION OF LONGITUDE MESSAGE------------------------------
    char data_read_3[RF22_ROUTER_MAX_MESSAGE_LEN];
    uint8_t data_send_3[RF22_ROUTER_MAX_MESSAGE_LEN];
    memset(data_read_3, '\0', RF22_ROUTER_MAX_MESSAGE_LEN);
    memset(data_send_3, '\0', RF22_ROUTER_MAX_MESSAGE_LEN);
    sprintf(data_read_3, "%s", "Longitude");
    data_read_3[RF22_ROUTER_MAX_MESSAGE_LEN - 1] = '\0';
    memcpy(data_send_3, data_read_3, RF22_ROUTER_MAX_MESSAGE_LEN);
    successful_packet = false;
    while (!successful_packet)
    {
      if (rf22.sendtoWait(data_send_3, sizeof(data_send_3), DESTINATION_ADDRESS) != RF22_ROUTER_ERROR_NONE)
      {
        Serial.println("sendtoWait failed");
        randNumber = random(200, max_delay);
        delay(randNumber);
      }
      else
      {
        successful_packet = true;
      }
    }
    //---------------------------------------------END OF ULTRASONIC LONGITUDE MESSAGE TRANSMISSION---
    o = randomFloat(minO, maxO);    //calculate longitude
    o_send = (long int)(o*100);
    Serial.print("Long = ");
    Serial.println(o);
    char data_read3[RF22_ROUTER_MAX_MESSAGE_LEN];
    uint8_t data_send3[RF22_ROUTER_MAX_MESSAGE_LEN];
    memset(data_read3, '\0', RF22_ROUTER_MAX_MESSAGE_LEN);
    memset(data_send3, '\0', RF22_ROUTER_MAX_MESSAGE_LEN);
    sprintf(data_read3, "%ld", o_send);
    data_read3[RF22_ROUTER_MAX_MESSAGE_LEN - 1] = '\0';
    memcpy(data_send3, data_read3, RF22_ROUTER_MAX_MESSAGE_LEN);
    successful_packet = false;
    while (!successful_packet)
    {
      if (rf22.sendtoWait(data_send3, sizeof(data_send3), DESTINATION_ADDRESS) != RF22_ROUTER_ERROR_NONE)
      {
        Serial.println("sendtoWait failed");
        randNumber = random(200, max_delay);
        delay(randNumber);
      }
      else
      {
        successful_packet = true;
      }
    }
    //---------------------------------------------------END OF LONGITUDE TRANSMISSION---------------------
  }
  if(digitalRead(LED)==HIGH){
    digitalWrite(LED, LOW);
  }
  //----------------------------------------------end of Ultrasonic transmission-------
  //delay(1000); //delays next sensor measurement
  currentTime = millis();
  totalTime = totalTime + (currentTime - startTime);
}//end of loop


//---------------------------Random function, that returns random number, between minf and maxf
float randomFloat(float minf, float maxf)
{
  return minf + random(1UL << 31) * (maxf - minf) / (1UL << 31);  // use 1ULL<<63 for max double values)
}

//----------------------------Function to calculate distance with latitude and longitude
float deg2rad(float);
float rad2deg(float);

float distance_function(float lat1, float lon1, float lat2, float lon2) {
  float theta, dist;
  if ((lat1 == lat2) && (lon1 == lon2)) {
    return 0;
  }
  else {
    theta = lon1 - lon2;
    dist = sin(deg2rad(lat1)) * sin(deg2rad(lat2)) + cos(deg2rad(lat1)) * cos(deg2rad(lat2)) * cos(deg2rad(theta));
    dist = acos(dist);
    dist = rad2deg(dist);
    dist = dist * 60 * 1.1515;
  }
  return (dist);
}

//-------------------------This function converts decimal degrees to radians
float deg2rad(float deg) {
  return (deg * pi / 180);
}
//------------------------This function converts radians to decimal degrees
float rad2deg(float rad) {
  return (rad * 180 / pi);
}
