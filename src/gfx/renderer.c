#include "../internal.h"

#include <float.h>
#include <stdarg.h>

#define RENDERER_MAX_VERTECIES 1000 * 40
#define RENDERER_MAX_INDICIES 1000 * 6

void sp_render_prep(void);
void sp_renderer_data_clear(void);
f32 sp_item_check_texture(SPTexture *tex);
vec2s sp_box_calc_pos(f32 x, f32 y, f32 width, f32 height);

typedef struct SPRenderer {
    SPVAO vao;
    SPVBO vbo;
    SPVBO ibo;
    SPShader shader;
    struct SPRendereringData {
        Vec(f32) vertex_buffer;
        Vec(u32) index_buffer;
        SPTexture textures[32];
        u32 texture_count;
        u64 offset;
    }renderering_data;
    SPCamera *camera_current;
    vec4s clear_color;
    Vec(SPFont *) fonts;
    RendererInfo info;
}SPRenderer;

SPRenderer *renderer = NULL;

SAPPHIRE_API void sp_renderer_init(void) {
    renderer = malloc(sizeof(*renderer));
    if(renderer == NULL)
        sp_panic(SP_ERROR_SP, "failed to create renderer", "");

    *renderer = (SPRenderer){
        .vao = sp_vao_create(),
        .vbo = sp_vbo_create(GL_ARRAY_BUFFER, true),
        .ibo = sp_vbo_create(GL_ELEMENT_ARRAY_BUFFER, true),
        .shader = sp_shader_create("sp.vs", "sp.fs", ((SPShaderField[]){
            {SP_SHADER_TYPE_FLOAT3, "position"},
            {SP_SHADER_TYPE_FLOAT2, "uv"},
            {SP_SHADER_TYPE_FLOAT1, "tex_index"},
            {SP_SHADER_TYPE_FLOAT4, "color"},
            {SP_SHADER_TYPE_FLOAT1, "kind"},
        })),
        .renderering_data = {
            .vertex_buffer = vec_with_size(f32, 1),
            .index_buffer = vec_with_size(u32, 1),
        },
        .camera_current = NULL,
        .clear_color = {{1, 1, 1, 1}},
        .fonts = vec_with_size(struct SPFont *, 2),
        .info = {0},
    };
    SPFont *font = sp_font_init_from_file("/usr/share/fonts/TTF/JetBrainsMono-Regular.ttf", 16);
    sp_font_push(font);
}

SAPPHIRE_API void sp_renderer_shutdown(void) {
    sp_vao_destroy(renderer->vao);
    sp_vbo_destroy(renderer->vbo);
    sp_vbo_destroy(renderer->ibo);
    vec_destroy(renderer->renderering_data.vertex_buffer);
    vec_destroy(renderer->renderering_data.index_buffer);
    vec_destroy(renderer->fonts);
    free(renderer);
    renderer = NULL;
}

SAPPHIRE_API void sp_renderer_camera_set(SPCamera *camera) {
    renderer->camera_current = camera;
}

SAPPHIRE_API void sp_batch_begin(void) {
    sp_render_prep();
}

static vec4s QUAD_VERTEX_POS[4] = {
        {{ 0.0,  0.0, 0, 1}},
        {{ 1.0,  0.0, 0, 1}},
        {{ 1.0,  1.0, 0, 1}},
        {{ 0.0,  1.0, 0, 1}}
    };

static vec2s QUAD_UV_CORDS[4] = {
        {{0, 0}},
        {{1, 0}},
        {{1, 1}},
        {{0, 1}}
    };

static vec4s CUBE_VERTEX_POS[8] = {
        {{ 0.0,  0.0, 0.0, 1}},
        {{ 1.0,  0.0, 0.0, 1}},
        {{ 1.0,  1.0, 0.0, 1}},
        {{ 0.0,  1.0, 0.0, 1}},

        {{ 0.0,  0.0, 1.0, 1}},
        {{ 1.0,  0.0, 1.0, 1}},
        {{ 1.0,  1.0, 1.0, 1}},
        {{ 0.0,  1.0, 1.0, 1}}
    };

