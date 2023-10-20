#ifndef _SAPPHIRE_RENDERER_H_
#define _SAPPHIRE_RENDERER_H_

#include "../core.h"
#include "gfx/camera.h"
#include "texture.h"

SAPPHIRE_API void sp_renderer_init(void);
SAPPHIRE_API void sp_renderer_shutdown(void);

//maybe change this API to let flush() and begin() take in a camera instead [-> sp_batch_begin(SPCamera *); | sp_renderer_flush(SPCamera *);]
SAPPHIRE_API void sp_renderer_camera_set(SPCamera *camera);
SAPPHIRE_API void sp_renderer_clear_color_set(f32 r, f32 g, f32 b, f32 a);
SAPPHIRE_API void sp_batch_begin(void);
SAPPHIRE_API void sp_renderer_flush(void);
SAPPHIRE_API void sp_batch_end(void);

// 2D rendering
SAPPHIRE_API void sp_render_quad(f32 x, f32 y, f32 width, f32 height, SPTexture *tex, u32 color_hex);
SAPPHIRE_API void sp_render_quad_rotated(f32 x, f32 y, f32 width, f32 height, f32 rotation, SPTexture *tex, u32 color_hex);
SAPPHIRE_API void sp_render_text(f32 x, f32 y, const char *format, ...);

// 3D rendering
SAPPHIRE_API void sp_render_cube(f32 x, f32 y, f32 z, f32 width, f32 height, f32 depth, SPTexture *tex, u32 color_hex);

typedef struct RendererInfo{
    u32 draw_calls;
}RendererInfo;
#ifdef DSP_DEBUG
SAPPHIRE_API RendererInfo sp_renderer_info_get(void);
#endif

#endif
