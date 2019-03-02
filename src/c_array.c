/*! @file
  @brief
  mruby/c Array class

  <pre>
  Copyright (C) 2015-2018 Kyushu Institute of Technology.
  Copyright (C) 2015-2018 Shimane IT Open-Innovation Center.

  This file is distributed under BSD 3-Clause License.

  </pre>
*/

#include "vm_config.h"
#include <string.h>
#include <assert.h>

#include "value.h"
#include "vm.h"
#include "alloc.h"
#include "static.h"
#include "class.h"
#include "c_array.h"
#include "console.h"
#include "opcode.h"

/*
  function summary

 (constructor)
    mrbc_array_new

 (destructor)
    mrbc_array_delete

 (setter)
  --[name]-------------[arg]---[ret]-------------------------------------------
    mrbc_array_set	*T	int
    mrbc_array_push	*T	int
    mrbc_array_unshift	*T	int
    mrbc_array_insert	*T	int

 (getter)
  --[name]-------------[arg]---[ret]---[note]----------------------------------
    mrbc_array_get		T	Data remains in the container
    mrbc_array_pop		T	Data does not remain in the container
    mrbc_array_shift		T	Data does not remain in the container
    mrbc_array_remove		T	Data does not remain in the container

 (others)
    mrbc_array_resize
    mrbc_array_clear
    mrbc_array_compare
    mrbc_array_minmax
*/


//================================================================
/*! constructor

  @param  vm	pointer to VM.
  @param  size	initial size
  @return 	array object
*/
mrb_value mrbc_array_new(struct VM *vm, int size)
{
  mrb_value value = {.tt = MRB_TT_ARRAY};

  /*
    Allocate handle and data buffer.
  */
  mrb_array *h = mrbc_alloc(vm, sizeof(mrb_array));
  if( !h ) return value;	// ENOMEM

  mrb_value *data = mrbc_alloc(vm, sizeof(mrb_value) * size);
  if( !data ) {			// ENOMEM
    mrbc_raw_free( h );
    return value;
  }

  h->ref_count = 1;
  h->tt = MRB_TT_ARRAY;
  h->data_size = size;
  h->n_stored = 0;
  h->data = data;

  value.array = h;
  return value;
}


//================================================================
/*! destructor

  @param  ary	pointer to target value
*/
void mrbc_array_delete(mrb_value *ary)
{
  mrb_array *h = ary->array;

  mrb_value *p1 = h->data;
  const mrb_value *p2 = p1 + h->n_stored;
  while( p1 < p2 ) {
    mrbc_dec_ref_counter(p1++);
  }

  mrbc_raw_free(h->data);
  mrbc_raw_free(h);
}


//================================================================
/*! clear vm_id

  @param  ary	pointer to target value
*/
void mrbc_array_clear_vm_id(mrb_value *ary)
{
  mrb_array *h = ary->array;

  mrbc_set_vm_id( h, 0 );

  mrb_value *p1 = h->data;
  const mrb_value *p2 = p1 + h->n_stored;
  while( p1 < p2 ) {
    mrbc_clear_vm_id(p1++);
  }
}


//================================================================
/*! resize buffer

  @param  ary	pointer to target value
  @param  size	size
  @return	mrb_error_code
*/
int mrbc_array_resize(mrb_value *ary, int size)
{
  mrb_array *h = ary->array;

  mrb_value *data2 = mrbc_raw_realloc(h->data, sizeof(mrb_value) * size);
  if( !data2 ) return E_NOMEMORY_ERROR;	// ENOMEM

  h->data = data2;
  h->data_size = size;

  return 0;
}


//================================================================
/*! setter

  @param  ary		pointer to target value
  @param  idx		index
  @param  set_val	set value
  @return		mrb_error_code
*/
int mrbc_array_set(mrb_value *ary, int idx, mrb_value *set_val)
{
  mrb_array *h = ary->array;

  if( idx < 0 ) {
    idx = h->n_stored + idx;
    if( idx < 0 ) return E_INDEX_ERROR;		// raise?
  }

  // need resize?
  if( idx >= h->data_size && mrbc_array_resize(ary, idx + 1) != 0 ) {
    return E_NOMEMORY_ERROR;			// ENOMEM
  }

  if( idx < h->n_stored ) {
    // release existing data.
    mrbc_dec_ref_counter( &h->data[idx] );
  } else {
    // clear empty cells.
    int i;
    for( i = h->n_stored; i < idx; i++ ) {
      h->data[i] = mrb_nil_value();
    }
    h->n_stored = idx + 1;
  }

  h->data[idx] = *set_val;

  return 0;
}


