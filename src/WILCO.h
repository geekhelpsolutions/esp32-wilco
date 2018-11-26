// WILCO.h

#ifndef _WILCO_h
#define _WILCO_h

#include "Arduino.h"
#include "NVSNamespace.h"
#include "NVSStringEntry.h"
#include "NVSIntegerEntry.h"


#define NVS_NAMESPACE_NAME    "WILCO"

#define NVS_KEY_WIFISSID       "wifi_ssid"
#define NVS_LABEL_WIFISSID     "WiFi SSID"

#define NVS_KEY_WIFIPASSWORD   "wifi_password"
#define NVS_LABEL_WIFIPASSWORD "WiFi Password"

#define NVS_KEY_MQTTSERVER     "mqtt_server"
#define NVS_LABEL_MQTTSERVER   "MQTT Server"

#define NVS_KEY_MQTTPORT       "mqtt_port"
#define NVS_LABEL_MQTTPORT     "MQTT Port"

#define NVS_KEY_MQTTUSERNAME   "mqtt_username"
#define NVS_LABEL_MQTTUSERNAME "MQTT Username"

#define NVS_KEY_MQTTPASSWORD   "mqtt_password"
#define NVS_LABEL_MQTTPASSWORD "MQTT Password"


#define AP_SSID      "WILCO AP"
#define AP_PASSWORD  NULL
#define AP_TIMEOUT   10


class WILCO {
public:
	WILCO() :
		_namespace(NVSNamespace(NVS_NAMESPACE_NAME)),
		_settingWiFiSSID(_namespace.stringEntry(NVS_KEY_WIFISSID, NVS_LABEL_WIFISSID)),
		_settingWiFiPassword(_namespace.stringEntry(NVS_KEY_WIFIPASSWORD, NVS_LABEL_WIFIPASSWORD)),
		_settingMQTTServer(_namespace.stringEntry(NVS_KEY_MQTTSERVER, NVS_LABEL_MQTTSERVER)),
		_settingMQTTPort(_namespace.integerEntry(NVS_KEY_MQTTPORT, NVS_LABEL_MQTTPORT)),
		_settingMQTTUsername(_namespace.stringEntry(NVS_KEY_MQTTUSERNAME, NVS_LABEL_MQTTUSERNAME)),
		_settingMQTTPassword(_namespace.stringEntry(NVS_KEY_MQTTPASSWORD, NVS_LABEL_MQTTPASSWORD))
	{};
	void begin();
	void loop();

private:
	IPAddress _ip;
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
	void _startMQTTClient();
	String _mqttId();

	void _serveSettingsForm();
	void _handleSettingsForm();

	String _generateFormField(NVSStringEntry *setting, bool isPassword = false);
	String _generateFormField(NVSIntegerEntry *setting);

	void _handleMQTTMessage(char * topic, byte * payload, unsigned int length);
};

#endif