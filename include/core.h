#ifndef _SAPPHIRE_CORE_H_
#define _SAPPHIRE_CORE_H_

#if defined(DLL_BUILD) && defined(_WIN32)
    #define SAPPHIRE_API __declspec(dllexport)
    #define SAPPHIRE_PATH_SEPERATOR \\
#elif defined(DLL_USE) && defined(_WIN32)
    #define SAPPHIRE_API __declspec(dllimport)
#else
    #define SAPPHIRE_PATH_SEPERATOR /
    #define SAPPHIRE_API
#endif

#define BIT(x) (1 << x)

#include <stdint.h>
#include <inttypes.h>

typedef uint8_t u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef uint64_t u64;

typedef int8_t s8;
typedef int16_t s16;
typedef int32_t s32;
typedef int64_t s64;

typedef float f32;
typedef double f64;

typedef u64 SPID;

#define SP_ARRSIZE(a) ((s32)sizeof(a) / sizeof(*a))

// Mosue buttons
#define SP_MOUSE_BUTTON_1 0
#define SP_MOUSE_BUTTON_2 1
#define SP_MOUSE_BUTTON_3 2
#define SP_MOUSE_BUTTON_4 3
#define SP_MOUSE_BUTTON_5 4
#define SP_MOUSE_BUTTON_6 5
#define SP_MOUSE_BUTTON_7 6
#define SP_MOUSE_BUTTON_8 7
#define SP_MOUSE_BUTTON_9 8
#define SP_MOUSE_BUTTON_LEFT SP_MOUSE_BUTTON_1
#define SP_MOUSE_BUTTON_RIGHT SP_MOUSE_BUTTON_2
#define SP_MOUSE_BUTTON_MIDDLE SP_MOUSE_BUTTON_3


#endif
