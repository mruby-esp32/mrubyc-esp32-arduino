/*
  ext.cpp

  Defining extension methods for ESP32

  Copyright (c) 2018, katsuhiko kageyama All rights reserved.

*/

#include "mrubyc_for_ESP32_Arduino.h"
#include "ext.h"

bool mrbc_trans_cppbool_value(mrb_vtype tt)
{
	if(tt==MRB_TT_TRUE){
		return true;
	}
	return false;
}

static void class_mrubyc_version(mrb_vm *vm, mrb_value *v, int argc )
{
  const char* buf = ESP32MRBC_VERSION;
  mrb_value value = mrbc_string_new_cstr(vm, buf);
  SET_RETURN(value);
}

void define_mrubyc_class(void)
{
  mrb_class *mrubyc;
  mrubyc = mrbc_define_class(0, "Mrubyc", mrbc_class_object);
  mrbc_define_method(0, mrubyc, "version", class_mrubyc_version);
}

void mrbc_define_user_class(void)
{
  define_mrubyc_class();
  define_esp_class();
  define_arduino_class();
  define_serial_class();

#ifdef USE_RGB_LCD
  define_rgb_lcd_class();
#endif

#ifdef ARDUINO_M5Stack_Core_ESP32
  define_m5stack_class();
#ifdef USE_M5AVATAR
  define_m5avatar_class();
#endif
#endif
}
