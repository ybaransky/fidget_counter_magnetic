#include "fidget.h"
#include "html.h"
#include "wifi.h"
#include "gui.h"


const char*         ssid = "fidget";
const char*         password = "leslie1a";

Wifi                wifi(ssid, password);
Fidget              fidget;
volatile uint32_t   interruptTime  = 0;
volatile uint32_t   interruptCount = 0;

void handleInterrupt(void) {
  interruptCount++;
  interruptTime = millis();
}

void sensorStart(void) {
  const int SENSOR = D3;            // hall sensor
  pinMode (BUILTIN_LED, OUTPUT) ;    // define LED as output interface
  pinMode(SENSOR, INPUT_PULLUP);
  attachInterrupt( digitalPinToInterrupt(SENSOR), handleInterrupt, FALLING);
  Serial.println("finished setting up magnetic sensor");
}

void setup() {  
  Serial.begin(115200);
  Serial.println();
  Serial.println("Starting...");
  Serial.setDebugOutput(true);  // WiFi diags

  fidget.init(3);
  wifi.start();
  sensorStart();
  guiStart();
}

void loop() {
  // put your main code here, to run repeatedly:
  static uint32_t   prevTime = 0;
  uint32_t          currTime = millis();
  bool              changed;

  wifi.process();

  changed = fidget.process(interruptCount, interruptTime, currTime );
  interruptCount = 0;

  if (changed || (currTime - prevTime > 500)) {
    prevTime = currTime;
    guiClear();
    guiDrawTitle();
    if (fidget.rpm())
        guiDrawRPM( fidget.rpm() );
    else
        guiDrawMaxRPM(fidget.maxRPM());
    guiRender();
  }
  delay(250);
}
