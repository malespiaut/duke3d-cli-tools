/* SPDX-License-Identifier: MIT */
/**
 * @file   types.h
 * @author Marc-Alexandre Espiaut <ma.dev@espiaut.fr>
 * @date   2026-03-03
 * @brief  Custom types.
 *
 * @copyright Copyright (c) 2026 Marc-Alexandre Espiaut
 */

#ifndef TYPES_H
#define TYPES_H

#include <stddef.h> /* size_t, ptrdiff_t */

typedef signed char i8;
typedef unsigned char u8;
typedef signed short i16;
typedef unsigned short u16;
typedef signed int i32;
typedef unsigned int u32;

#if defined(_MSC_VER)
typedef signed __int64 i64;
typedef unsigned __int64 u64;
#else
typedef signed long long i64;
typedef unsigned long long u64;
#endif

typedef float f32;
typedef double f64;

typedef ptrdiff_t isize;
typedef size_t usize;
typedef unsigned long uptr; /* not perfect; best C89 can do */

/* Compile time tests. */
/* These will trigger a compilation error
 * if the types don't match the expected sizes */
typedef char assert_u8_size[(sizeof(u8) == 1) ? 1 : -1];
typedef char assert_u16_size[(sizeof(u16) == 2) ? 1 : -1];
typedef char assert_u32_size[(sizeof(u32) == 4) ? 1 : -1];
typedef char assert_u64_size[(sizeof(u64) == 8) ? 1 : -1];

typedef struct vec2_i32_s vec2_i32_t;
struct vec2_i32_s
{
  i32 x;
  i32 y;
};

typedef struct vec2_i8_s vec2_i8_t;
struct vec2_i8_s
{
  i8 x;
  i8 y;
};

typedef struct vec2_u8_s vec2_u8_t;
struct vec2_u8_s
{
  u8 x;
  u8 y;
};

typedef struct vec3_i16_s vec3_i16_t;
struct vec3_i16_s
{
  i16 x;
  i16 y;
  i16 z;
};

typedef struct vec3_i32_s vec3_i32_t;
struct vec3_i32_s
{
  i32 x;
  i32 y;
  i32 z;
};

#endif /* TYPES_H */
