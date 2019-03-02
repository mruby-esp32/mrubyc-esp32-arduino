/*! @file
  @brief
  mruby/c value definitions

  <pre>
  Copyright (C) 2015-2018 Kyushu Institute of Technology.
  Copyright (C) 2015-2018 Shimane IT Open-Innovation Center.

  This file is distributed under BSD 3-Clause License.


  </pre>
*/

#ifndef MRBC_SRC_VALUE_H_
#define MRBC_SRC_VALUE_H_

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif


// mrb types
//typedef float mrb_float;
//typedef int32_t mrb_int;
typedef int16_t mrb_sym;

/* aspec access ? */
#define MRB_ASPEC_REQ(a)          (((a) >> 18) & 0x1f)
#define MRB_ASPEC_OPT(a)          (((a) >> 13) & 0x1f)
#define MRB_ASPEC_REST(a)         (((a) >> 12) & 0x1)
#define MRB_ASPEC_POST(a)         (((a) >> 7) & 0x1f)

#define MRBC_OBJECT_HEADER \
  uint16_t ref_count; \
  mrb_vtype tt : 8  // TODO: for debug use only.


struct VM;
struct IREP;
struct RObject;
typedef void (*mrb_func_t)(struct VM *vm, struct RObject *v, int argc);


//================================================================
/*!@brief
  define the value type.
*/
typedef enum {
  /* internal use */
  MRB_TT_HANDLE = -1,
  /* primitive */
  MRB_TT_EMPTY = 0,
  MRB_TT_NIL,
  MRB_TT_FALSE,		// (note) true/false threshold. see op_jmpif

  MRB_TT_TRUE,
  MRB_TT_FIXNUM,
  MRB_TT_FLOAT,
  MRB_TT_SYMBOL,
  MRB_TT_CLASS,

  /* non-primitive */
  MRB_TT_OBJECT = 20,
  MRB_TT_PROC,
  MRB_TT_ARRAY,
  MRB_TT_STRING,
  MRB_TT_RANGE,
  MRB_TT_HASH,

} mrb_vtype;


//================================================================
/*!@brief
  define the error code. (BETA TEST)
*/
typedef enum {
  E_NOMEMORY_ERROR = 1,
  E_RUNTIME_ERROR,
  E_TYPE_ERROR,
  E_ARGUMENT_ERROR,
  E_INDEX_ERROR,
  E_RANGE_ERROR,
  E_NAME_ERROR,
  E_NOMETHOD_ERROR,
  E_SCRIPT_ERROR,
  E_SYNTAX_ERROR,
  E_LOCALJUMP_ERROR,
  E_REGEXP_ERROR,
  E_NOTIMP_ERROR,
  E_FLOATDOMAIN_ERROR,
  E_KEY_ERROR,
} mrb_error_code;



//================================================================
/*!@brief
  mruby/c value object.
*/
typedef struct RObject {
  mrb_vtype tt : 8;
  union {
    int32_t i;			// MRB_TT_FIXNUM, SYMBOL
#if MRBC_USE_FLOAT
    double d;			// MRB_TT_FLOAT
#endif
    struct RClass *cls;		// MRB_TT_CLASS
    struct RObject *handle;	// handle to objects
    struct RInstance *instance;	// MRB_TT_OBJECT
    struct RProc *proc;		// MRB_TT_PROC
    struct RArray *array;	// MRB_TT_ARRAY
    struct RString *string;	// MRB_TT_STRING
    const char *str;		// C-string (only loader use.)
    struct RRange *range;	// MRB_TT_RANGE
    struct RHash *hash;		// MRB_TT_HASH
  };
} mrb_object;
typedef struct RObject mrb_value;


//================================================================
/*!@brief
  mruby/c class object.
*/
typedef struct RClass {
  mrb_sym sym_id;	// class name
#ifdef MRBC_DEBUG
  const char *names;	// for debug. delete soon.
#endif
  struct RClass *super;	// mrbc_class[super]
  struct RProc *procs;	// mrbc_proc[rprocs], linked list
} mrb_class;


