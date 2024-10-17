#ifndef _SAPPHIRE_INTERNAL_H_
#define _SAPPHIRE_INTERNAL_H_

#include <stdlib.h>
#include <string.h>

#include <sp_core.h>
#include <util/os.h>
#define CORE_SHORT
#include <core.h>

// constants
#define PI_2 GLM_PI_2
#define PI GLM_PI
#define TAU (2 * PI)

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
#include <glad/glad.h>

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wmissing-braces"
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wstatic-in-inline"

#include <cglm/cglm.h>
#include <cglm/struct.h>

#pragma GCC diagnostic pop
#pragma GCC diagnostic pop

#include <util/math.h>

#define FLAG_SET(f, v) ((f) |= (v))
#define FLAG_CLEAR(f, v) ((f) &= ~(v))
#define FLAG_CHECK(f, v) ((f & v) == (v))
#define FLAG_FLIP(f, v) ((f) ^= (v))
#define FLAS_SET_BOOL(f, b, v) ((f) |= b ? (v) : 0)

/*typedef s32 ivec2[2];
typedef union ivec2s {
  ivec2 raw;
  struct {
    int x;
    int y;
  };
}ivec2s;*/

typedef enum SPError {
    SP_ERROR_UNKNOWN,
    SP_ERROR_GLFW,
    SP_ERROR_GLAD,
    SP_ERROR_SP,
    SP_ERROR_IO,
    SP_ERROR_FONT,
}SPError;

// Logging
const char *sp_error_to_string(SPError err);
#define sp_panic(err, message, ...) sp_panic_expl((err), __LINE__, __FILE__, (message), __VA_ARGS__)
void sp_panic_expl(SPError err, u32 line, const char *file, const char *message, ...);
void sp_warn(const char *message, ...);
void sp_info(const char *message, ...);

// OpenGL buffer objects
typedef struct SPVBO {
    GLuint handle;
    GLint type;
    bool dynamic;
}SPVBO;

SPVBO sp_vbo_create(GLint type, bool dynamic);
void sp_vbo_bind(SPVBO self);
void sp_vbo_buffer(SPVBO self, void *data, size_t offset, size_t count);
void sp_vbo_destroy(SPVBO self);

typedef struct SPVAO {
    GLuint handle;
}SPVAO;

SPVAO sp_vao_create(void);
void sp_vao_bind(SPVAO self);
void sp_vao_attr(SPVAO vao, SPVBO vbo, GLuint index, GLuint size, GLenum type, GLsizei stride, size_t offset);
void sp_vao_destroy(SPVAO self);

// Textures
#include <gfx/texture.h>
struct SPTexture {
    GLuint handle;
    ivec2s size;
};

void sp_texture_bind(SPTexture self);

#include <gfx/renderer.h>
// render pipeline
typedef struct RenderPipeline {
    u64 id;
    Vec(f32) vertecies;
    Vec(s32) indicies;
}RenderPipeline;

// Render pass
typedef struct RenderPass {
    u64 id;
    RenderPipeline pipeline;
}RenderPass;

typedef struct SPViewProj {
    mat4s view, proj;
}SPViewProj;

typedef enum SPShaderType {
    SP_SHADER_TYPE_FLOAT4,
    SP_SHADER_TYPE_FLOAT3,
    SP_SHADER_TYPE_FLOAT2,
    SP_SHADER_TYPE_FLOAT1,
}SPShaderType;

typedef struct SPShaderField {
    SPShaderType type;
    const char *name;
}SPShaderField;

// Shaders
typedef struct SPShader {
    GLuint handle;
    GLuint vs_handle;
    GLuint fs_handle;
    Vec(struct SPShaderField) fields;
}SPShader;

