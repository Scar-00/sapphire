#ifndef _SAPPHIRE_FRAMEBUFFER_H_
#define _SAPPHIRE_FRAMEBUFFER_H_

#include "../sp_core.h"

#include <stdbool.h>

typedef enum SPFrameBufferSpecType {
    SP_FRAMEBUFFER_RGBA8,
    SP_FRAMEBUFFER_DEPTH_24_STENICIL_8,
    SP_FRAMEBUFFER_DEPTH = SP_FRAMEBUFFER_DEPTH_24_STENICIL_8,
}SPFrameBufferSpecType;

typedef struct SPFrameBufferSpec {
    u32 width, height;
    SPFrameBufferSpecType *type;
}SPFrameBufferSpec;

#define sp_framebuffer_spec_init(list) (sp_framebuffer_spec_init_expl(SP_ARRSIZE(list), list))
SAPPHIRE_API SPFrameBufferSpec sp_framebuffer_spec_init_expl(u32 count, SPFrameBufferSpecType *types);

typedef struct SPFrameBuffer SPFrameBuffer;

SAPPHIRE_API SPFrameBuffer sp_framebuffer_init(SPFrameBufferSpec spec);
SAPPHIRE_API void sp_framebuffer_invalidate(SPFrameBuffer *self);
SAPPHIRE_API u32 sp_framebuffer_get_color_attachment(SPFrameBuffer self, u32 index);

SAPPHIRE_API void sp_framebuffer_bind(SPFrameBuffer self);
SAPPHIRE_API void sp_framebuffer_unbind(SPFrameBuffer self);

#endif