//0, 1, 2, 2, 3, 0

static u32 CUBE_INDICIES[] = {
        1, 0, 3, 1, 3, 2,
        4, 5, 6, 4, 6, 7,
        5, 1, 2, 5, 2, 6,
        0, 4, 7, 0, 7, 3,
        2, 3, 7, 2, 7, 6,
        5, 4, 0, 5, 0, 1
    };

static vec2s CUBE_UV_CORDS[] = {
        {{1, 0}},
        {{0, 0}},
        {{0, 1}},
        {{1, 1}},

        {{1, 0}},
        {{0, 0}},
        {{0, 1}},
        {{1, 1}}
    };


void sp_render_quad_int(mat4s transform, SPTexture *tex, u32 color_hex) {
    f32 tex_index = sp_item_check_texture(tex);
    vec4s color = sp_hex_to_vec4(color_hex);

    for(u32 i = 0; i < 4; i++) {
        vec3s pos = glms_mat4_mulv3(transform, (vec3s){{QUAD_VERTEX_POS[i].x, QUAD_VERTEX_POS[i].y, QUAD_VERTEX_POS[i].z}}, QUAD_VERTEX_POS[i].w);
        vec_push(renderer->renderering_data.vertex_buffer, pos.x);
        vec_push(renderer->renderering_data.vertex_buffer, pos.y);
        vec_push(renderer->renderering_data.vertex_buffer, pos.z);
        vec_push(renderer->renderering_data.vertex_buffer, QUAD_UV_CORDS[i].x);
        vec_push(renderer->renderering_data.vertex_buffer, QUAD_UV_CORDS[i].y);
        vec_push(renderer->renderering_data.vertex_buffer, tex_index);
        vec_push(renderer->renderering_data.vertex_buffer, color.x);
        vec_push(renderer->renderering_data.vertex_buffer, color.y);
        vec_push(renderer->renderering_data.vertex_buffer, color.z);
        vec_push(renderer->renderering_data.vertex_buffer, color.w);
        vec_push(renderer->renderering_data.vertex_buffer, 0);
    };
    vec_push(renderer->renderering_data.index_buffer, renderer->renderering_data.offset + 0);
    vec_push(renderer->renderering_data.index_buffer, renderer->renderering_data.offset + 1);
    vec_push(renderer->renderering_data.index_buffer, renderer->renderering_data.offset + 2);
    vec_push(renderer->renderering_data.index_buffer, renderer->renderering_data.offset + 2);
    vec_push(renderer->renderering_data.index_buffer, renderer->renderering_data.offset + 3);
    vec_push(renderer->renderering_data.index_buffer, renderer->renderering_data.offset + 0);
    renderer->renderering_data.offset += 4;
}

SAPPHIRE_API void sp_render_quad(f32 x, f32 y, f32 width, f32 height, SPTexture *tex, u32 color_hex) {
    mat4s pos = glms_translate(glms_mat4_identity(), (vec3s){{x, y, 0}});
    mat4s scale = glms_scale(glms_mat4_identity(), (vec3s){{width, height, 1.0f}});
    mat4s transform = glms_mat4_mul(pos, scale);
    sp_render_quad_int(transform, tex, color_hex);
}

SAPPHIRE_API void sp_render_quad_rotated(f32 x, f32 y, f32 width, f32 height, f32 rotation, SPTexture *tex, u32 color_hex) {
    mat4s pos = glms_translate(glms_mat4_identity(), (vec3s){{x, y, 0}});
    mat4s rot = glms_rotate(glms_mat4_identity(), glm_rad(rotation), (vec3s){{0, 0, 1}});
    mat4s scale = glms_scale(glms_mat4_identity(), (vec3s){{width, height, 1.0f}});
    mat4s transform = glms_mat4_mul(glms_mat4_mul(pos, rot), scale);
    sp_render_quad_int(transform, tex, color_hex);
}

