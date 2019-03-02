/*! @file
  @brief
  mruby/c Symbol class

  <pre>
  Copyright (C) 2015-2018 Kyushu Institute of Technology.
  Copyright (C) 2015-2018 Shimane IT Open-Innovation Center.

  This file is distributed under BSD 3-Clause License.

  </pre>
*/

#ifndef MRBC_SRC_SYMBOL_H_
#define MRBC_SRC_SYMBOL_H_

#include "value.h"

#ifdef __cplusplus
extern "C" {
#endif

struct VM;

mrb_value mrbc_symbol_new(struct VM *vm, const char *str);
uint16_t calc_hash(const char *str);
mrb_sym str_to_symid(const char *str);
const char *symid_to_str(mrb_sym sym_id);
void mrbc_init_class_symbol(struct VM *vm);


//================================================================
/*! get c-language string (char *)
*/
static inline const char * mrbc_symbol_cstr(const mrb_value *v)
{
  return symid_to_str(v->i);
}


#ifdef __cplusplus
}
#endif
#endif