//================================================================
/*! getter

  @param  ary		pointer to target value
  @param  idx		index
  @return		mrb_value data at index position or Nil.
*/
mrb_value mrbc_array_get(mrb_value *ary, int idx)
{
  mrb_array *h = ary->array;

  if( idx < 0 ) idx = h->n_stored + idx;
  if( idx < 0 || idx >= h->n_stored ) return mrb_nil_value();

  return h->data[idx];
}


//================================================================
/*! push a data to tail

  @param  ary		pointer to target value
  @param  set_val	set value
  @return		mrb_error_code
*/
int mrbc_array_push(mrb_value *ary, mrb_value *set_val)
{
  mrb_array *h = ary->array;

  if( h->n_stored >= h->data_size ) {
    int size = h->data_size + 6;
    if( mrbc_array_resize(ary, size) != 0 )
      return E_NOMEMORY_ERROR;		// ENOMEM
  }

  h->data[h->n_stored++] = *set_val;

  return 0;
}


//================================================================
/*! pop a data from tail.

  @param  ary		pointer to target value
  @return		tail data or Nil
*/
mrb_value mrbc_array_pop(mrb_value *ary)
{
  mrb_array *h = ary->array;

  if( h->n_stored <= 0 ) return mrb_nil_value();
  return h->data[--h->n_stored];
}


//================================================================
/*! insert a data to the first.

  @param  ary		pointer to target value
  @param  set_val	set value
  @return		mrb_error_code
*/
int mrbc_array_unshift(mrb_value *ary, mrb_value *set_val)
{
  return mrbc_array_insert(ary, 0, set_val);
}


//================================================================
/*! removes the first data and returns it.

  @param  ary		pointer to target value
  @return		first data or Nil
*/
mrb_value mrbc_array_shift(mrb_value *ary)
{
  mrb_array *h = ary->array;

  if( h->n_stored <= 0 ) return mrb_nil_value();

  mrb_value ret = h->data[0];
  memmove(h->data, h->data+1, sizeof(mrb_value) * --h->n_stored);

  return ret;
}


//================================================================
/*! insert a data

  @param  ary		pointer to target value
  @param  idx		index
  @param  set_val	set value
  @return		mrb_error_code
*/
int mrbc_array_insert(mrb_value *ary, int idx, mrb_value *set_val)
{
  mrb_array *h = ary->array;

  if( idx < 0 ) {
    idx = h->n_stored + idx + 1;
    if( idx < 0 ) return E_INDEX_ERROR;		// raise?
  }

  // need resize?
  int size = 0;
  if( idx >= h->data_size ) {
    size = idx + 1;
  } else if( h->n_stored >= h->data_size ) {
    size = h->data_size + 1;
  }
  if( size && mrbc_array_resize(ary, size) != 0 ) {
    return E_NOMEMORY_ERROR;			// ENOMEM
  }

  // move datas.
  if( idx < h->n_stored ) {
    memmove(h->data + idx + 1, h->data + idx,
	    sizeof(mrb_value) * (h->n_stored - idx));
  }

  // set data
  h->data[idx] = *set_val;
  h->n_stored++;

  // clear empty cells if need.
  if( idx >= h->n_stored ) {
    int i;
    for( i = h->n_stored-1; i < idx; i++ ) {
      h->data[i] = mrb_nil_value();
    }
    h->n_stored = idx + 1;
  }

  return 0;
}


//================================================================
/*! remove a data

  @param  ary		pointer to target value
  @param  idx		index
  @return		mrb_value data at index position or Nil.
*/
mrb_value mrbc_array_remove(mrb_value *ary, int idx)
{
  mrb_array *h = ary->array;

  if( idx < 0 ) idx = h->n_stored + idx;
  if( idx < 0 || idx >= h->n_stored ) return mrb_nil_value();

  mrb_value val = h->data[idx];
  h->n_stored--;
  if( idx < h->n_stored ) {
    memmove(h->data + idx, h->data + idx + 1,
	    sizeof(mrb_value) * (h->n_stored - idx));
  }

  return val;
}


