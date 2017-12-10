#include "Arduino.h"
#include <DNSServer.h>
#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>
#include "wifi.h"
#include "html.h"

const byte          DNS_PORT = 53;
DNSServer*          dnsServer;
ESP8266WebServer*   server;

void Wifi::start(void) {
    Serial.println("startWifi in AP mode");
    WiFi.mode(WIFI_AP);
    WiFi.softAP(_ssid, _password);
    Serial.printf("Access point '%s' started\n", _ssid);
    Serial.printf("IP address: %s\n",WiFi.softAPIP().toString().c_str());

    // start the web machine
    dnsServer = new DNSServer();
    server    = new ESP8266WebServer(80);
    Serial.println("created dnsServer & webServer");

    // Setup the DNS server redirecting all the domains to the apIP
    dnsServer->setErrorReplyCode(DNSReplyCode::NoError);
    dnsServer->start(DNS_PORT, "*", WiFi.softAPIP());
    Serial.println("created captive portal...");

    server->on("/",      handleRoot  );
    server->on("/reset", handleReset );
    server->onNotFound (handleNotFound);
    server->begin();
    Serial.println("web services started...");
    
    mdnsStart();
}

void  Wifi::process() {
  if (WiFi.status() == WL_CONNECTED) {
    /* Just connected to WLAN */
    Serial.println ( "" );
    Serial.print ( "Connected to " );
    Serial.println ( _ssid );
    Serial.print ( "IP address: " );
    Serial.println ( WiFi.localIP() );

    // Setup eDNS responder
    mdnsStart();
  }
  dnsServer->processNextRequest();
  server->handleClient();
}

void Wifi::mdnsStart(void) {
  return;
  if (!MDNS.begin(_ssid)) {
    Serial.println("Error setting up MDNS responder!");
  } else {
    Serial.println("mDNS responder started");
    // Add service to MDNS-SD
    MDNS.addService("http", "tcp", 80);
  }
}

