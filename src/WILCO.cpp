//
// 
// 

#include "WILCO.h"
#include <WiFi.h>
#include <DNSServer.h>
#include <WebServer.h>


#define DNS_PORT  53
#define DNS_TTL   300

#define HTTP_PORT 80


DNSServer dnsServer;
WebServer webServer(HTTP_PORT);


void WILCO::begin() {
	_namespace.begin();

}

void WILCO::loop() {
	dnsServer.processNextRequest();
	webServer.handleClient();
}

void WILCO::_startAPMode() {
	Serial.print("Starting AP Mode...");
	WiFi.mode(WIFI_MODE_AP);
	if (!WiFi.softAP(AP_SSID, AP_PASSWORD)) {
		Serial.println(" failed.");
		return;
	}
	Serial.println(" done.");

	_startDNSServer();
	_startWebServer();
}

void WILCO::_startClientMode() {
	Serial.print("Starting Client Mode...");
	WiFi.mode(WIFI_MODE_STA);
	long timeout = millis() + (AP_TIMEOUT * 1000);
	while (!WiFi.isConnected()) {
		if (millis() >= timeout) {
			Serial.println(" timed out.");
			_startAPMode();
			return;
		}
	}
	Serial.println(" done.");

	_startWebServer();
}

void WILCO::_startDNSServer() {
	Serial.print("Starting DNS Server...");
	IPAddress ip = WiFi.localIP();
	dnsServer.setTTL(DNS_TTL);
	if (!dnsServer.start(DNS_PORT, "*", ip)) {
		Serial.println(" failed.");
		return;
	}
	Serial.println(" done.");
}

void WILCO::_startWebServer() {
	String fields = "";
	fields += _generateFormField(&_settingWiFiSSID);
	fields += _generateFormField(&_settingWiFiPassword, true);

	String html =
		"<!DOCTYPE html>\n"
		"<html>\n"
		"  <head>\n"
		"    <meta charset=\"utf-8\">\n"
		"    <meta name=\"viewport\" content=\"width=device-width, initial-scale:1.0\">\n"
		"    <title>WILCO Settings</title>\n"
		"    <style>\n"
		"      * {\n"
		"        box-sizing: border-box;\n"
		"        font-family: sans-serif;\n"
		"      }\n"
		"      form {\n"
		"        width: 400px;\n"
		"      }\n"
		"      input {\n"
		"        width: 100%;\n"
		"        padding: 12px 20px;\n"
		"        margin: 8px 0;\n"
		"      }\n"
		"    </style>\n"
		"  </head>\n"
		"  <body>\n"
		"    <form method=\"post\">\n"
		"      <h1>WILCO Settings</h1>"
		+ fields +
		"    </form>\n"
		"  </body>\n"
		"</html>";

}

String WILCO::_generateFormField(NVSStringEntry *setting, bool isPassword = false) {
	String type = isPassword ? "password" : "text";
	return String(
		"      <label for=\"" + setting->name() + "\">" + setting->name() + "</label>"
		"      <input type=\"" + type + "\" id=\"" + setting->name() + "\" name=\"" + setting->name() + "\" value=\"" + setting->load() + "\" />"
	);
}

String WILCO::_generateFormField(NVSIntegerEntry *setting) {
	return String(
		"      <label for=\"" + setting->name() + "\">" + setting->name() + "</label>"
		"      <input type=\"text\" id=\"" + setting->name() + "\" name=\"" + setting->name() + "\" value=\"" + setting->load() + "\" />"
	);
}
