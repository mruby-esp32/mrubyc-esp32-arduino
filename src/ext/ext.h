/*
  ext.h

  Header file of extension methods

  Copyright (c) 2018, katsuhiko kageyama All rights reserved.

*/

#ifndef __EXT_H_
#define __EXT_H_

#include "mrubyc_config.h"
#include "ext_debug.h"
#include "value.h"

#define RECV_BUFF_SIZE 1024

bool mrbc_trans_cppbool_value(mrb_vtype tt);

/* Basic Classes for ESP32 Arduino */
/* Please install a depending board package: https://github.com/espressif/arduino-esp32/blob/master/docs/arduino-ide/boards_manager.md */

void define_esp_class(void);
void define_arduino_class(void);
void define_serial_class(void);

/* Additional Classes */

#ifdef USE_RGB_LCD
void define_rgb_lcd_class(void);
#endif

/* Please install depending library: http://www.m5stack.com/assets/docs/ */
#ifdef ARDUINO_M5Stack_Core_ESP32
void define_m5stack_class(void);
#ifdef USE_M5AVATAR
void define_m5avatar_class(void);
#endif
#endif

#endif
