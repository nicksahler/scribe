#include "ada_print.h"
#include "HttpClient.h"
#include "ArduinoJson.h"

#include <vector>

Adafruit_Thermal printer(&Serial1);

HttpClient http;
http_request_t request;
http_response_t response;
http_header_t headers[] = {
    { "Content-Type", "application/json" },
    { "Accept" , "*/*"},
    { NULL, NULL } // NOTE: Always terminate headers will NULL
};

String hostname = "100.8.214.195";

void setup() {
  Serial1.begin(19200);

  printer.begin();
  printer.underlineOn();
  printer.println(F("Print Server Opened"));
  printer.underlineOff();
  printer.feed(2);
  printer.setDefault();

  Particle.function("notify", notify);
  Particle.variable("hostname", hostname);
}

void loop() { }

int notify(String resource) {
    request.hostname = "100.8.214.195";
    request.port = 3000;
    request.path = "/items/" + resource;

    http.request(request, response, headers);

    if (response.status == 200) {
      DynamicJsonBuffer jsonBuffer;
      printer.println("Message");
      Serial.println(response.body);

      JsonObject& root = jsonBuffer.parseObject(response.body);
      if (!root.success())
        return -1;

      JsonArray& message = root["message"].asArray();

      if (!message.success())
        return -2;

      for(JsonArray::iterator it=message.begin(); it!=message.end(); ++it) {
        printer.writeBytes((uint8_t)atoi(*it));
      }
      printer.feed(2);
    }

    return response.status;
}
