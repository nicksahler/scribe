#pragma once

#include "application.h"
#include "spark_wiring_string.h"
#include "spark_wiring_tcpclient.h"
#include "spark_wiring_usbserial.h"
#include <vector>

/**
 * This struct is used to pass additional HTTP headers such as API-keys.
 * Normally you pass this as an array. The last entry must have NULL as key.
 */
typedef struct
{
  const char* header;
  const char* value;
} http_header_t;

/**
 * HTTP Request struct.
 * hostname request host
 * path	 request path
 * port   request port
 * body	 request body
 */
typedef struct
{
  String hostname;
  IPAddress ip;
  String path;
  // TODO: Look at setting the port by default.
  //int port = 80;
  int port;
  String body;
} http_request_t;

typedef struct
{
  int status;
  String body;
} http_response_t;

class HttpClient {
public:
  TCPClient client;
  std::vector<char> buffer;

  HttpClient(void);

  void request(http_request_t &req, http_response_t &res, http_header_t headers[] = (http_header_t*)NULL, const char* method = "GET");

private:
  void sendHeader(const char* aHeaderName, const char* aHeaderValue);
  void sendHeader(const char* aHeaderName, const int aHeaderValue);
  void sendHeader(const char* aHeaderName);
};
