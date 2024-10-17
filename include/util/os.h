#ifndef _SAPPHIRE_OS_H_
#define _SAPPHIRE_OS_H_

#include "../sp_core.h"

typedef enum Color {
    DARK_BLUE = 1,
    GREEN,
    CYAN,
    RED,
    PURPLE,
    YELLOW,
    LIGHT_GREY,
    GREY,
    BLUE,
    LIGHT_GREEN,
    LIGHT_BLUE,
    LIGHT_RED,
    LIGHT_PURPLE,
    LIGHT_YELLOW,
    WHITE,
    DEFAULT = LIGHT_GREY,
}Color;

SAPPHIRE_API const char *sp_create_os_path(void);
SAPPHIRE_API void sp_set_os_term_color(Color c);


#endif // _SAPPHIRE_OS_H_