//================================================================
/*!@brief
  mruby/c instance object.
*/
typedef struct RInstance {
  MRBC_OBJECT_HEADER;

  struct RClass *cls;
  struct RKeyValueHandle *ivar;
  uint8_t data[];
} mrb_instance;


//================================================================
/*!@brief
  mruby/c proc object.
*/
typedef struct RProc {
  MRBC_OBJECT_HEADER;

  unsigned int c_func : 1;	// 0:IREP, 1:C Func
  mrb_sym sym_id;
#ifdef MRBC_DEBUG
  const char *names;		// for debug; delete soon
#endif
  struct RProc *next;
  union {
    struct IREP *irep;
    mrb_func_t func;
  };
} mrb_proc;



// for C call
#define SET_INT_RETURN(n)	(mrbc_release(v), v[0].tt=MRB_TT_FIXNUM, v[0].i=(n))
#define SET_NIL_RETURN()	(mrbc_release(v), v[0].tt=MRB_TT_NIL)
#define SET_FLOAT_RETURN(n)	(mrbc_release(v), v[0].tt=MRB_TT_FLOAT, v[0].d=(n))
#define SET_FALSE_RETURN()	(mrbc_release(v), v[0].tt=MRB_TT_FALSE)
#define SET_TRUE_RETURN()	(mrbc_release(v), v[0].tt=MRB_TT_TRUE)
#define SET_RETURN(n)		(mrbc_release(v), v[0]=(n))

#define GET_TT_ARG(n)		(v[(n)].tt)
#define GET_INT_ARG(n)		(v[(n)].i)
#define GET_ARY_ARG(n)		(v[(n)])
#define GET_ARG(n)		(v[(n)])
#define GET_FLOAT_ARG(n)	(v[(n)].d)
#define GET_STRING_ARG(n)	(v[(n)].string->data)



mrb_object *mrbc_obj_alloc(struct VM *vm, mrb_vtype tt);
mrb_proc *mrbc_rproc_alloc(struct VM *vm, const char *name);
int mrbc_compare(const mrb_value *v1, const mrb_value *v2);
void mrbc_dup(mrb_value *v);
void mrbc_release(mrb_value *v);
void mrbc_dec_ref_counter(mrb_value *v);
void mrbc_clear_vm_id(mrb_value *v);
int32_t mrbc_atoi(const char *s, int base);
struct IREP *mrbc_irep_alloc(struct VM *vm);
void mrbc_irep_free(struct IREP *irep);
mrb_value mrbc_instance_new(struct VM *vm, mrb_class *cls, int size);
void mrbc_instance_delete(mrb_value *v);
void mrbc_instance_setiv(mrb_object *obj, mrb_sym sym_id, mrb_value *v);
mrb_value mrbc_instance_getiv(mrb_object *obj, mrb_sym sym_id);



//================================================================
/*!@brief
  Returns a fixnum in mruby/c.

  @param  n	int value
  @return	mrb_value of type fixnum.
*/
static inline mrb_value mrb_fixnum_value( int32_t n )
{
  mrb_value value = {.tt = MRB_TT_FIXNUM};
  value.i = n;
  return value;
}


#if MRBC_USE_FLOAT
//================================================================
/*!@brief
  Returns a float in mruby/c.

  @param  n	dluble value
  @return	mrb_value of type float.
*/
static inline mrb_value mrb_float_value( double n )
{
  mrb_value value = {.tt = MRB_TT_FLOAT};
  value.d = n;
  return value;
}
#endif


//================================================================
/*!@brief
  Returns a nil in mruby/c.

  @return	mrb_value of type nil.
*/
static inline mrb_value mrb_nil_value(void)
{
  mrb_value value = {.tt = MRB_TT_NIL};
  return value;
}


//================================================================
/*!@brief
  Returns a true in mruby/c.

  @return	mrb_value of type true.
*/
static inline mrb_value mrb_true_value(void)
{
  mrb_value value = {.tt = MRB_TT_TRUE};
  return value;
}


//================================================================
/*!@brief
  Returns a false in mruby/c.

  @return	mrb_value of type false.
*/
static inline mrb_value mrb_false_value(void)
{
  mrb_value value = {.tt = MRB_TT_FALSE};
  return value;
}


#ifdef __cplusplus
}
#endif
#endif
