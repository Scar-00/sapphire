#ifndef _SAPPHIRE_CORE_H_
#define _SAPPHIRE_CORE_H_

#if defined (_WIN32)
    #define SP_OS_WIN
#else
    #define SP_OS_UNIX
#endif

//#define SP_OS_OSX

#if defined(DLL_BUILD) && defined(SP_OS_WIN)
    #define SAPPHIRE_API __declspec(dllexport)
    #define SAPPHIRE_PATH_SEPERATOR \\
#elif defined(DLL_USE) && defined(SP_OS_WIN)
    #define SAPPHIRE_API __declspec(dllimport)
#else
    #define SAPPHIRE_PATH_SEPERATOR /
    #define SAPPHIRE_API
#endif


#define BIT(x) (1 << x)

typedef double              f64;
typedef float               f32;
typedef long long int       s64;
typedef int                 s32;
typedef short int           s16;
typedef signed char         s8;

//unsigned types
typedef unsigned long long  u64;
typedef unsigned int        u32;
typedef unsigned short      u16;
typedef unsigned char       u8;

#include <stdint.h>
#include <inttypes.h>
#include <stddef.h>

typedef uint64_t SPID;

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
