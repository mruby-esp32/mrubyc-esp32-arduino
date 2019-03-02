/*! @file
  @brief
  mruby/c Range object

  <pre>
  Copyright (C) 2015-2018 Kyushu Institute of Technology.
  Copyright (C) 2015-2018 Shimane IT Open-Innovation Center.

  This file is distributed under BSD 3-Clause License.

  </pre>
*/

#ifndef MRBC_SRC_C_RANGE_H_
#define MRBC_SRC_C_RANGE_H_

#include <stdint.h>
#include "value.h"

#ifdef __cplusplus
extern "C" {
#endif

//================================================================
/*!@brief
  Define Range object (same the handles of other objects)
*/
typedef struct RRange {
  MRBC_OBJECT_HEADER;

  uint8_t flag_exclude;	// true: exclude the end object, otherwise include.
  mrb_value first;
  mrb_value last;

} mrb_range;


struct VM;

mrb_value mrbc_range_new(struct VM *vm, mrb_value *first, mrb_value *last, int flag_exclude);
void mrbc_range_delete(mrb_value *v);
void mrbc_range_clear_vm_id(mrb_value *v);
int mrbc_range_compare(const mrb_value *v1, const mrb_value *v2);
void mrbc_init_class_range(mrb_vm *vm);



//================================================================
/*! get first value
*/
static inline mrb_value mrbc_range_first(const mrb_value *v)
{
  return v->range->first;
}

//================================================================
/*! get last value
*/
static inline mrb_value mrbc_range_last(const mrb_value *v)
{
  return v->range->last;
}

//================================================================
/*! get exclude_end?
*/
static inline int mrbc_range_exclude_end(const mrb_value *v)
{
  return v->range->flag_exclude;
}



#ifdef __cplusplus
}
#endif
#endif
