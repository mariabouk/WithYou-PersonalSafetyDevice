#include <RF22.h>
#include <RF22Router.h>

#define MY_ADDRESS 5        //receiver
#define NODE_ADDRESS_1 12   //transmitter
#define NODE_ADDRESS_2 13   //transmitter
RF22Router rf22(MY_ADDRESS);

char BPM[] = "BPM";
char distance[] = "Distance";
char latitude[] = "Latitude";
char longitude[] = "Longitude";
int indexI1 = 0;    //integer for 1st transmitter
int indexF1 = 0;    //float
int indexI2 = 0;    //integer for 2nd transmitter
int indexF2 = 0;    //float

void setup() {
  Serial.begin(9600);
  if (!rf22.init())
    Serial.println("RF22 init failed");
  if (!rf22.setFrequency(431.0))
    Serial.println("setFrequency Fail");
  rf22.setTxPower(RF22_TXPOW_20DBM);
  rf22.setModemConfig(RF22::GFSK_Rb125Fd125);
  rf22.addRouteTo(NODE_ADDRESS_1, NODE_ADDRESS_1);
  rf22.addRouteTo(NODE_ADDRESS_2, NODE_ADDRESS_2);
}


void loop() {
  uint8_t buf[RF22_ROUTER_MAX_MESSAGE_LEN];
  char incoming[RF22_ROUTER_MAX_MESSAGE_LEN];
  memset(buf, '\0', RF22_ROUTER_MAX_MESSAGE_LEN);
  memset(incoming, '\0', RF22_ROUTER_MAX_MESSAGE_LEN);
  uint8_t len = sizeof(buf);
  uint8_t from;
  int received_value = 0;
  long int received_value2 = 0;

  if (rf22.recvfromAck(buf, &len, &from))
  {
    if (from == 12 && indexI2!=1 && indexF2!=1) {
      if (indexI1 == 1) {
        buf[RF22_ROUTER_MAX_MESSAGE_LEN - 1] = '\0';
        memcpy(incoming, buf, RF22_ROUTER_MAX_MESSAGE_LEN);
        Serial.print("got request from : ");
        Serial.println(from, DEC);
        received_value = atoi((char*)incoming);
        Serial.println(received_value);
        indexI1 = 0;
        delay(1000);
      }
      if (indexF1 == 1) {
        buf[RF22_ROUTER_MAX_MESSAGE_LEN - 1] = '\0';
        memcpy(incoming, buf, RF22_ROUTER_MAX_MESSAGE_LEN);
        Serial.print("got request from : ");
        Serial.println(from, DEC);
        received_value2 = atol((char*)incoming);
        Serial.println(received_value2 / 100.00);
        indexF1 = 0;
        delay(1000);
      }

      if (strcmp((char*)buf, BPM) == 0) {               //BPM string message arrived
        Serial.print("Data of BPM received from: ");
        Serial.println(from, DEC);
        indexI1 = 1;
      }                                                   //Distance string message arrived
      if (strcmp((char*)buf, distance) == 0) {
        Serial.print("Data of Distance received from: ");
        Serial.println(from, DEC);
        Serial.println("Congratulations! You've covered 1 Km.");
        indexF1 = 1;
      }                                                   //Latitude string message arrived
      if (strcmp((char*)buf, latitude) == 0) {
        Serial.println("Person is in DANGER!");
        Serial.print("Latitude received from: ");
        Serial.println(from, DEC);
        indexF1 = 1;
      }                                                   //Longitude string message arrived
      if (strcmp((char*)buf, longitude) == 0) {
        Serial.print("Longitude received from: ");
        Serial.println(from, DEC);
        indexF1 = 1;
      }
    }
    if (from == 13 && indexI1!=1 && indexF1!=1) {
      if (indexI2 == 1) {
        buf[RF22_ROUTER_MAX_MESSAGE_LEN - 1] = '\0';
        memcpy(incoming, buf, RF22_ROUTER_MAX_MESSAGE_LEN);
        Serial.print("got request from : ");
        Serial.println(from, DEC);
        received_value = atoi((char*)incoming);
        Serial.println(received_value);
        indexI2 = 0;
        delay(1000);
      }
      if (indexF2 == 1) {
        buf[RF22_ROUTER_MAX_MESSAGE_LEN - 1] = '\0';
        memcpy(incoming, buf, RF22_ROUTER_MAX_MESSAGE_LEN);
        Serial.print("got request from : ");
        Serial.println(from, DEC);
        received_value2 = atol((char*)incoming);
        Serial.println(received_value2 / 100.00);
        indexF2 = 0;
        delay(1000);
      }

      if (strcmp((char*)buf, BPM) == 0) {               //BPM string message arrived
        Serial.print("Data of BPM received from: ");
        Serial.println(from, DEC);
        indexI2 = 1;
      }                                                   //Distance string message arrived
      if (strcmp((char*)buf, distance) == 0) {
        Serial.print("Data of Distance received from: ");
        Serial.println(from, DEC);
        Serial.println("Congratulations! You've covered 1 Km.");
        indexF2 = 1;
      }                                                   //Latitude string message arrived
      if (strcmp((char*)buf, latitude) == 0) {
        Serial.println("Person is in DANGER!");
        Serial.print("Latitude received from: ");
        Serial.println(from, DEC);
        indexF2 = 1;
      }                                                   //Longitude string message arrived
      if (strcmp((char*)buf, longitude) == 0) {
        Serial.print("Longitude received from: ");
        Serial.println(from, DEC);
        indexF2 = 1;
      }
    }
  }
}