SPShader sp_shader_create_int(const char *vs_path, const char *fs_path, size_t layout_size, struct SPShaderField layout[]);
#define sp_shader_create(vs_path, fs_path, layout) sp_shader_create_int(vs_path, fs_path, SP_ARRSIZE(layout), layout)
SPShader sp_shader_create_src_int(StringView vs_src, StringView fs_src, size_t layout_size, struct SPShaderField layout[]);
#define sp_shader_create_src(vs_src, fs_src, layout) sp_shader_create_src_int(vs_src, fs_src, SP_ARRSIZE(layout), layout)
void sp_shader_bind(SPShader self);
void sp_shader_uniform_float(SPShader self, char *name, float f);
void sp_shader_uniform_mat4(SPShader self, char *name, mat4s m);
void sp_shader_uniform_view_proj(SPShader self, SPViewProj view_proj);
void sp_shader_uniform_vec4(SPShader self, char *name, vec4s v);
void sp_shader_uniform_texture2D(SPShader self, char *name, SPTexture texture, GLuint n);
void sp_shader_uniform_textures2D(SPShader self, char *name);

void sp_attributes_bind(SPVAO vao, SPVBO vbo, SPShader shader);

// util
inline vec4s sp_rgb_to_vec4s(f32 r, f32 g, f32 b);
vec4s sp_hex_to_vec4(u32 hex);
s32 sp_hash_str(const char *str);
#define sp_scp(ptr) ptr ? ptr : sp_panic_expl(SP_ERROR_SP, __LINE__, __FILE__, "invalid pointer[%p]", ptr)

#define NS_PER_SECOND (1000000000)
#define NS_PER_MS (1000000)
u64 sp_time_now(void);

//io

//fonts
#include <util/font.h>

struct SPChar {
    vec2s size;
    vec2s bearing;
    vec2s uv_min;
    vec2s uv_max;
    u32 advance;
};

struct SPFont {
    vec2s size;
    struct SPChar *chars;
    struct SPTexture *texture;
};

// windows
#include <gfx/window.h>
typedef struct SPButton {
    bool down, last, last_tick, pressed, pressed_tick;
}SPButton;

typedef struct SPMouse {
    SPButton buttons[GLFW_MOUSE_BUTTON_LAST];
    vec2s pos, delta;
}SPMouse;

typedef struct SPKeyBoard {
    SPButton keys[GLFW_KEY_LAST];
}SPKeyBoard;

typedef enum SPEvent {
    SP_EVENT_KEY_PRESSED,
    SP_EVENT_BUTTON_PRESSED,
    SP_EVENT_CURSOR_MOVED,
    SP_EVENT_WINDOW_RESIZED,
}SPEvent;

struct SPWindow {
    const char *name;
    vec2s size;
    SPID id;
    GLFWwindow *handle;
    SPMouse mouse;
    SPKeyBoard keyboard;
    s32 mouse_wheel;
    WinFn init, destroy, update, render;
    WinTickFn tick;
    SPWindowFlag flags;
    u64 second_last;
    u64 frames, fps, frame_last, frame_delta;
    u64 ticks, tps, tick_remainder;
    u64 tps_desired;
    SPEvent *events;
    SPFramebufferCallback fb_forward;
    SPCursorposCallback cursor_forward;
    SPKeyCallback key_forward;
    SPMouseCallback mouse_forward;
    SPScrollCallback scroll_forward;
    SPFont *fonts;
};

// TODO: rename ?
bool sp_window_event(SPEvent event);
bool sp_window_handle_event(SPEvent e);
void sp_window_add_font(SPFont font);
SPFont *sp_window_font_back(void);

// camera
#include <gfx/camera.h>
SPViewProj sp_camera_viewproj_get(SPCamera const *camera);
SPCameraType sp_camera_type_get(SPCamera const *camera);
mat4s sp_camera_proj_get(const SPCamera *camera);
mat4s sp_camera_view_get(const SPCamera *camera);
vec3s sp_camera_dir_get(const SPCamera *camera);

// framebuffer
#include <gfx/framebuffer.h>

struct SPFrameBuffer {
    u32 samples;
    SPFrameBufferSpec spec;
    bool swap_chain;
    u32 *color_attachments;
    u32 depth_buffer;
    u32 renderer_id;
};

void sp_framebuffer_color_attachment_create(u32 *attachment, u32 width, u32 height);

#endif
