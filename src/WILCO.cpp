//
// 
// 

#include "WILCO.h"
#include <WiFi.h>
#include <WiFiClientSecure.h>
#include <DNSServer.h>
#include <WebServer.h>
#include <PubSubClient.h>

#define DNS_PORT  53
#define DNS_TTL   300

#define HTTP_PORT 80


DNSServer dnsServer;
WebServer webServer(HTTP_PORT);
WiFiClientSecure wifiClient;
PubSubClient mqttClient(wifiClient);

void WILCO::begin() {
	_namespace.begin();
	if ((!_settingWiFiSSID.exists()) || (!_settingWiFiPassword.exists())) {
		_startAPMode();
		return;
	}
	_startClientMode();
}

void WILCO::loop() {
	dnsServer.processNextRequest();
	webServer.handleClient();
	mqttClient.loop();
}

void WILCO::_startAPMode() {
	Serial.print("Starting AP Mode...");
	WiFi.mode(WIFI_MODE_AP);
	if (!WiFi.softAP(AP_SSID, AP_PASSWORD)) {
		Serial.println(" failed.");
		return;
	}
	Serial.println(" done.");

	_ip = WiFi.softAPIP();
	Serial.print("IP: ");
	Serial.println(_ip);

	_startDNSServer();
	_startWebServer();
}

void WILCO::_startClientMode() {
	Serial.print("Starting Client Mode...");
	WiFi.mode(WIFI_MODE_STA);
	WiFi.begin(
		_settingWiFiSSID.load().c_str(),
		_settingWiFiPassword.load().c_str()
	);
	long timeout = millis() + (AP_TIMEOUT * 1000);
	while (!WiFi.isConnected()) {
		if (millis() >= timeout) {
			Serial.println(" timed out.");
			_startAPMode();
			return;
		}
	}
	Serial.println(" done.");

	_ip = WiFi.localIP();
	Serial.print("IP: ");
	Serial.println(_ip);

	_startWebServer();
	_startMQTTClient();
}

void WILCO::_startDNSServer() {
	Serial.print("Starting DNS Server...");
	dnsServer.setTTL(DNS_TTL);
	if (!dnsServer.start(DNS_PORT, "*", _ip)) {
		Serial.println(" failed.");
		return;
	}
	Serial.println(" done.");
}

void WILCO::_startWebServer() {

	Serial.print("Starting Web Server...");

	webServer.onNotFound([this]() {
		if (webServer.method() == HTTP_GET) _serveSettingsForm();
		if (webServer.method() == HTTP_POST) _handleSettingsForm();
	});
	webServer.begin();

	Serial.println(" done.");

}

void WILCO::_startMQTTClient() {
	Serial.print("Connecting MQTT Client...");

	mqttClient.setCallback([this](char * topic, byte * payload, unsigned int length) {
		char * data = (char*)payload;
		data[length] = '\0';

		String topicString(topic);
		String payloadString(data);

		Serial.printf("Topic: %s\n", topicString.c_str());
		Serial.printf("Data:\n%s\n", payloadString.c_str());
	});

	String serverString = _settingMQTTServer.load();
	char * server = new char[serverString.length() + 1];
	for (int i = 0; i < serverString.length(); i++) {
		server[i] = serverString.charAt(i);
	}
	server[serverString.length()] = '\0';

	mqttClient.setServer(
		server,
		_settingMQTTPort.load()
	);
	if (!mqttClient.connect(
		_mqttId().c_str(),
		_settingMQTTUsername.load().c_str(),
		_settingMQTTPassword.load().c_str()
	)) {
		Serial.print(" failed (");
		Serial.print(mqttClient.state());
		Serial.println(").");
		return;
	}

	Serial.println(" done.");
	mqttClient.subscribe("test");

}

String WILCO::_mqttId() {
	String mac = WiFi.macAddress();
	mac.replace(":", "");
	return "ESP_" + mac.substring(6);
}

void WILCO::_serveSettingsForm() {

	String fields = "";
	fields += _generateFormField(&_settingWiFiSSID);
	fields += _generateFormField(&_settingWiFiPassword, true);
	fields += _generateFormField(&_settingMQTTServer);
	fields += _generateFormField(&_settingMQTTPort);
	fields += _generateFormField(&_settingMQTTUsername);
	fields += _generateFormField(&_settingMQTTPassword, true);

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
		"    <input type=\"submit\" value=\"Save\" />"
		"    </form>\n"
		"  </body>\n"
		"</html>";

	webServer.send(200, "text/html", html);

}

void WILCO::_handleSettingsForm() {
	for (int argNum = 0; argNum < webServer.args(); argNum++) {
		String key = webServer.argName(argNum);
		String value = webServer.arg(argNum);

		if (key == NVS_KEY_WIFISSID) _settingWiFiSSID.save(value);
		if (key == NVS_KEY_WIFIPASSWORD) _settingWiFiPassword.save(value);
		if (key == NVS_KEY_MQTTSERVER) _settingMQTTServer.save(value);
		if (key == NVS_KEY_MQTTPORT) _settingMQTTPort.save(value.toInt());
		if (key == NVS_KEY_MQTTUSERNAME) _settingMQTTUsername.save(value);
		if (key == NVS_KEY_MQTTPASSWORD) _settingMQTTPassword.save(value);
	}

	String html =
		"<!DOCTYPE html>\n"
		"<html>\n"
		"  <head>\n"
		"    <meta charset=\"utf-8\">\n"
		"    <meta name=\"viewport\" content=\"width=device-width, initial-scale:1.0\">\n"
		"    <title>WILCO Settings</title>\n"
		"  </head>\n"
		"  <body>\n"
		"    <h2>Saved. Rebooting.</h2>\n"
		"  </body>\n"
		"</html>";

	webServer.send(200, "text/html", html);
	delay(1000);
	esp_restart();
}

String WILCO::_generateFormField(NVSStringEntry *setting, bool isPassword) {
	String type = isPassword ? "password" : "text";
	String name = setting->name();
	String label = setting->label();
	String value = setting->exists() ? setting->load() : "";

	String output = "";
	output += (String)"<label for=\"" + name + "\">" + label + "</label>\n";
	output += (String)"<input type=\"" + type + "\" id=\"" + name + "\" name=\"" + name + "\" value=\"" + value + "\" />\n";
	
	return output;
}

String WILCO::_generateFormField(NVSIntegerEntry *setting) {
	String name = setting->name();
	String label = setting->label();
	int value = setting->exists() ? setting->load() : NULL;
	
	String output = "";
	output += (String)"<label for=\"" + name + "\">" + label + "</label>\n";
	output += (String)"<input type=\"number\" id=\"" + name + "\" name=\"" + name + "\" value=\"" + value + "\" />\n";

	return output;
}
