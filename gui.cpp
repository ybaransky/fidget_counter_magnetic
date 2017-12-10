#include <Wire.h>
#include "SSD1306.h"

SSD1306 display(0x3c, D2, D1);

const char* title = "Yurij's Counter";

void  guiClear(void) {
  display.clear();
}
void  guiRender(void) {
  display.display();
}

void guiDrawTitle(void) {
    // Font Demo1
    // create more fonts at http://oleddisplay.squix.ch/
    display.setTextAlignment(TEXT_ALIGN_CENTER);
    display.setFont(ArialMT_Plain_16);
    display.drawString(64, 0, title);
}

void guiDrawRPM(int rpm) {
    char rpm_str[32];
    sprintf(rpm_str, "%d RPM", rpm);
    display.setFont(ArialMT_Plain_24);  //    display.setFont(ArialMT_Plain_24);
    display.setTextAlignment(TEXT_ALIGN_CENTER); 
    display.drawString(64, 30, rpm_str); // 18,42 for 2 lines to display
}

void guiDrawMaxRPM(int maxrpm) { 
    if (maxrpm) {
      display.setFont(ArialMT_Plain_16);
      display.drawString(64, 20,"Previous Max");

      char max_str[32];
      sprintf(max_str,"%d RPM",maxrpm);
      display.setFont(ArialMT_Plain_24);
      display.drawString(64, 42, max_str);
    }
}

void guiStart(void)  {
  // Initialising the UI will init the display too.
  display.init();
  display.flipScreenVertically();
  display.clear();
  guiDrawTitle();
  display.display();
  Serial.println("finished setting up display");
}
