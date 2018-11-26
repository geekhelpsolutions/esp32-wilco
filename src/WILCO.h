// WILCO.h

#ifndef _WILCO_h
#define _WILCO_h

#include "Arduino.h"
#include "NVSNamespace.h"
#include "NVSStringEntry.h"
#include "NVSIntegerEntry.h"

#define NVS_NAMESPACE_NAME    "WILCO"
#define NVS_KEY_WIFISSID      "wifi_ssid"
#define NVS_KEY_WIFIPASSWORD  "wifi_password"
#define NVS_KEY_MQTTSERVER    "mqtt_server"
#define NVS_KEY_MQTTPORT      "mqtt_port"
#define NVS_KEY_MQTTUSERNAME  "mqtt_username"
#define NVS_KEY_MQTTPASSWORD  "mqtt_password"

#define AP_SSID      "WILCO AP"
#define AP_PASSWORD  NULL
#define AP_TIMEOUT   10


class WILCO {
	WILCO() :
		_namespace(NVSNamespace(NVS_NAMESPACE_NAME)),
		_settingWiFiSSID(_namespace.stringEntry(NVS_KEY_WIFISSID)),
		_settingWiFiPassword(_namespace.stringEntry(NVS_KEY_WIFIPASSWORD)),
		_settingMQTTServer(_namespace.stringEntry(NVS_KEY_MQTTSERVER)),
		_settingMQTTPort(_namespace.integerEntry(NVS_KEY_MQTTPORT)),
		_settingMQTTUsername(_namespace.stringEntry(NVS_KEY_MQTTUSERNAME)),
		_settingMQTTPassword(_namespace.stringEntry(NVS_KEY_MQTTPASSWORD))
	{};
	void begin();
	void loop();

private:
	NVSNamespace _namespace;
	NVSStringEntry _settingWiFiSSID;
	NVSStringEntry _settingWiFiPassword;
	NVSStringEntry _settingMQTTServer;
	NVSIntegerEntry _settingMQTTPort;
	NVSStringEntry _settingMQTTUsername;
	NVSStringEntry _settingMQTTPassword;

	void _startAPMode();
	void _startClientMode();
	void _startDNSServer();
	void _startWebServer();

	String _generateFormField(NVSStringEntry *setting, bool isPassword);
	String _generateFormField(NVSIntegerEntry *setting);
};

#endif