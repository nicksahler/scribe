#include "ada_print.h"
#include "HttpClient.h"
#include "ArduinoJson.h"

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
    request.hostname = "api.myjson.com";
    request.port = 80;
    request.path = "/bins/" + resource;

    http.get(request, response, headers);

    if (response.status == 200) {
      printer.println(response.body);
      printer.feed(2);
    }

    return response.status;
}
