// Enable debug prints
//#define MY_DEBUG

// Enable and select radio type attached 
//#define MY_RADIO_NRF24
//#define MY_RADIO_RFM69
//#define MY_RS485
#define MY_GATEWAY_SERIAL

#include <MySensors.h>  
#include <SimpleDHT.h>

// Define DHT sensor
static const byte DHT_DATA_PIN = 14;
SimpleDHT11 gDHTSensor;

// Sensor global variables (last known values)
byte gTemperature = 0;
byte gHumidity = 0;

// RGB pins setup
static const byte PIN_LED_R = 3;
static const byte PIN_LED_G = 5;
static const byte PIN_LED_B = 6;

// Sleep time between sensor updates (in milliseconds)
// Must be >1000ms for DHT22 and >2000ms for DHT11
static const uint64_t UPDATE_INTERVAL = 6000;

// define sensor IDs
static const byte CHILD_ID_HUM  = 0;
static const byte CHILD_ID_TEMP = 1;
static const byte CHILD_ID_RGB  = 2;

// metric values?
bool gMetric = true;

// message definitions
MyMessage msgHum(CHILD_ID_HUM, V_HUM);
MyMessage msgTemp(CHILD_ID_TEMP, V_TEMP);

void before()
{
  pinMode(PIN_LED_R, OUTPUT);
  pinMode(PIN_LED_G, OUTPUT);
  pinMode(PIN_LED_B, OUTPUT);
  digitalWrite(PIN_LED_R, 1);
  digitalWrite(PIN_LED_G, 1);
  digitalWrite(PIN_LED_B, 1);
}

void presentation()  
{ 
  // Send the sketch version information to the gateway
  sendSketchInfo("ArduinoMainHUB", "0.1");

  // Register all sensors to gw (they will be created as child devices)
  present(CHILD_ID_HUM, S_HUM);
  present(CHILD_ID_TEMP, S_TEMP);
  present(CHILD_ID_RGB, S_RGB_LIGHT);

  // check if we use metric values
  gMetric = getControllerConfig().isMetric;
}


void setup()
{


}

void receive(const MyMessage &message)
{
    // We only expect one type of message from controller. But we better check anyway.
    if (message.type==V_RGB) 
    {
      String hexstring = message.getString();
      long number = (long) strtol( &hexstring[0], NULL, 16);
      long RGB_values[3] = {0, 0, 0};
      RGB_values[0] = number >> 16;
      RGB_values[1] = number >> 8 & 0xFF;
      RGB_values[2] = number & 0xFF;
      analogWrite(PIN_LED_R, int(~RGB_values[0]));
      analogWrite(PIN_LED_G, int(~RGB_values[1]));
      analogWrite(PIN_LED_B, int(~RGB_values[2]));
    }
}

void loop()      
{  
  // Get temperature from DHT library
  byte temp = 0;
  byte hum = 0;
  if (gDHTSensor.read(DHT_DATA_PIN, &temp, &hum, NULL) == SimpleDHTErrSuccess)
  {
    // check if data needs to be updated
    if (temp!=gTemperature) send(msgTemp.set(gTemperature=temp, 1)); 
    if (hum!=gHumidity)     send(msgHum.set(gHumidity=hum, 1));
  }
  wait(UPDATE_INTERVAL); 
}
