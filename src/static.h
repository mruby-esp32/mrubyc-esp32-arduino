/*! @file
  @brief
  Declare static data.

  <pre>
  Copyright (C) 2015-2016 Kyushu Institute of Technology.
  Copyright (C) 2015-2016 Shimane IT Open-Innovation Center.

  This file is distributed under BSD 3-Clause License.
  </pre>
*/

#ifndef MRBC_SRC_STATIC_H_
#define MRBC_SRC_STATIC_H_

#include "vm.h"
#include "global.h"
#include "value.h"

#ifdef __cplusplus
extern "C" {
#endif


/* VM */
extern mrb_vm mrbc_vm[];

/* Object */
//extern mrb_object *mrbc_pool_object;


/* Class Tree */
extern mrb_class *mrbc_class_object;

extern mrb_class *mrbc_class_proc;
extern mrb_class *mrbc_class_false;
extern mrb_class *mrbc_class_true;
extern mrb_class *mrbc_class_nil;
extern mrb_class *mrbc_class_array;
extern mrb_class *mrbc_class_fixnum;
extern mrb_class *mrbc_class_float;
extern mrb_class *mrbc_class_math;
extern mrb_class *mrbc_class_string;
extern mrb_class *mrbc_class_symbol;
extern mrb_class *mrbc_class_range;
extern mrb_class *mrbc_class_hash;


void init_static(void);


#ifdef __cplusplus
}
#endif
#endif