SAPPHIRE_API void sp_render_text(f32 x, f32 y, const char *format, ...) {
    va_list args;
    struct SPFont *font = renderer->fonts[vec_len(renderer->fonts) - 1];
    struct SPTexture *texture = font->texture;

    va_start(args, format);
    String text = string_vformat(format, args);
    va_end(args);

    f32 tex_index = sp_item_check_texture(texture);

    vec2s pos = {{x, y}};

    for(size_t j = 0; j < string_len(&text); j++) {
        u32 index = string_cstr(&text)[j] - 32;

        if(index == '\n')
            continue;

        struct SPChar c = font->chars[index];

        vec2s e_pox = {{
            pos.x + c.bearing.x,
            pos.y - (c.size.y - c.bearing.y)
        }};

        vec2s uv_cords[4] = {
            {{c.uv_min.x, c.uv_max.y}},
            {{c.uv_max.x, c.uv_max.y}},
            {{c.uv_max.x, c.uv_min.y}},
            {{c.uv_min.x, c.uv_min.y}}
        };

        vec2s size = c.size;
        mat4s transform = glms_mat4_mul(
            glms_translate(glms_mat4_identity(), (vec3s){{e_pox.x, e_pox.y, 0}}),
            glms_scale(glms_mat4_identity(), (vec3s){{size.x, size.y, 1.0f}})
        );
        for(u32 i = 0; i < 4; i++) {
            vec3s pos = glms_mat4_mulv3(transform, (vec3s){{QUAD_VERTEX_POS[i].x, QUAD_VERTEX_POS[i].y, QUAD_VERTEX_POS[i].z}}, QUAD_VERTEX_POS[i].w);
            vec_push(renderer->renderering_data.vertex_buffer, pos.x);
            vec_push(renderer->renderering_data.vertex_buffer, pos.y);
            vec_push(renderer->renderering_data.vertex_buffer, pos.z);
            vec_push(renderer->renderering_data.vertex_buffer, uv_cords[i].x);
            vec_push(renderer->renderering_data.vertex_buffer, uv_cords[i].y);
            vec_push(renderer->renderering_data.vertex_buffer, tex_index);
            vec_push(renderer->renderering_data.vertex_buffer, 1);
            vec_push(renderer->renderering_data.vertex_buffer, 1);
            vec_push(renderer->renderering_data.vertex_buffer, 1);
            vec_push(renderer->renderering_data.vertex_buffer, 1);
            vec_push(renderer->renderering_data.vertex_buffer, 1);
        }
        vec_push(renderer->renderering_data.index_buffer, renderer->renderering_data.offset + 0);
        vec_push(renderer->renderering_data.index_buffer, renderer->renderering_data.offset + 1);
        vec_push(renderer->renderering_data.index_buffer, renderer->renderering_data.offset + 2);
        vec_push(renderer->renderering_data.index_buffer, renderer->renderering_data.offset + 2);
        vec_push(renderer->renderering_data.index_buffer, renderer->renderering_data.offset + 3);
        vec_push(renderer->renderering_data.index_buffer, renderer->renderering_data.offset + 0);
        renderer->renderering_data.offset += 4;

        pos.x += c.advance >> 6;
    }
    string_destroy(&text);
}

SAPPHIRE_API void sp_render_quad_v2(vec2s pos, vec2s size, SPTexture *tex, u32 color_hex) {
    sp_render_quad(pos.x, pos.y, size.x, size.y, tex, color_hex);
}

SAPPHIRE_API void sp_render_quad_rotated_v2(vec2s pos, vec2s size, f32 rotation, SPTexture *tex, u32 color_hex) {
    sp_render_quad_rotated(pos.x, pos.y, size.x, size.y, rotation, tex, color_hex);
}

