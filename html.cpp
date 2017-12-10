#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include "html.h"
#include "util.h"
#include "fidget.h"

extern  Fidget            fidget;
extern  ESP8266WebServer* server;

const char HTTP_HEAD[] PROGMEM = \
"<!DOCTYPE htm><html lang='en'>"\
"<head>"\
"<meta name='viewport' content='width=device-width, initial-scale=1, user-scalable=no'/>"\
"<title>Yurij's Fidget Counter</title>";
const char HTTP_STYLE[] PROGMEM = \
"<style>"\
".q{float: right;width: 64px;text-align: right;}"\
".c{text-align: center;}"\
"input{width:95%;}"\
"div,input{padding:5px;font-size:1em;}"\
"body{margin:0; padding:0; text-align:center; font-family:verdana;} "\
"button{border:0;border-radius:0.3rem;background-color:#1fa3ec;color:#fff;line-height:2.4rem;font-size:1.2rem;width:100%;} "\
"table {border-collapse: collapse; width:100%}" \
"table, th, td {border: 2px solid blue;}" \
"th {padding: 5px; text-align: center;}" \
"td {padding: 3px; text-align: center;}" \
"</style>";
const char HTTP_HEAD_END[] PROGMEM        = \
"</head><body>"\
"<div style='text-align:center; display:inline-block; min-width:260px;'>";
const char HTTP_END[] PROGMEM             = \
"</div></body></html>";


void handleNotFound() {
  if (captivePortal()) { // If captive portal redirect instead of displaying the error page.
    return;
  }
  String message = "File Not Found\n\n";
  message += "URI: ";
  message += server->uri();
  message += "\nMethod: ";
  message += ( server->method() == HTTP_GET ) ? "GET" : "POST";
  message += "\nArguments: ";
  message += server->args();
  message += "\n";

  for ( uint8_t i = 0; i < server->args(); i++ ) {
    message += " " + server->argName ( i ) + ": " + server->arg ( i ) + "\n";
  }
  server->sendHeader("Cache-Control", "no-cache, no-store, must-revalidate");
  server->sendHeader("Pragma", "no-cache");
  server->sendHeader("Expires", "-1");
  server->send ( 404, "text/plain", message );
}

String spinRow(SpinData* spin, int i, bool last=false) {
  char duration_str[16];
  float duration = float(spin->duration()) / 1000.0;
  dtostrf(duration,6,1,duration_str);
  String start,end;
  if (last) {
      start = "<td><font color='red'>";
      end   = "</font></td>";
  } else {
      start = "<td>";
      end   = "</td>";
  }
  String row = "<tr>";
  row += start + String(i)              + end;
  row += start + String(spin->maxRPM()) + end;
  row += start + String(spin->rpm())    + end;
  row += start + String(spin->count())  + end;
  row += start + String(duration_str)   + end;
  row += "</tr>\r\n";
  return row;
}

/** Handle root or redirect to captive portal */
void handleRoot(void) {
//  Serial.println("handling root");
  if (captivePortal()) { // If caprive portal redirect instead of displaying the page.
    return;
  }
  String page = FPSTR(HTTP_HEAD);
  page += FPSTR(HTTP_STYLE);
  page += FPSTR(HTTP_HEAD_END);
  int refreshRate = fidget.spinning() ? 1 : 3;
  page += "<meta http-equiv='refresh' content='" + String(refreshRate) + "'3'>\r\n";
  page += "<h2 style='text-align:center'>Yurij's Fidget Counter</h2>\r\n";
  page += "<hr>\r\n";
  page += "<h2 style='text-align:center'>Max RPM = " + String(fidget.maxRPM()) + "</h2>\r\n";

  page += "<br><table style='text-align:center'> \r\n";
  page += "<tr><th>Spin</th><th>Max</th><th>Last</th><th>Turns</th><th>Time</th></tr>\r\n";
  // lets show the most resent first
  Spins* spins = fidget.spins();
  int  nspins = fidget.nspins();
  if (SpinData* spin = fidget.currSpin())
      page += spinRow(spin, nspins+1,true);

  for (int i=nspins; i>0; i--) {
    page += spinRow(spins[i-1],i);
  }

  page += "</table>\r\n";

  page += "<br><form action='/reset' method='get'><button>Clear</buttonr></form>";
  page += FPSTR(HTTP_END);
  server->send(200, "text/html", page);
//  Serial.println(page);
}

void handleReset() {
  Serial.println("reseting counters...");
  fidget.reset();
  server->sendHeader("Location",String("/"), true);
  server->send( 302, "text/plain", "");
}
