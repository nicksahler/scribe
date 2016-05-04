// Copyright Benoit Blanchon 2014-2016
// MIT License
//
// Arduino JSON library
// https://github.com/bblanchon/ArduinoJson
// If you like this project, please add a star!

#include "JsonBuffer.h"

#include "JsonParser.h"
#include "JsonArray.h"
#include "JsonObject.h"

using namespace ArduinoJson;
using namespace ArduinoJson::Internals;

JsonArray &JsonBuffer::createArray() {
  JsonArray *ptr = new (this) JsonArray(this);
  return ptr ? *ptr : JsonArray::invalid();
}

JsonObject &JsonBuffer::createObject() {
  JsonObject *ptr = new (this) JsonObject(this);
  return ptr ? *ptr : JsonObject::invalid();
}

JsonArray &JsonBuffer::parseArray(char *json, uint8_t nestingLimit) {
  JsonParser parser(this, json, nestingLimit);
  return parser.parseArray();
}

JsonObject &JsonBuffer::parseObject(char *json, uint8_t nestingLimit) {
  JsonParser parser(this, json, nestingLimit);
  return parser.parseObject();
}

JsonVariant JsonBuffer::parse(char *json, uint8_t nestingLimit) {
  JsonParser parser(this, json, nestingLimit);
  return parser.parseVariant();
}

char *JsonBuffer::strdup(const char *source, size_t length) {
  size_t size = length + 1;
  char *dest = static_cast<char *>(alloc(size));
  if (dest != NULL) memcpy(dest, source, size);
  return dest;
}