static void sp_render_cube_int(mat4s transform, SPTexture *tex, u32 color_hex) {
    f32 tex_index = sp_item_check_texture(tex);
    vec4s color = sp_hex_to_vec4(color_hex);
    for(u32 i = 0; i < 8; i++) {
        vec3s pos = glms_mat4_mulv3(transform, (vec3s){{CUBE_VERTEX_POS[i].x, CUBE_VERTEX_POS[i].y, CUBE_VERTEX_POS[i].z}}, CUBE_VERTEX_POS[i].w);
        //vec3s pos = glms_vec3(CUBE_VERTEX_POS[i]);
        vec_push(renderer->renderering_data.vertex_buffer, pos.x);
        vec_push(renderer->renderering_data.vertex_buffer, pos.y);
        vec_push(renderer->renderering_data.vertex_buffer, pos.z);
        vec_push(renderer->renderering_data.vertex_buffer, CUBE_UV_CORDS[i].x);
        vec_push(renderer->renderering_data.vertex_buffer, CUBE_UV_CORDS[i].y);
        vec_push(renderer->renderering_data.vertex_buffer, tex_index);
        vec_push(renderer->renderering_data.vertex_buffer, color.x);
        vec_push(renderer->renderering_data.vertex_buffer, color.y);
        vec_push(renderer->renderering_data.vertex_buffer, color.z);
        vec_push(renderer->renderering_data.vertex_buffer, color.w);
        vec_push(renderer->renderering_data.vertex_buffer, 0);
    }
    for(u32 idx = 0; idx < 36; idx++) {
        vec_push(renderer->renderering_data.index_buffer, renderer->renderering_data.offset + CUBE_INDICIES[idx]);
    }
    renderer->renderering_data.offset += 8;
}

SAPPHIRE_API void sp_render_cube(f32 x, f32 y, f32 z, f32 width, f32 height, f32 depth, SPTexture *tex, u32 color_hex) {
    mat4s transform = glms_mat4_mul(
        glms_translate(glms_mat4_identity(), (vec3s){{x, y, z}}),
        glms_scale(glms_mat4_identity(), (vec3s){{width, height, depth}})
    );
    sp_render_cube_int(transform, tex, color_hex);
}

SAPPHIRE_API void sp_render_cube_v3(vec3s pos, vec3s dims, SPTexture *tex, u32 color_hex) {
    sp_render_cube(pos.x, pos.y, pos.z, dims.x, dims.y, dims.z, tex, color_hex);
}

SAPPHIRE_API void sp_renderer_flush(void) {
    sp_shader_bind(renderer->shader);
    if(renderer->camera_current) {
        sp_shader_uniform_view_proj(renderer->shader, sp_camera_viewproj_get(renderer->camera_current));
    }
    sp_shader_uniform_mat4(renderer->shader, "m", glms_mat4_identity());
    sp_shader_uniform_textures2D(renderer->shader, "textures");
    for(size_t i = 0; i < 32; i++) {
        glBindTextureUnit(i, renderer->renderering_data.textures[i].handle);
    }

    sp_vbo_buffer(renderer->vbo, renderer->renderering_data.vertex_buffer, 0, vec_len(renderer->renderering_data.vertex_buffer) * sizeof(f32));
    sp_vbo_buffer(renderer->ibo, renderer->renderering_data.index_buffer, 0, vec_len(renderer->renderering_data.index_buffer) * sizeof(u32));

    sp_vao_attr(renderer->vao, renderer->vbo, 0, 3, GL_FLOAT, 11 * sizeof(f32), 0 * sizeof(f32));
    sp_vao_attr(renderer->vao, renderer->vbo, 1, 2, GL_FLOAT, 11 * sizeof(f32), 3 * sizeof(f32));
    sp_vao_attr(renderer->vao, renderer->vbo, 2, 1, GL_FLOAT, 11 * sizeof(f32), 5 * sizeof(f32));
    sp_vao_attr(renderer->vao, renderer->vbo, 3, 4, GL_FLOAT, 11 * sizeof(f32), 6 * sizeof(f32));
    sp_vao_attr(renderer->vao, renderer->vbo, 4, 1, GL_FLOAT, 11 * sizeof(f32), 10 * sizeof(f32));

    sp_vao_bind(renderer->vao);
    sp_vbo_bind(renderer->ibo);

    glDrawElements(GL_TRIANGLES, vec_len(renderer->renderering_data.index_buffer), GL_UNSIGNED_INT, NULL);
    renderer->info.draw_calls++;
    sp_info("Drew with %llu indicies", vec_len(renderer->renderering_data.index_buffer));
    sp_renderer_data_clear();
}

