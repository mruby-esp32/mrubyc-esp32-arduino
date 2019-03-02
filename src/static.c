/*! @file
  @brief
  Declare static data.

  <pre>
  Copyright (C) 2015-2016 Kyushu Institute of Technology.
  Copyright (C) 2015-2016 Shimane IT Open-Innovation Center.

  This file is distributed under BSD 3-Clause License.
  </pre>
*/

#include "vm_config.h"
#include "static.h"
#include "class.h"
#include "symbol.h"

/* Class Tree */
mrb_class *mrbc_class_object;

/* Proc */
mrb_class *mrbc_class_proc;

/* Classes */
mrb_class *mrbc_class_false;
mrb_class *mrbc_class_true;
mrb_class *mrbc_class_nil;
mrb_class *mrbc_class_array;
mrb_class *mrbc_class_fixnum;
mrb_class *mrbc_class_symbol;
mrb_class *mrbc_class_float;
mrb_class *mrbc_class_math;
mrb_class *mrbc_class_string;
mrb_class *mrbc_class_range;
mrb_class *mrbc_class_hash;

void init_static(void)
{
  mrbc_init_global();

  mrbc_init_class();
}
