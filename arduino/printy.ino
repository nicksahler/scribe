#include "ada_print.h"
#include "HttpClient.h"
#include "ArduinoJson.h"

#include <vector>

Adafruit_Thermal printer(&Serial1);

HttpClient http;
http_request_t request;
http_response_t response;
http_header_t headers[] = {
    //  { "Content-Type", "application/json" },
    //  { "Accept" , "application/json" },
    { "Accept" , "*/*"},
    { NULL, NULL } // NOTE: Always terminate headers will NULL
};

void setup() {
  Serial1.begin(19200);

  printer.begin();
  printer.underlineOn();
  printer.println(F("Print Server Opened"));
  printer.underlineOff();
  printer.feed(2);
  printer.setDefault();

  Particle.function("notify", notify);
}

void loop() {
}

int notify(String resource) {
    request.hostname = "100.8.214.195";
    request.port = 3000;
    request.path = "/items/" + resource;

    http.get(request, response, headers);

    /*std::vector<uint8_t> v {10,27,33,56,27,45,1,73,78,67,79,77,73,78,71,32,77,69,83,83,65,71,69,27,45,0,27,33,0,10,10,109,101,108,63,32,77,111,114,101,32,108,105,107,101,32,27,33,8,115,109,101,108,108,27,33,0,33,33,33,33,32,33,33,32,33,10};
    for( int i = 0; i < v.size(); i++ )
     printer.writeBytes(v[i]);

    printer.feed(2);*/

    if (response.status == 200) {
      DynamicJsonBuffer jsonBuffer;
      printer.println("Message");

      JsonObject& root = jsonBuffer.parseObject(response.body);
      if (!root.success())
        printer.println("Failed");

      JsonArray& message = root["message"].asArray();

      if (!message.success())
        printer.println("Failed message");

      for(JsonArray::iterator it=message.begin(); it!=message.end(); ++it) {
        printer.writeBytes((uint8_t)atoi(*it));
      }
      printer.feed(2);
    }

    return response.status;
}
