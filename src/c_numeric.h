/*! @file
  @brief


  <pre>
  Copyright (C) 2015 Kyushu Institute of Technology.
  Copyright (C) 2015 Shimane IT Open-Innovation Center.

  This file is distributed under BSD 3-Clause License.


  </pre>
*/

#ifndef MRBC_SRC_C_NUMERIC_H_
#define MRBC_SRC_C_NUMERIC_H_

#include "vm.h"

#ifdef __cplusplus
extern "C" {
#endif


void mrbc_init_class_fixnum(mrb_vm *vm);
void mrbc_init_class_float(mrb_vm *vm);

#ifdef __cplusplus
}
#endif
#endif
