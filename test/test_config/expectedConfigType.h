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
};

extern expectedConfigStruct sollEmptyConfig;

extern void test_empty_config(configItem * pScen);
extern void test_expected_config(expectedConfigStruct soll,configItem * pScen);