SAPPHIRE_API void sp_batch_end(void) {
    sp_renderer_flush();
    if(renderer)
        renderer->info = (RendererInfo){0};
}

SAPPHIRE_API void sp_renderer_clear_color_set(f32 r, f32 g, f32 b, f32 a) {
    renderer->clear_color = (vec4s){{r, g, b, a}};
}

void sp_render_prep(void) {
    if(renderer->camera_current) {
        switch(sp_camera_type_get(renderer->camera_current)) {
        case SP_CAMERA_ORTHO:
            glClear(GL_COLOR_BUFFER_BIT);
            glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
            glDisable(GL_DEPTH_TEST);
            glDisable(GL_CULL_FACE);
            glEnable(GL_BLEND);
            glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
            break;
        case SP_CAMERA_PERSPECTIVE:
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
            glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
            glEnable(GL_DEPTH_TEST);
            glDepthFunc(GL_LESS);
            glEnable(GL_CULL_FACE);
            glCullFace(GL_BACK);
            glEnable(GL_BLEND);
            glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
            break;
        }
        glClearColor(
            renderer->clear_color.x, renderer->clear_color.y,
            renderer->clear_color.z, renderer->clear_color.w
        );

        //sp_camera_update(renderer->camera_current);
    }
}

void sp_renderer_data_clear(void) {
    renderer->renderering_data.offset = 0;
    renderer->renderering_data.texture_count = 0;
    vec_len(renderer->renderering_data.vertex_buffer) = 0;
    vec_len(renderer->renderering_data.index_buffer) = 0;
    memset(renderer->renderering_data.vertex_buffer, 0, vec_cap(renderer->renderering_data.vertex_buffer) * sizeof(f32));
    memset(renderer->renderering_data.index_buffer, 0, vec_cap(renderer->renderering_data.index_buffer) * sizeof(u32));
    memset(renderer->renderering_data.textures, 0, 32 * sizeof(SPTexture));
}

f32 sp_item_check_texture(SPTexture *tex) {
    if(renderer->renderering_data.texture_count >= 31
    || vec_len(renderer->renderering_data.vertex_buffer) >= RENDERER_MAX_VERTECIES
    || vec_len(renderer->renderering_data.index_buffer) >= RENDERER_MAX_INDICIES
    ) {
        sp_renderer_flush();
    }

    s32 tex_index = -1;
    for(size_t i = 0; i < 32; i++) {
        if(renderer->renderering_data.textures[i].handle == tex->handle) {
            tex_index = i;
        }
    }

    if(tex_index == -1) {
        tex_index = renderer->renderering_data.texture_count;
        renderer->renderering_data.textures[(s32)renderer->renderering_data.texture_count] = *tex;
        renderer->renderering_data.texture_count++;
    }

    if(tex_index == -1)
        sp_panic(SP_ERROR_SP, "couldnt fit texture [%d]", tex);

    return (f32)tex_index;
}

SAPPHIRE_API void sp_font_push(SPFont *font) {
    if(!renderer)
        return;

    vec_push(renderer->fonts, font);
}

SAPPHIRE_API void sp_font_pop(void) {
    if(!renderer)
        return;

    vec_pop(renderer->fonts);
}

#ifdef DSP_DEBUG
SAPPHIRE_API RendererInfo sp_renderer_info_get(void) {
    if(!renderer)
        sp_panic("renderer needs to be initalized");
    return renderer->info;
}
#endif
