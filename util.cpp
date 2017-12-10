#include "Arduino.h"
#include <ESP8266WebServer.h>
#include "util.h"

extern ESP8266WebServer *server;

bool isIp(const String& str) {
  for (int i = 0; i < str.length(); i++) {
    int c = str.charAt(i);
    if (c != '.' && (c < '0' || c > '9')) {
      return false;
    }
  }
  return true;
}

String toStringIp(IPAddress ip) {
  String res = "";
  for (int i = 0; i < 3; i++) {
    res += String((ip >> (8 * i)) & 0xFF) + ".";
  }
  res += String(((ip >> 8 * 3)) & 0xFF);
  return res;
}

bool captivePortal(void) {
  if (isIp(server->hostHeader()))
      return false;

  server->sendHeader("Location", String("http://") + 
          toStringIp( server->client().localIP() ), true);
  // Empty content inhibits Content-length header so we have to close the socket ourselves.
  server->send ( 302, "text/plain", ""); 
  // Stop is needed because we sent no content length
  server->client().stop();
  return true;
}

