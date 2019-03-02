/*! @file
  @brief


  <pre>
  Copyright (C) 2015 Kyushu Institute of Technology.
  Copyright (C) 2015 Shimane IT Open-Innovation Center.

  This file is distributed under BSD 3-Clause License.


  </pre>
*/

#ifndef MRBC_SRC_OPCODE_H_
#define MRBC_SRC_OPCODE_H_

#ifdef __cplusplus
extern "C" {
#endif


#define GET_OPCODE(code)            ((code) & 0x7f)
#define GETARG_A(code)              (((code) >> 23) & 0x1ff)
#define GETARG_B(code)              (((code) >> 14) & 0x1ff)
#define GETARG_C(code)              (((code) >>  7) & 0x7f)
#define GETARG_Ax(code)             (((code) >>  7) & 0x1ffffff)

#define GETARG_Bz(code)              GETARG_UNPACK_b(code,14,2)

#define GETARG_UNPACK_b(i,n1,n2)    ((((code)) >> (7+(n2))) & (((1<<(n1))-1)))

#define MKOPCODE(op)                ((op & 0x7f)<<24)
#define MKARG_A(c)                  ((c & 0xff)<<1 | (c & 0x01)>>8)
#define MKARG_B(c)                  ((c & 0x1fc)<<6 | (c & 0x03)<<22)
#define MKARG_C(c)                  ((c & 0x7e)<<15 | (c & 0x01)<<31)


#define MAXARG_Bx                   (0xffff)
#define MAXARG_sBx                  (MAXARG_Bx>>1)
#define GETARG_Bx(code)             (((code) >>  7) & 0xffff)
#define GETARG_sBx(code)            (GETARG_Bx(code)-MAXARG_sBx)
#define GETARG_C(code)              (((code) >>  7) & 0x7f)


//================================================================
/*!@brief

*/
enum OPCODE {
  OP_NOP       = 0x00,
  OP_MOVE      = 0x01,
  OP_LOADL     = 0x02,
  OP_LOADI     = 0x03,
  OP_LOADSYM   = 0x04,
  OP_LOADNIL   = 0x05,
  OP_LOADSELF  = 0x06,
  OP_LOADT     = 0x07,
  OP_LOADF     = 0x08,
  OP_GETGLOBAL = 0x09,
  OP_SETGLOBAL = 0x0a,

  OP_GETIV     = 0x0d,
  OP_SETIV     = 0x0e,

  OP_GETCONST  = 0x11,
  OP_SETCONST  = 0x12,

  OP_GETUPVAR  = 0x15,
  OP_SETUPVAR  = 0x16,
  OP_JMP       = 0x17,
  OP_JMPIF     = 0x18,
  OP_JMPNOT    = 0x19,
  OP_SEND      = 0x20,
  OP_SENDB     = 0x21,

  OP_CALL      = 0x23,

  OP_ENTER     = 0x26,

  OP_RETURN    = 0x29,

  OP_BLKPUSH   = 0x2b,
  OP_ADD       = 0x2c,
  OP_ADDI      = 0x2d,
  OP_SUB       = 0x2e,
  OP_SUBI      = 0x2f,
  OP_MUL       = 0x30,
  OP_DIV       = 0x31,
  OP_EQ        = 0x32,
  OP_LT        = 0x33,
  OP_LE        = 0x34,
  OP_GT        = 0x35,
  OP_GE        = 0x36,
  OP_ARRAY     = 0x37,

  OP_STRING    = 0x3d,
  OP_STRCAT    = 0x3e,
  OP_HASH      = 0x3f,
  OP_LAMBDA    = 0x40,
  OP_RANGE     = 0x41,

  OP_CLASS     = 0x43,

  OP_EXEC      = 0x45,
  OP_METHOD    = 0x46,

  OP_TCLASS    = 0x48,

  OP_STOP      = 0x4a,

  OP_ABORT     = 0x50,  // using OP_ABORT inside mruby/c only
};

#ifdef __cplusplus
}
#endif
#endif
