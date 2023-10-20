#ifndef _SAPPHIRE_H_
#define _SAPPHIRE_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "util/os.h"
#include "util/math.h"
#include "util/cglm.h"
#include "util/font.h"
#include "gfx/window.h"
#include "gfx/renderer.h"
#include "gfx/camera.h"
#include "gfx/texture.h"
#include "gfx/framebuffer.h"

typedef enum SPKey {
    SP_KEY_SPACE              = 32,
    SP_KEY_APOSTROPHE         = 39,  /* ' */
    SP_KEY_COMMA              = 44,  /* , */
    SP_KEY_MINUS              = 45,  /* - */
    SP_KEY_PERIOD             = 46,  /* . */
    SP_KEY_SLASH              = 47,  /* / */
    SP_KEY_0                  = 48,
    SP_KEY_1                  = 49,
    SP_KEY_2                  = 50,
    SP_KEY_3                  = 51,
    SP_KEY_4                  = 52,
    SP_KEY_5                  = 53,
    SP_KEY_6                  = 54,
    SP_KEY_7                  = 55,
    SP_KEY_8                  = 56,
    SP_KEY_9                  = 57,
    SP_KEY_SEMICOLON          = 59,  /* ; */
    SP_KEY_EQUAL              = 61,  /* = */
    SP_KEY_A                  = 65,
    SP_KEY_B                  = 66,
    SP_KEY_C                  = 67,
    SP_KEY_D                  = 68,
    SP_KEY_E                  = 69,
    SP_KEY_F                  = 70,
    SP_KEY_G                  = 71,
    SP_KEY_H                  = 72,
    SP_KEY_I                  = 73,
    SP_KEY_J                  = 74,
    SP_KEY_K                  = 75,
    SP_KEY_L                  = 76,
    SP_KEY_M                  = 77,
    SP_KEY_N                  = 78,
    SP_KEY_O                  = 79,
    SP_KEY_P                  = 80,
    SP_KEY_Q                  = 81,
    SP_KEY_R                  = 82,
    SP_KEY_S                  = 83,
    SP_KEY_T                  = 84,
    SP_KEY_U                  = 85,
    SP_KEY_V                  = 86,
    SP_KEY_W                  = 87,
    SP_KEY_X                  = 88,
    SP_KEY_Y                  = 89,
    SP_KEY_Z                  = 90,
    SP_KEY_LEFT_BRACKET       = 91,  /* [ */
    SP_KEY_BACKSLASH          = 92,  /* \ */
    SP_KEY_RIGHT_BRACKET      = 93,  /* ] */
    SP_KEY_GRAVE_ACCENT       = 96,  /* ` */
    SP_KEY_WORLD_1            = 161, /* non-US #1 */
    SP_KEY_WORLD_2            = 162, /* non-US #2 */

/* Function keys */
    SP_KEY_ESCAPE             = 256,
    SP_KEY_ENTER              = 257,
    SP_KEY_TAB                = 258,
    SP_KEY_BACKSPACE          = 259,
    SP_KEY_INSERT             = 260,
    SP_KEY_DELETE             = 261,
    SP_KEY_RIGHT              = 262,
    SP_KEY_LEFT               = 263,
    SP_KEY_DOWN               = 264,
    SP_KEY_UP                 = 265,
    SP_KEY_PAGE_UP            = 266,
    SP_KEY_PAGE_DOWN          = 267,
    SP_KEY_HOME               = 268,
    SP_KEY_END                = 269,
    SP_KEY_CAPS_LOCK          = 280,
    SP_KEY_SCROLL_LOCK        = 281,
    SP_KEY_NUM_LOCK           = 282,
    SP_KEY_PRINT_SCREEN       = 283,
    SP_KEY_PAUSE              = 284,
    SP_KEY_F1                 = 290,
    SP_KEY_F2                 = 291,
    SP_KEY_F3                 = 292,
    SP_KEY_F4                 = 293,
    SP_KEY_F5                 = 294,
    SP_KEY_F6                 = 295,
    SP_KEY_F7                 = 296,
    SP_KEY_F8                 = 297,
    SP_KEY_F9                 = 298,
    SP_KEY_F10                = 299,
    SP_KEY_F11                = 300,
    SP_KEY_F12                = 301,
    SP_KEY_F13                = 302,
    SP_KEY_F14                = 303,
    SP_KEY_F15                = 304,
    SP_KEY_F16                = 305,
    SP_KEY_F17                = 306,
    SP_KEY_F18                = 307,
    SP_KEY_F19                = 308,
    SP_KEY_F20                = 309,
    SP_KEY_F21                = 310,
    SP_KEY_F22                = 311,
    SP_KEY_F23                = 312,
    SP_KEY_F24                = 313,
    SP_KEY_F25                = 314,
    SP_KEY_KP_0               = 320,
    SP_KEY_KP_1               = 321,
    SP_KEY_KP_2               = 322,
    SP_KEY_KP_3               = 323,
    SP_KEY_KP_4               = 324,
    SP_KEY_KP_5               = 325,
    SP_KEY_KP_6               = 326,
    SP_KEY_KP_7               = 327,
    SP_KEY_KP_8               = 328,
    SP_KEY_KP_9               = 329,
    SP_KEY_KP_DECIMAL         = 330,
    SP_KEY_KP_DIVIDE          = 331,
    SP_KEY_KP_MULTIPLY        = 332,
    SP_KEY_KP_SUBTRACT        = 333,
    SP_KEY_KP_ADD             = 334,
    SP_KEY_KP_ENTER           = 335,
    SP_KEY_KP_EQUAL           = 336,
    SP_KEY_LEFT_SHIFT         = 340,
    SP_KEY_LEFT_CONTROL       = 341,
    SP_KEY_LEFT_ALT           = 342,
    SP_KEY_LEFT_SUPER         = 343,
    SP_KEY_RIGHT_SHIFT        = 344,
    SP_KEY_RIGHT_CONTROL      = 345,
    SP_KEY_RIGHT_ALT          = 346,
    SP_KEY_RIGHT_SUPER        = 347,
    SP_KEY_MENU               = 348,

    SP_KEY_LAST               = SP_KEY_MENU,
}SPKey;

#ifdef __cplusplus
}
#endif
#endif
