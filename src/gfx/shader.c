#include <core.h>
#include "../internal.h"

static void _log(
    GLint handle, const char *adverb,
    void (*getlog)(GLuint, GLsizei, GLsizei *, GLchar*),
    void (*getiv)(GLuint, GLenum, GLint *)) {
    GLint loglen = 0;
    getiv(handle, GL_INFO_LOG_LENGTH, &loglen);

    char *logtext = calloc(1, loglen);
    getlog(handle, loglen, NULL, logtext);
    sp_panic(SP_ERROR_SP, "%s shader:\n%s", adverb, logtext);
}

static GLuint _compile(StringStream src, GLenum type) {
    GLuint handle = glCreateShader(type);
    glShaderSource(handle, 1, (const GLchar *const *)&src.c_str, (const int *)&src.length);
    glCompileShader(handle);

    GLint compiled = 0;
    glGetShaderiv(handle, GL_COMPILE_STATUS, &compiled);

    if(compiled == 0) {
        _log(handle, "compiling", glGetShaderInfoLog, glGetShaderiv);
    }
    stringstream_destroy(&src);
    return handle;
}

/*SPShader sp_shader_create(const char *vs_path, const char *fs_path) {
    GaiaFile vertex_shader = file_read(vs_path, "rb");
    GaiaFile fragment_shader = file_read(fs_path, "rb");
    return sp_shader_create_src(vertex_shader.data.c_str, fragment_shader.data.c_str);
    file_close(&vertex_shader);
    file_close(&fragment_shader);
}

SPShader sp_shader_create_src(const char *vertex, const char *fragment) {
    SPShader self = {
        .vs_handle = _compile(stringstream_init(vertex), GL_VERTEX_SHADER),
        .fs_handle = _compile(stringstream_init(fragment), GL_FRAGMENT_SHADER),
        .handle = glCreateProgram(),
    };

    glAttachShader(self.handle, self.vs_handle);
    glAttachShader(self.handle, self.fs_handle);
    glLinkProgram(self.handle);

    GLint linked = 0;
    glGetProgramiv(self.handle, GL_LINK_STATUS, &linked);

    if(linked == 0) {
        _log(self.handle, "linking", glGetProgramInfoLog, glGetProgramiv);
    }

    return self;
}*/

SPShader sp_shader_create_int(const char *vs_path, const char *fs_path, size_t layout_size, struct SPShaderField layout[]) {
    GaiaFile vertex_shader = file_read(vs_path, "rb");
    GaiaFile fragment_shader = file_read(fs_path, "rb");
    SPShader self = sp_shader_create_src_int(vertex_shader.data.c_str, fragment_shader.data.c_str, layout_size, layout);
    file_close(&vertex_shader);
    file_close(&fragment_shader);
    return self;
}

SPShader sp_shader_create_src_int(const char *vs_src, const char *fs_src, size_t layout_size, struct SPShaderField layout[]) {
    SPShader self = {
        .vs_handle = _compile(stringstream_init(vs_src), GL_VERTEX_SHADER),
        .fs_handle = _compile(stringstream_init(fs_src), GL_FRAGMENT_SHADER),
        .handle = glCreateProgram(),
        .fields = array_create(struct SPShaderField, layout_size),
    };

    //copy layout into fields
    array_loop(self.fields, i) {
        self.fields[i] = layout[i];
    }

    glAttachShader(self.handle, self.vs_handle);
    glAttachShader(self.handle, self.fs_handle);
    glLinkProgram(self.handle);

    GLint linked = 0;
    glGetProgramiv(self.handle, GL_LINK_STATUS, &linked);

    if(linked == 0) {
        _log(self.handle, "linking", glGetProgramInfoLog, glGetProgramiv);
    }

    return self;
}

void sp_shader_bind(SPShader self) {
    glUseProgram(self.handle);
}

void sp_shader_uniform_float(SPShader self, char *name, float f) {
    glUniform1f(glGetUniformLocation(self.handle, name), f);
}

void sp_shader_uniform_mat4(SPShader self, char *name, mat4s m) {
    glUniformMatrix4fv(glGetUniformLocation(self.handle, name), 1, GL_FALSE, (const GLfloat *) &m.raw);
}

void sp_shader_uniform_view_proj(SPShader self, SPViewProj view_proj) {
    sp_shader_uniform_mat4(self, "p", view_proj.proj);
    sp_shader_uniform_mat4(self, "v", view_proj.view);
}

void sp_shader_uniform_vec4(SPShader self, char *name, vec4s v) {
    glUniform4f(glGetUniformLocation(self.handle, name), v.x, v.y, v.z, v.w);
}

void sp_shader_uniform_texture2D(SPShader self, char *name, SPTexture texture, GLuint n) {
    glActiveTexture(GL_TEXTURE0 + n);
    sp_texture_bind(texture);
    glUniform1i(glGetUniformLocation(self.handle, (const GLchar *) name), n);
}

void sp_shader_uniform_textures2D(SPShader self, char *name) {
    int samplers[32];
    for(size_t i = 0; i < 32; i++) {
        samplers[i] = i;
    }
    glUniform1iv(glGetUniformLocation(self.handle, name), 32, samplers);
}

static size_t sp_get_stride(SPShader *shader) {
    size_t stride = 0;
    array_loop(shader->fields, i) {
        switch (shader->fields[i].type) {
            case SP_SHADER_TYPE_FLOAT4: {
                stride += 4;
            } break;
            case SP_SHADER_TYPE_FLOAT3: {
                stride += 3;
            } break;
            case SP_SHADER_TYPE_FLOAT2: {
                stride += 2;
            } break;
            case SP_SHADER_TYPE_FLOAT1: {
                stride += 1;
            } break;
        }
    }
    if(stride == 0)
        sp_warn("failed to compute shader stride. layout maybe empty");
    return stride;
}

/*static const char *SP_SHADER_TYPE_STRING[] = {
    "FLOAT4",
    "FLOAT3",
    "FLOAT2",
    "FLOAT1",
};*/

void sp_attributes_bind(SPVAO vao, SPVBO vbo, SPShader shader) {
    size_t stride = sp_get_stride(&shader);
    size_t offset = 0;
    array_loop(shader.fields, i) {
        switch (shader.fields[i].type) {
            case SP_SHADER_TYPE_FLOAT4: {
                sp_vao_attr(vao, vbo, i, 4, GL_FLOAT, stride * sizeof(f32), offset * sizeof(f32));
            } break;
            case SP_SHADER_TYPE_FLOAT3: {
                sp_vao_attr(vao, vbo, i, 3, GL_FLOAT, stride * sizeof(f32), offset * sizeof(f32));
            } break;
            case SP_SHADER_TYPE_FLOAT2: {
                sp_vao_attr(vao, vbo, i, 2, GL_FLOAT, stride * sizeof(f32), offset * sizeof(f32));
            } break;
            case SP_SHADER_TYPE_FLOAT1: {
                sp_vao_attr(vao, vbo, i, 1, GL_FLOAT, stride * sizeof(f32), offset * sizeof(f32));
            } break;
            default: {
                sp_warn("invailid shader type `%s` [`%s`]", shader.fields[i].type, shader.fields[i].name);
            }
        }
    }
}
