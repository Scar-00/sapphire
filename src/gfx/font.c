#include "../internal.h"
#include <freetype/freetype.h>
#include <math.h>
#include <stdlib.h>

SAPPHIRE_API SPFont *sp_font_init_from_file(const char *path, size_t size) {
    if(!file_exists(string_view_from(path))) {
        sp_info("Failed to load font: file `%s` does not exist", path);
        return NULL;
    }

    struct SPFont self = {
        .chars = vec_with_size(struct SPChar, 96),
    };

    FT_Library ft;
    if(FT_Init_FreeType(&ft)) {
        sp_panic(SP_ERROR_FONT, "Could not init freetype", "");
    }

    FT_Face face = NULL;
    if(FT_New_Face(ft, path, 0, &face)) {
        sp_panic(SP_ERROR_FONT, "Could not load font [%s]", path);
    }

    FT_Set_Pixel_Sizes(face, 0, size);

    s32 max_dim = (1 + (face->size->metrics.height >> 5)) * ceilf(sqrtf(96));

    ivec2s tex_size = {{1, 0}};
    while (tex_size.x < max_dim) tex_size.x <<= 1;
    tex_size.y = tex_size.x;

    ivec2s pen = {{0, 0}};

    u8 *bitmap = calloc(tex_size.x * tex_size.y, sizeof(u8));

    for(u32 c = 32; c < 128; c++) {
        if(FT_Load_Char(face, c, FT_LOAD_RENDER | FT_LOAD_TARGET_(FT_RENDER_MODE_SDF)))
            sp_warn("Could not load char [%c]", c);

        int mode = FT_RENDER_MODE_NORMAL;
        //int mode = FT_RENDER_MODE_SDF;
        if(FT_Render_Glyph(face->glyph, mode)) {
            sp_panic(SP_ERROR_FONT, "", "");
        }

        FT_Bitmap *bmp = &face->glyph->bitmap;
        if(pen.x + bmp->width >= (u32)tex_size.x) {
            pen.x = 0;
            pen.y += ((face->size->metrics.height >> 5) + 1);
        }

        for(u32 row = 0; row < bmp->rows; ++row) {
            for(u32 col = 0; col < bmp->width; ++col) {
                s32 x = pen.x + col;
                s32 y = pen.y + row;
                bitmap[y * tex_size.x + x] = bmp->buffer[row * bmp->pitch + col];
            }
        }

        struct SPChar character = {
            .size = {{face->glyph->bitmap.width, face->glyph->bitmap.rows}},
            .bearing = {{face->glyph->bitmap_left, face->glyph->bitmap_top}},
            .uv_min = {{pen.x / (f32)tex_size.x, pen.y / (f32)tex_size.y}},
            .uv_max = {{(pen.x + bmp->width) / (f32)tex_size.y, (pen.y + bmp->rows) / (f32)tex_size.y}},
            .advance = face->glyph->advance.x,
        };
        vec_push(self.chars, character);
        pen.x += bmp->width + 1;
    }

    self.size = (vec2s){{tex_size.x, tex_size.y}};
    self.texture = sp_texture_from_font(bitmap, tex_size.x, tex_size.y);

    free(bitmap);

    sp_window_add_font(self);

    return sp_window_font_back();
}

SAPPHIRE_API void *sp_font_atlas_get(SPFont *font) {
    return font->texture;
}

SAPPHIRE_API u32 sp_font_size_get(SPFont *font) {
    return font->size.x;
}
