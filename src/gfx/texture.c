#include <gfx/texture.h>
#include "../internal.h"

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

void sp_texture_load_pixels(const char* path, u8 **pixels_out, size_t *width_out, size_t *height_out);

SAPPHIRE_API SPTexture *sp_texture_from_pixels(u8 *pixels, size_t width, size_t height) {
    struct SPTexture *self = malloc(sizeof(*self));
    *self = (SPTexture){
        .size = (ivec2s) {{ width, height }},
    };

    glGenTextures(1, &self->handle);
    glBindTexture(GL_TEXTURE_2D, self->handle);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, pixels);
    return self;
}

SAPPHIRE_API SPTexture *sp_texture_from_font(u8 *pixels, size_t width, size_t height) {
    struct SPTexture *self = malloc(sizeof(*self));
    *self = (SPTexture){
        .size = (ivec2s) {{ width, height }},
    };

    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

    glGenTextures(1, &self->handle);
    glBindTexture(GL_TEXTURE_2D, self->handle);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    //GLint swizzle[] = { GL_ONE, GL_ONE, GL_ONE, GL_RED };
    //glTexParameteriv(GL_TEXTURE_2D, GL_TEXTURE_SWIZZLE_RGBA, swizzle);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, width, height, 0, GL_RED, GL_UNSIGNED_BYTE, pixels);
    return self;
}

SAPPHIRE_API SPTexture *sp_texture_from_path(const char *path) {
    u8 *pixels;
    size_t width, height;
    sp_texture_load_pixels(path, &pixels, &width, &height);
    SPTexture *self = sp_texture_from_pixels(pixels, width, height);
    free(pixels);
    return self;
}

SAPPHIRE_API void sp_texture_destroy(SPTexture *self) {
    glDeleteTextures(1, &self->handle);
    free(self);
}

SAPPHIRE_API u32 sp_texture_get_handle(SPTexture const *self) {
    return self->handle;
}

SAPPHIRE_API s32 sp_texture_get_width(SPTexture const *self) {
    return self->size.x;
}

SAPPHIRE_API s32 sp_texture_get_height(SPTexture const *self) {
    return self->size.y;
}

void sp_texture_bind(SPTexture self) {
    glBindTexture(GL_TEXTURE_2D, self.handle);
}

void sp_texture_load_pixels(const char* path, u8 **pixels_out, size_t *width_out, size_t *height_out) {
    s32 width, height, channels = 0;
    stbi_set_flip_vertically_on_load(true);
    u8 *image = stbi_load(path, &width, &height, &channels, STBI_rgb_alpha);
    if(image == NULL)
        sp_panic(SP_ERROR_SP, "imgage[%s] could not be loaded", path);

    *pixels_out = malloc(width * height * 4);
    memcpy(*pixels_out, image, width * height * 4);
    *width_out = width;
    *height_out = height;
    stbi_image_free(image);
}
