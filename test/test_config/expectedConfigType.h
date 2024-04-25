#pragma once

#include <Arduino.h>
#include <configScenario.hpp>


struct  expectedConfigStruct {
  void *     pObj;
  const char *  cmd;
  const char *  str;
  uint32_t      p1;
  uint32_t      p2;
  uint32_t      p3;
  uint32_t      p4;
  uint32_t      size;
  uint8_t       data[5];
};

extern expectedConfigStruct sollEmptyConfig;

void test_empty_config(configItem * pScen);
void test_expected_config(expectedConfigStruct soll,configItem * pScen);
