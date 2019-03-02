/*! @file
  @brief
  mruby/c Key(Symbol) - Value store.

  <pre>
  Copyright (C) 2015-2018 Kyushu Institute of Technology.
  Copyright (C) 2015-2018 Shimane IT Open-Innovation Center.

  This file is distributed under BSD 3-Clause License.

  </pre>
*/

#ifndef MRBC_SRC_C_KEYVALUE_H_
#define MRBC_SRC_C_KEYVALUE_H_

#include "value.h"

#ifdef __cplusplus
extern "C" {
#endif

//================================================================
/*! Define Key-Value data.
*/
typedef struct RKeyValue {
  mrb_sym sym_id;	//!< symbol ID as key.
  mrb_value value;	//!< stored value.

} mrb_kv;


//================================================================
/*! Define Key-Value handle.
*/
typedef struct RKeyValueHandle {
  uint16_t data_size;	//!< data buffer size.
  uint16_t n_stored;	//!< # of stored.
  mrb_kv *data;		//!< pointer to allocated memory.

} mrb_kv_handle;


mrb_kv_handle *mrbc_kv_new(struct VM *vm, int size);
void mrbc_kv_delete(mrb_kv_handle *kvh);
void mrbc_kv_clear_vm_id(mrb_kv_handle *kvh);
int mrbc_kv_resize(mrb_kv_handle *kvh, int size);
int mrbc_kv_set(mrb_kv_handle *kvh, mrb_sym sym_id, mrb_value *set_val);
mrb_value *mrbc_kv_get(mrb_kv_handle *kvh, mrb_sym sym_id);
int mrbc_kv_append(mrb_kv_handle *kvh, mrb_sym sym_id, mrb_value *set_val);
int mrbc_kv_reorder(mrb_kv_handle *kvh);
int mrbc_kv_remove(mrb_kv_handle *kvh, mrb_sym sym_id);
void mrbc_kv_clear(mrb_kv_handle *kvh);



//================================================================
/*! get size
*/
inline static int mrbc_kv_size(const mrb_kv_handle *kvh)
{
  return kvh->n_stored;
}


#ifdef __cplusplus
}
#endif
#endif
