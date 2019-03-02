/*
  ext_esp.cpp

  Defining extension methods of ESP32

  Copyright (c) 2018, katsuhiko kageyama All rights reserved.

*/

#include "mrubyc_config.h"
#include "mrubyc.h"
#include "ext.h"
#include <esp_system.h>

static void class_esp_idf_version(mrb_vm *vm, mrb_value *v, int argc )
{
  const char* buf = esp_get_idf_version();
  mrb_value value = mrbc_string_new_cstr(vm, buf);
  SET_RETURN(value);
}


void define_esp_class(void)
{
  mrb_class *class_esp;
  class_esp = mrbc_define_class(0, "Esp", mrbc_class_object);
  mrbc_define_method(0, class_esp, "idf_version", class_esp_idf_version);

}
