#if defined(cglm_h) && defined(cglm_structs_h)
#ifndef _SAPPHIRE_CGLM_H_
#define _SAPPHIRE_CGLM_H_

#include "../core.h"

SAPPHIRE_API vec2s sp_mouse_pos(void);

SAPPHIRE_API void sp_render_quad_v2(vec2s pos, vec2s size, SPTexture *tex, u32 color_hex);
SAPPHIRE_API void sp_render_quad_rotated_v2(vec2s pos, vec2s size, f32 rotation, SPTexture *tex, u32 color_hex);

SAPPHIRE_API void sp_render_cube_v3(vec3s pos, vec3s dims, SPTexture *tex, u32 color_hex);

#endif
#endif
