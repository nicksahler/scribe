#include "HttpClient.h"
#include <string>
#define LOGGING
static const uint16_t TIMEOUT = 5000; // Allow maximum 5s between data packets.

/**
* Constructor.
*/
HttpClient::HttpClient()
{

}

/**
* Method to send a header, should only be called from within the class.
*/
void HttpClient::sendHeader(const char* aHeaderName, const char* aHeaderValue) {
  client.print(aHeaderName);
  client.print(": ");
  client.println(aHeaderValue);

  #ifdef LOGGING
  Serial.print(aHeaderName);
  Serial.print(": ");
  Serial.println(aHeaderValue);
  #endif
}

void HttpClient::sendHeader(const char* aHeaderName, const int aHeaderValue) {
  client.print(aHeaderName);
  client.print(": ");
  client.println(aHeaderValue);

  #ifdef LOGGING
  Serial.print(aHeaderName);
  Serial.print(": ");
  Serial.println(aHeaderValue);
  #endif
}

void HttpClient::sendHeader(const char* aHeaderName) {
  client.println(aHeaderName);

  #ifdef LOGGING
  Serial.println(aHeaderName);
  #endif
}

/**
* Method to send an HTTP Request. Allocate variables in your application code
* in the res struct and set the headers and the options in the req
* struct.
*/
void HttpClient::request(http_request_t &req, http_response_t &res, http_header_t headers[], const char* method) {
  // If a proper response code isn't received it will be set to -1.
  res.status = -1;

  // NOTE: The default port tertiary statement is unpredictable if the request structure is not initialised
  // http_request_t request = {0} or memset(&request, 0, sizeof(http_request_t)) should be used
  // to ensure all fields are zero
  bool connected = false;
  if(req.hostname!=NULL) {
    connected = client.connect(req.hostname.c_str(), (req.port) ? req.port : 80 );
  }   else {
    connected = client.connect(req.ip, req.port);
  }

  #ifdef LOGGING
  if (connected) {
    if(req.hostname!=NULL) {
      Serial.print("HttpClient>\tConnecting to: ");
      Serial.print(req.hostname);
    } else {
      Serial.print("HttpClient>\tConnecting to IP: ");
      Serial.print(req.ip);
    }
    Serial.print(":");
    Serial.println(req.port);
  } else {
    Serial.println("HttpClient>\tConnection failed.");
  }
  #endif

  if (!connected) {
    client.stop();
    // If TCP Client can't connect to host, exit here.
    return;
  }

  //
  // Send HTTP Headers
  //

  // Send initial headers (only HTTP 1.0 is supported for now).
  client.print(method);
  client.print(" ");
  client.print(req.path);
  client.print(" HTTP/1.0\r\n");

  #ifdef LOGGING
  Serial.println("HttpClient>\tStart of HTTP Request.");
  Serial.print(method);
  Serial.print(" ");
  Serial.print(req.path);
  Serial.print(" HTTP/1.0\r\n");
  #endif

  // Send General and Request Headers.
  sendHeader("Connection", "close"); // Not supporting keep-alive for now.
  if(req.hostname!=NULL) {
    sendHeader("HOST", req.hostname.c_str());
  }

  //Send Entity Headers
  // TODO: Check the standard, currently sending Content-Length : 0 for empty
  // POST requests, and no content-length for other types.
  if (req.body != NULL) {
    sendHeader("Content-Length", (req.body).length());
  } else if (strcmp(method, "POST") == 0) { //Check to see if its a Post method.
    sendHeader("Content-Length", 0);
  }

  if (headers != NULL)
  {
    int i = 0;
    while (headers[i].header != NULL)
    {
      if (headers[i].value != NULL) {
        sendHeader(headers[i].header, headers[i].value);
      } else {
        sendHeader(headers[i].header);
      }
      i++;
    }
  }

  // Empty line to finish headers
  client.println();
  client.flush();

  //
  // Send HTTP Request Body
  //

  if (req.body != NULL) {
    client.println(req.body);

    #ifdef LOGGING
    Serial.println(req.body);
    #endif
  }

  #ifdef LOGGING
  Serial.println("HttpClient>\tEnd of HTTP Request.");
  #endif

  buffer.clear();

  //
  // Receive HTTP Response
  //
  // The first value of client.available() might not represent the
  // whole response, so after the first chunk of data is received instead
  // of terminating the connection there is a delay and another attempt
  // to read data.
  // The loop exits when the connection is closed, or if there is a
  // timeout or an error.

  unsigned long lastRead = millis();
  unsigned long firstRead = millis();
  bool error = false;
  bool timeout = false;

  do {
    #ifdef LOGGING
    int bytes = client.available();
    if(bytes) {
      Serial.print("\r\nHttpClient>\tReceiving TCP transaction of ");
      Serial.print(bytes);
      Serial.println(" bytes.");
    }
    #endif

    while (client.available()) {
      char c = client.read();
      #ifdef LOGGING
      Serial.print(c);
      #endif
      lastRead = millis();

      if (c == -1) {
        error = true;

        #ifdef LOGGING
        Serial.println("HttpClient>\tError: No data available.");
        #endif

        break;
      }

      buffer.push_back(c);

    }

    #ifdef LOGGING
    if (bytes) {
      Serial.print("\r\nHttpClient>\tEnd of TCP transaction.");
    }
    #endif

    // Check that there hasn't been more than 5s since last read.
    timeout = millis() - lastRead > TIMEOUT;

    // Unless there has been an error or timeout wait 200ms to allow server
    // to respond or close connection.
    if (!error && !timeout) {
      delay(200);
    }
  } while (client.connected() && !timeout && !error);

  #ifdef LOGGING
  if (timeout) {
    Serial.println("\r\nHttpClient>\tError: Timeout while reading response.");
  }
  Serial.print("\r\nHttpClient>\tEnd of HTTP Response (");
  Serial.print(millis() - firstRead);
  Serial.println("ms).");
  #endif
  client.stop();

  buffer.push_back('\0'); // Null-terminate buffer
  Serial.println(buffer.size());
  std::string temp(buffer.begin(), buffer.end());
  String raw_response = temp.c_str();
  // Not super elegant way of finding the status code, but it works.
  String statusCode = raw_response.substring(9,12);

  #ifdef LOGGING
  Serial.print("HttpClient>\tStatus Code: ");
  Serial.println(statusCode);
  #endif

  int bodyPos = raw_response.indexOf("\r\n\r\n");
  if (bodyPos == -1) {
    #ifdef LOGGING
    Serial.println("HttpClient>\tError: Can't find HTTP response body.");
    #endif

    return;
  }
  // Return the entire message body from bodyPos+4 till end.
  res.body = "";
  res.body += raw_response.substring(bodyPos+4);
  res.status = atoi(statusCode.c_str());
}