//================================================================
/*! clear all

  @param  ary		pointer to target value
*/
void mrbc_array_clear(mrb_value *ary)
{
  mrb_array *h = ary->array;

  mrb_value *p1 = h->data;
  const mrb_value *p2 = p1 + h->n_stored;
  while( p1 < p2 ) {
    mrbc_dec_ref_counter(p1++);
  }

  h->n_stored = 0;
}


//================================================================
/*! compare

  @param  v1	Pointer to mrb_value
  @param  v2	Pointer to another mrb_value
  @retval 0	v1 == v2
  @retval plus	v1 >  v2
  @retval minus	v1 <  v2
*/
int mrbc_array_compare(const mrb_value *v1, const mrb_value *v2)
{
  int i;
  for( i = 0; ; i++ ) {
    if( i >= mrbc_array_size(v1) || i >= mrbc_array_size(v2) ) {
      return mrbc_array_size(v1) - mrbc_array_size(v2);
    }

    int res = mrbc_compare( &v1->array->data[i], &v2->array->data[i] );
    if( res != 0 ) return res;
  }
}


//================================================================
/*! get min, max value

  @param  ary		pointer to target value
  @param  pp_min_value	returns minimum mrb_value
  @param  pp_max_value	returns maxmum mrb_value
*/
void mrbc_array_minmax(mrb_value *ary, mrb_value **pp_min_value, mrb_value **pp_max_value)
{
  mrb_array *h = ary->array;

  if( h->n_stored == 0 ) {
    *pp_min_value = NULL;
    *pp_max_value = NULL;
    return;
  }

  mrb_value *p_min_value = h->data;
  mrb_value *p_max_value = h->data;

  int i;
  for( i = 1; i < h->n_stored; i++ ) {
    if( mrbc_compare( &h->data[i], p_min_value ) < 0 ) {
      p_min_value = &h->data[i];
    }
    if( mrbc_compare( &h->data[i], p_max_value ) > 0 ) {
      p_max_value = &h->data[i];
    }
  }

  *pp_min_value = p_min_value;
  *pp_max_value = p_max_value;
}


//================================================================
/*! method new
*/
static void c_array_new(mrb_vm *vm, mrb_value v[], int argc)
{
  /*
    in case of new()
  */
  if( argc == 0 ) {
    mrb_value ret = mrbc_array_new(vm, 0);
    if( ret.array == NULL ) return;		// ENOMEM

    SET_RETURN(ret);
    return;
  }

  /*
    in case of new(num)
  */
  if( argc == 1 && v[1].tt == MRB_TT_FIXNUM && v[1].i >= 0 ) {
    mrb_value ret = mrbc_array_new(vm, v[1].i);
    if( ret.array == NULL ) return;		// ENOMEM

    mrb_value nil = mrb_nil_value();
    if( v[1].i > 0 ) {
      mrbc_array_set(&ret, v[1].i - 1, &nil);
    }
    SET_RETURN(ret);
    return;
  }

  /*
    in case of new(num, value)
  */
  if( argc == 2 && v[1].tt == MRB_TT_FIXNUM && v[1].i >= 0 ) {
    mrb_value ret = mrbc_array_new(vm, v[1].i);
    if( ret.array == NULL ) return;		// ENOMEM

    int i;
    for( i = 0; i < v[1].i; i++ ) {
      mrbc_dup(&v[2]);
      mrbc_array_set(&ret, i, &v[2]);
    }
    SET_RETURN(ret);
    return;
  }

  /*
    other case
  */
  console_print( "ArgumentError\n" );	// raise?
}


