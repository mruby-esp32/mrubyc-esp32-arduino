/*! @file
  @brief
  mruby/c Hash class

  <pre>
  Copyright (C) 2015-2018 Kyushu Institute of Technology.
  Copyright (C) 2015-2018 Shimane IT Open-Innovation Center.

  This file is distributed under BSD 3-Clause License.

  </pre>
*/

#ifndef MRBC_SRC_C_HASH_H_
#define MRBC_SRC_C_HASH_H_

#include "value.h"
#include "c_array.h"

#ifdef __cplusplus
extern "C" {
#endif

//================================================================
/*!@brief
  Define Hash handle.
*/
typedef struct RHash {
  // (NOTE)
  //  Needs to be same members and order as RArray.
  MRBC_OBJECT_HEADER;

  uint16_t data_size;	//!< data buffer size.
  uint16_t n_stored;	//!< # of stored.
  mrb_value *data;	//!< pointer to allocated memory.

  // TODO: and other member for search.

} mrb_hash;


//================================================================
/*!@brief
  Define Hash iterator.
*/
typedef struct RHashIterator {
  mrb_hash *target;
  mrb_value *point;
  mrb_value *p_end;
} mrb_hash_iterator;


mrb_value mrbc_hash_new(struct VM *vm, int size);
void mrbc_hash_delete(mrb_value *hash);
mrb_value *mrbc_hash_search(const mrb_value *hash, const mrb_value *key);
int mrbc_hash_set(mrb_value *hash, mrb_value *key, mrb_value *val);
mrb_value mrbc_hash_get(mrb_value *hash, mrb_value *key);
mrb_value mrbc_hash_remove(mrb_value *hash, mrb_value *key);
void mrbc_hash_clear(mrb_value *hash);
int mrbc_hash_compare(const mrb_value *v1, const mrb_value *v2);
mrb_value mrbc_hash_dup(struct VM *vm, mrb_value *src);
void mrbc_init_class_hash(struct VM *vm);


//================================================================
/*! get size
*/
inline static int mrbc_hash_size(const mrb_value *hash) {
  return hash->hash->n_stored / 2;
}

//================================================================
/*! clear vm_id
*/
inline static void mrbc_hash_clear_vm_id(mrb_value *hash) {
  mrbc_array_clear_vm_id(hash);
}

//================================================================
/*! resize buffer
*/
inline static int mrbc_hash_resize(mrb_value *hash, int size)
{
  return mrbc_array_resize(hash, size * 2);
}


//================================================================
/*! iterator constructor
*/
inline static mrb_hash_iterator mrbc_hash_iterator( mrb_value *v )
{
  mrb_hash_iterator ite;
  ite.target = v->hash;
  ite.point = v->hash->data;
  ite.p_end = ite.point + v->hash->n_stored;

  return ite;
}

//================================================================
/*! iterator has_next?
*/
inline static int mrbc_hash_i_has_next( mrb_hash_iterator *ite )
{
  return ite->point < ite->p_end;
}

//================================================================
/*! iterator getter
*/
inline static mrb_value *mrbc_hash_i_next( mrb_hash_iterator *ite )
{
  mrb_value *ret = ite->point;
  ite->point += 2;
  return ret;
}


#ifdef __cplusplus
}
#endif
#endif
