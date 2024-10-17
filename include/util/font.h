#ifndef _SAPPHIRE_FONT_H_
#define _SAPPHIRE_FONT_H_

#include "../sp_core.h"
#include <stddef.h>

typedef struct SPFont SPFont;

SAPPHIRE_API SPFont *sp_font_init_from_file(const char *path, size_t size);
SAPPHIRE_API void sp_font_push(SPFont *font);
SAPPHIRE_API void sp_font_pop(void);
SAPPHIRE_API u32 sp_font_size_get(SPFont *font);

#ifdef SP_DEBUG
SAPPHIRE_API void *sp_font_atlas_get(SPFont *font);
#endif

#endif