//================================================================
/*! (operator) +
*/
static void c_array_add(mrb_vm *vm, mrb_value v[], int argc)
{
  if( GET_TT_ARG(1) != MRB_TT_ARRAY ) {
    console_print( "TypeError\n" );	// raise?
    return;
  }

  mrb_array *h1 = v[0].array;
  mrb_array *h2 = v[1].array;

  mrb_value value = mrbc_array_new(vm, h1->n_stored + h2->n_stored);
  if( value.array == NULL ) return;		// ENOMEM

  memcpy( value.array->data,                h1->data,
	  sizeof(mrb_value) * h1->n_stored );
  memcpy( value.array->data + h1->n_stored, h2->data,
	  sizeof(mrb_value) * h2->n_stored );
  value.array->n_stored = h1->n_stored + h2->n_stored;

  mrb_value *p1 = value.array->data;
  const mrb_value *p2 = p1 + value.array->n_stored;
  while( p1 < p2 ) {
    mrbc_dup(p1++);
  }

  mrbc_release(v+1);
  SET_RETURN(value);
}


//================================================================
/*! (operator) []
*/
static void c_array_get(mrb_vm *vm, mrb_value v[], int argc)
{
  /*
    in case of self[nth] -> object | nil
  */
  if( argc == 1 && v[1].tt == MRB_TT_FIXNUM ) {
    mrb_value ret = mrbc_array_get(v, v[1].i);
    mrbc_dup(&ret);
    SET_RETURN(ret);
    return;
  }

  /*
    in case of self[start, length] -> Array | nil
  */
  if( argc == 2 && v[1].tt == MRB_TT_FIXNUM && v[2].tt == MRB_TT_FIXNUM ) {
    int len = mrbc_array_size(&v[0]);
    int idx = v[1].i;
    if( idx < 0 ) idx += len;
    if( idx < 0 ) goto RETURN_NIL;

    int size = (v[2].i < (len - idx)) ? v[2].i : (len - idx);
					// min( v[2].i, (len - idx) )
    if( size < 0 ) goto RETURN_NIL;

    mrb_value ret = mrbc_array_new(vm, size);
    if( ret.array == NULL ) return;		// ENOMEM

    int i;
    for( i = 0; i < size; i++ ) {
      mrb_value val = mrbc_array_get(v, v[1].i + i);
      mrbc_dup(&val);
      mrbc_array_push(&ret, &val);
    }

    SET_RETURN(ret);
    return;
  }

  /*
    other case
  */
  console_print( "Not support such case in Array#[].\n" );
  return;

 RETURN_NIL:
  SET_NIL_RETURN();
}


//================================================================
/*! (operator) []=
*/
static void c_array_set(mrb_vm *vm, mrb_value v[], int argc)
{
  /*
    in case of self[nth] = val
  */
  if( argc == 2 && v[1].tt == MRB_TT_FIXNUM ) {
    mrbc_array_set(v, v[1].i, &v[2]);	// raise? IndexError or ENOMEM
    v[2].tt = MRB_TT_EMPTY;
    return;
  }

  /*
    in case of self[start, length] = val
  */
  if( argc == 3 && v[1].tt == MRB_TT_FIXNUM && v[2].tt == MRB_TT_FIXNUM ) {
    // TODO: not implement yet.
  }

  /*
    other case
  */
  console_print( "Not support such case in Array#[].\n" );
}


//================================================================
/*! (method) clear
*/
static void c_array_clear(mrb_vm *vm, mrb_value v[], int argc)
{
  mrbc_array_clear(v);
}


//================================================================
/*! (method) delete_at
*/
static void c_array_delete_at(mrb_vm *vm, mrb_value v[], int argc)
{
  mrb_value val = mrbc_array_remove(v, GET_INT_ARG(1));
  SET_RETURN(val);
}


//================================================================
/*! (method) empty?
*/
static void c_array_empty(mrb_vm *vm, mrb_value v[], int argc)
{
  int n = mrbc_array_size(v);

  if( n ) {
    SET_FALSE_RETURN();
  } else {
    SET_TRUE_RETURN();
  }
}


//================================================================
/*! (method) size,length,count
*/
static void c_array_size(mrb_vm *vm, mrb_value v[], int argc)
{
  int n = mrbc_array_size(v);

  SET_INT_RETURN(n);
}


