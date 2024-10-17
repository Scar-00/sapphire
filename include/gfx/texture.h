#ifndef _SAPPHIRE_TEXTURE_H_
#define _SAPPHIRE_TEXTURE_H_

#include "../sp_core.h"

typedef struct SPTexture SPTexture;

SAPPHIRE_API SPTexture *sp_texture_from_pixels(u8 *pixels, size_t width, size_t height);
SAPPHIRE_API SPTexture *sp_texture_from_font(u8 *pixels, size_t width, size_t height);
SAPPHIRE_API SPTexture *sp_texture_from_path(const char *path);
SAPPHIRE_API void sp_texture_destroy(SPTexture *self);

SAPPHIRE_API u32 sp_texture_get_handle(SPTexture const *self);
SAPPHIRE_API s32 sp_texture_get_width(SPTexture const *self);
SAPPHIRE_API s32 sp_texture_get_height(SPTexture const *self);

#endif
