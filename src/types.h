#pragma once
typedef unsigned char u8;
typedef unsigned short u16;
typedef unsigned int u32;
typedef unsigned long long u64;

typedef signed char i8;
typedef signed short i16;
typedef signed int i32;
typedef signed long long i64;

typedef float f32;
typedef double f64;

typedef char b8;

#if defined(__clang__) || defined(__gcc__)
#define STATIC_ASSERT _Static_assert
#else
#define STATIC_ASSERT static_assert
#endif

// STATIC_ASSERT(sizeof(u8) == 1, "expected u8 to be 1 byte");
// STATIC_ASSERT(sizeof(u16) == 2, "expected u16 to be 2 bytes");
// STATIC_ASSERT(sizeof(u32) == 4, "expected u32 to be 4 bytes");
// STATIC_ASSERT(sizeof(u64) == 8, "expected u64 to be 8 bytes");


// STATIC_ASSERT(sizeof(i8) == 1, "expected i8 to be 1 byte");
// STATIC_ASSERT(sizeof(i16) == 2, "expected i16 to be 2 bytes");
// STATIC_ASSERT(sizeof(i32) == 4, "expected i32 to be 4 bytes");
// STATIC_ASSERT(sizeof(i64) == 8, "expected i64 to be 8 bytes");

// STATIC_ASSERT(sizeof(f32) == 4, "expected f32 to be 4 bytes");
// STATIC_ASSERT(sizeof(f64) == 8, "expected f64 to be 8 bytes");

#define TRUE 1
#define FALSE 0