//================================================================
/*! (method) index
*/
static void c_array_index(mrb_vm *vm, mrb_value v[], int argc)
{
  mrb_value *value = &GET_ARG(1);
  mrb_value *data = v->array->data;
  int n = v->array->n_stored;
  int i;

  for( i = 0; i < n; i++ ) {
    if( mrbc_compare(&data[i], value) == 0 ) break;
  }

  if( i < n ) {
    SET_INT_RETURN(i);
  } else {
    SET_NIL_RETURN();
  }
}


//================================================================
/*! (method) first
*/
static void c_array_first(mrb_vm *vm, mrb_value v[], int argc)
{
  mrb_value val = mrbc_array_get(v, 0);
  mrbc_dup(&val);
  SET_RETURN(val);
}


//================================================================
/*! (method) last
*/
static void c_array_last(mrb_vm *vm, mrb_value v[], int argc)
{
  mrb_value val = mrbc_array_get(v, -1);
  mrbc_dup(&val);
  SET_RETURN(val);
}


//================================================================
/*! (method) push
*/
static void c_array_push(mrb_vm *vm, mrb_value v[], int argc)
{
  mrbc_array_push(&v[0], &v[1]);	// raise? ENOMEM
  v[1].tt = MRB_TT_EMPTY;
}


//================================================================
/*! (method) pop
*/
static void c_array_pop(mrb_vm *vm, mrb_value v[], int argc)
{
  /*
    in case of pop() -> object | nil
  */
  if( argc == 0 ) {
    mrb_value val = mrbc_array_pop(v);
    SET_RETURN(val);
    return;
  }

  /*
    in case of pop(n) -> Array
  */
  if( argc == 1 && v[1].tt == MRB_TT_FIXNUM ) {
    // TODO: not implement yet.
  }

  /*
    other case
  */
  console_print( "Not support such case in Array#pop.\n" );
}


//================================================================
/*! (method) unshift
*/
static void c_array_unshift(mrb_vm *vm, mrb_value v[], int argc)
{
  mrbc_array_unshift(&v[0], &v[1]);	// raise? IndexError or ENOMEM
  v[1].tt = MRB_TT_EMPTY;
}


//================================================================
/*! (method) shift
*/
static void c_array_shift(mrb_vm *vm, mrb_value v[], int argc)
{
  /*
    in case of pop() -> object | nil
  */
  if( argc == 0 ) {
    mrb_value val = mrbc_array_shift(v);
    SET_RETURN(val);
    return;
  }

  /*
    in case of pop(n) -> Array
  */
  if( argc == 1 && v[1].tt == MRB_TT_FIXNUM ) {
    // TODO: not implement yet.
  }

  /*
    other case
  */
  console_print( "Not support such case in Array#shift.\n" );
}


//================================================================
/*! (method) dup
*/
static void c_array_dup(mrb_vm *vm, mrb_value v[], int argc)
{
  mrb_array *h = v[0].array;

  mrb_value value = mrbc_array_new(vm, h->n_stored);
  if( value.array == NULL ) return;		// ENOMEM

  memcpy( value.array->data, h->data, sizeof(mrb_value) * h->n_stored );
  value.array->n_stored = h->n_stored;

  mrb_value *p1 = value.array->data;
  const mrb_value *p2 = p1 + value.array->n_stored;
  while( p1 < p2 ) {
    mrbc_dup(p1++);
  }

  SET_RETURN(value);
}



//================================================================
/*! (method) each
*/
static void c_array_each(mrb_vm *vm, mrb_value v[], int argc)
{
  uint32_t code[2] = {
    MKOPCODE(OP_CALL) | MKARG_A(argc),
    MKOPCODE(OP_ABORT)
  };
  mrb_irep irep = {
    0,     // nlocals
    0,     // nregs
    0,     // rlen
    2,     // ilen
    0,     // plen
    (uint8_t *)code,   // iseq
    NULL,  // pools
    NULL,  // ptr_to_sym
    NULL,  // reps
  };

  // array size
  int n = v[0].array->n_stored;

  mrbc_push_callinfo(vm, 0);

  // adjust reg_top for reg[0]==Proc
  vm->current_regs += v - vm->regs + 1;

  int i;
  for( i=0 ; i<n ; i++ ){
    // set index
    mrbc_release( &v[2] );
    v[2] = mrbc_array_get(v, i);
    mrbc_dup( &v[2] );

    // set OP_CALL irep
    vm->pc = 0;
    vm->pc_irep = &irep;

    // execute OP_CALL
    mrbc_vm_run(vm);
  }

  mrbc_pop_callinfo(vm);
}


