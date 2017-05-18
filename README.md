# ESP32 RGB LED for homebridge

This is a project made for [homebridge-better-http-rgb](https://www.npmjs.com/package/homebridge-better-http-rgb) on [HomeBridge](https://github.com/nfarina/homebridge).

HTTP server implemented from [lwip-allnetworks](https://github.com/yiannisy/lwip-allnetworks).

Sample HomeBridge config:
```json
{
    "bridge": {
        "name": "Homebridge",
        "username": "B8:27:EB:F9:51:49",
        "port": 51826,
        "pin": "031-45-154"
    },

    "description": "Homebridge",

    "accessories": [
    {
        "accessory": "HTTP-RGB",
        "name": "RGB Led on ESP32",

        "switch": {
            "status": "http://192.168.1.115/cmd/status/",
            "powerOn": "http://192.168.1.115/cmd/on/",
            "powerOff": "http://192.168.1.115/cmd/off/"
        },

        "brightness": {
            "status": "http://192.168.1.115/cmd/brightness/",
            "url": "http://192.168.1.115/cmd/brightness:%s/"
        },

        "color": {
            "status": "http://192.168.1.115/cmd/color/",
            "url": "http://192.168.1.115/cmd/color:%s/",
            "brightness": true
        }
    }],

    "platforms": []
}
```

 **NOTICE0:** Be sure to include "COMPONENT_LDFLAGS += -lstdc++" in Makefile in order to compile.

 **NOTICE1:** Although I'm using C++ class, it doesn't mean that you can control multiple RGB LED simultaneously by creating multiple objects.

 **NOTICE2:** I included support for ledc_fade function in [ESP-IDF](https://github.com/espressif/esp-idf), but I strongly recommend not using it because it will jam the server thread.
