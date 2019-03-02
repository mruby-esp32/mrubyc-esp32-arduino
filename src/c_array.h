/*! @file
  @brief
  mruby/c Array class

  <pre>
  Copyright (C) 2015-2018 Kyushu Institute of Technology.
  Copyright (C) 2015-2018 Shimane IT Open-Innovation Center.

  This file is distributed under BSD 3-Clause License.

  </pre>
*/

#ifndef MRBC_SRC_C_ARRAY_H_
#define MRBC_SRC_C_ARRAY_H_

#include "value.h"

#ifdef __cplusplus
extern "C" {
#endif

//================================================================
/*!@brief
  Define Array handle.
*/
typedef struct RArray {
  MRBC_OBJECT_HEADER;

  uint16_t data_size;	//!< data buffer size.
  uint16_t n_stored;	//!< # of stored.
  mrb_value *data;	//!< pointer to allocated memory.

} mrb_array;


struct VM;

mrb_value mrbc_array_new(struct VM *vm, int size);
void mrbc_array_delete(mrb_value *ary);
void mrbc_array_clear_vm_id(mrb_value *ary);
int mrbc_array_resize(mrb_value *ary, int size);
int mrbc_array_set(mrb_value *ary, int idx, mrb_value *set_val);
mrb_value mrbc_array_get(mrb_value *ary, int idx);
int mrbc_array_push(mrb_value *ary, mrb_value *set_val);
mrb_value mrbc_array_pop(mrb_value *ary);
int mrbc_array_unshift(mrb_value *ary, mrb_value *set_val);
mrb_value mrbc_array_shift(mrb_value *ary);
int mrbc_array_insert(mrb_value *ary, int idx, mrb_value *set_val);
mrb_value mrbc_array_remove(mrb_value *ary, int idx);
void mrbc_array_clear(mrb_value *ary);
int mrbc_array_compare(const mrb_value *v1, const mrb_value *v2);
void mrbc_array_minmax(mrb_value *ary, mrb_value **pp_min_value, mrb_value **pp_max_value);
void mrbc_init_class_array(struct VM *vm);


//================================================================
/*! get size
*/
inline static int mrbc_array_size(const mrb_value *ary)
{
  return ary->array->n_stored;
}


#ifdef __cplusplus
}
#endif
#endif
