//-------------------------------------------------------------------------------------------------
// Project Cormorant - GB / CGB Emulator
// jcds (jdibenes@outlook.com)
// 2011
//-------------------------------------------------------------------------------------------------

#pragma once

#include <stdint.h>

typedef uint8_t  u8;
typedef uint16_t u16;
typedef uint32_t u32;

typedef int8_t  s8;
typedef int16_t s16;
typedef int32_t s32;

union v16 {struct {u8  lo, hi;} b; u16 w; s16 s;};
union v32 {struct {v16 lo, hi;} w; u32 d; s32 i;};

#define BIT_TEST(v, b)     (v & (1U << b))
#define BIT_FIELD(v, b, m) ((v >> b) & m)