//================================================================
/*! (method) min
*/
static void c_array_min(mrb_vm *vm, mrb_value v[], int argc)
{
  // Subset of Array#min, not support min(n).

  mrb_value *p_min_value, *p_max_value;

  mrbc_array_minmax(&v[0], &p_min_value, &p_max_value);
  if( p_min_value == NULL ) {
    SET_NIL_RETURN();
    return;
  }

  mrbc_dup(p_min_value);
  SET_RETURN(*p_min_value);
}


//================================================================
/*! (method) max
*/
static void c_array_max(mrb_vm *vm, mrb_value v[], int argc)
{
  // Subset of Array#max, not support max(n).

  mrb_value *p_min_value, *p_max_value;

  mrbc_array_minmax(&v[0], &p_min_value, &p_max_value);
  if( p_max_value == NULL ) {
    SET_NIL_RETURN();
    return;
  }

  mrbc_dup(p_max_value);
  SET_RETURN(*p_max_value);
}


//================================================================
/*! (method) minmax
*/
static void c_array_minmax(mrb_vm *vm, mrb_value v[], int argc)
{
  // Subset of Array#minmax, not support minmax(n).

  mrb_value *p_min_value, *p_max_value;
  mrb_value nil = mrb_nil_value();
  mrb_value ret = mrbc_array_new(vm, 2);

  mrbc_array_minmax(&v[0], &p_min_value, &p_max_value);
  if( p_min_value == NULL ) p_min_value = &nil;
  if( p_max_value == NULL ) p_max_value = &nil;

  mrbc_dup(p_min_value);
  mrbc_dup(p_max_value);
  mrbc_array_set(&ret, 0, p_min_value);
  mrbc_array_set(&ret, 1, p_max_value);

  SET_RETURN(ret);
}



//================================================================
/*! initialize
*/
void mrbc_init_class_array(struct VM *vm)
{
  mrbc_class_array = mrbc_define_class(vm, "Array", mrbc_class_object);

  mrbc_define_method(vm, mrbc_class_array, "new", c_array_new);
  mrbc_define_method(vm, mrbc_class_array, "+", c_array_add);
  mrbc_define_method(vm, mrbc_class_array, "[]", c_array_get);
  mrbc_define_method(vm, mrbc_class_array, "at", c_array_get);
  mrbc_define_method(vm, mrbc_class_array, "[]=", c_array_set);
  mrbc_define_method(vm, mrbc_class_array, "<<", c_array_push);
  mrbc_define_method(vm, mrbc_class_array, "clear", c_array_clear);
  mrbc_define_method(vm, mrbc_class_array, "delete_at", c_array_delete_at);
  mrbc_define_method(vm, mrbc_class_array, "empty?", c_array_empty);
  mrbc_define_method(vm, mrbc_class_array, "size", c_array_size);
  mrbc_define_method(vm, mrbc_class_array, "length", c_array_size);
  mrbc_define_method(vm, mrbc_class_array, "count", c_array_size);
  mrbc_define_method(vm, mrbc_class_array, "index", c_array_index);
  mrbc_define_method(vm, mrbc_class_array, "first", c_array_first);
  mrbc_define_method(vm, mrbc_class_array, "last", c_array_last);
  mrbc_define_method(vm, mrbc_class_array, "push", c_array_push);
  mrbc_define_method(vm, mrbc_class_array, "pop", c_array_pop);
  mrbc_define_method(vm, mrbc_class_array, "shift", c_array_shift);
  mrbc_define_method(vm, mrbc_class_array, "unshift", c_array_unshift);
  mrbc_define_method(vm, mrbc_class_array, "dup", c_array_dup);
  mrbc_define_method(vm, mrbc_class_array, "each", c_array_each);
  mrbc_define_method(vm, mrbc_class_array, "min", c_array_min);
  mrbc_define_method(vm, mrbc_class_array, "max", c_array_max);
  mrbc_define_method(vm, mrbc_class_array, "minmax", c_array_minmax);
}
