#include <core.h>
#include "../internal.h"

SPVAO sp_vao_create() {
    SPVAO self = {0};
    glGenVertexArrays(1, &self.handle);
    return self;
}

void sp_vao_bind(SPVAO self) {
    glBindVertexArray(self.handle);
}

void sp_vao_attr(SPVAO vao, SPVBO vbo, GLuint index, GLuint size, GLenum type, GLsizei stride, size_t offset) {
    sp_vao_bind(vao);
    sp_vbo_bind(vbo);
    switch (type) {
        case GL_BYTE:
        case GL_UNSIGNED_BYTE:
        case GL_SHORT:
        case GL_UNSIGNED_SHORT:
        case GL_INT:
        case GL_UNSIGNED_INT:
        case GL_INT_2_10_10_10_REV:
        case GL_UNSIGNED_INT_2_10_10_10_REV:
            glVertexAttribIPointer(index, size, type, stride, (void *) offset);
            break;
        default:
            glVertexAttribPointer(index, size, type, GL_FALSE, stride, (void *) offset);
            break;
    }
    glEnableVertexAttribArray(index);
}

void sp_vao_destroy(SPVAO self) {
    glDeleteVertexArrays(1, &self.handle);
}

SPVBO sp_vbo_create(GLint type, bool dynamic) {
    SPVBO self = {
        .type = type,
        .dynamic = dynamic,
    };
    glGenBuffers(1, &self.handle);
    return self;
}

void sp_vbo_bind(SPVBO self) {
    glBindBuffer(self.type, self.handle);
}

void sp_vbo_buffer(SPVBO self, void *data, size_t offset, size_t count) {
    sp_vbo_bind(self);
    glBufferData(self.type, count - offset, data, self.dynamic ? GL_DYNAMIC_DRAW : GL_STATIC_DRAW);
}

void sp_vbo_destroy(SPVBO self) {
    glDeleteBuffers(1, &self.handle);
}