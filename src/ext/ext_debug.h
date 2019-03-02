/*
  ext_debug.h

  Header file of extension methods

  Copyright (c) 2018, katsuhiko kageyama All rights reserved.

*/

#ifndef __EXT_DEBUG_H_
#define __EXT_DEBUG_H_

#include <Arduino.h>
#include "mrubyc_config.h"

#if defined(USE_USB_SERIAL_FOR_STDIO) && defined(ESP32_DEBUG)
#define DEBUG_PRINT(val)    Serial.print(val)
#define DEBUG_PRINTLN(val)  Serial.println(val)
#else
#define DEBUG_PRINT(val)
#define DEBUG_PRINTLN(val)
#endif

#endif //__EXT_DEBUG_H_
