#include "../internal.h"

SAPPHIRE_API SPFrameBufferSpec sp_framebuffer_spec_init_expl(u32 count, SPFrameBufferSpecType *types) {
    SPFrameBufferSpec self = {
        .type = vec_with_size(SPFrameBufferSpecType, count),
        .width = 0,
        .height = 0,
    };

    vec_iter(self.type, i) {
        self.type[i] = types[i];
    }

    return self;
}

SAPPHIRE_API SPFrameBuffer sp_framebuffer_init(SPFrameBufferSpec spec) {
    return (SPFrameBuffer){
        .spec = spec,
        .samples = 1,
        .swap_chain = true,
        .color_attachments = vec_with_size(u32, vec_len(spec.type)),
        .depth_buffer = 0,
        .renderer_id = 0,
    };
}

SAPPHIRE_API void sp_framebuffer_invalidate(SPFrameBuffer *self) {
    if(self->renderer_id) {
        //glDeleteFramebuffers(1, &self->renderer_id);
        //glDeleteTextures(1, &self->color_buffer);
    }

    glCreateFramebuffers(1, &self->renderer_id);
    glBindFramebuffer(GL_FRAMEBUFFER, self->renderer_id);

    glCreateTextures(GL_TEXTURE_2D, 1, &self->color_attachments[0]);
    glBindTexture(GL_TEXTURE_2D, self->color_attachments[0]);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, self->spec.width, self->spec.height, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, self->color_attachments[0], 0);

    //array_loop(self->color_attachments, i) {
    //    sp_framebuffer_color_attachment_create(&self->color_attachments[i], self->spec.width, self->spec.height);
    //}

    if(glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
        sp_panic(SP_ERROR_GLAD, "Could not create framebuffer [%d]", self->renderer_id);
    }

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

SAPPHIRE_API u32 sp_framebuffer_get_color_attachment(SPFrameBuffer self, u32 index) {
    return self.color_attachments[index];
}

SAPPHIRE_API void sp_framebuffer_bind(SPFrameBuffer self) {
    glBindFramebuffer(GL_FRAMEBUFFER, self.renderer_id);
    glViewport(0, 0 , self.spec.width, self.spec.height);
}

SAPPHIRE_API void sp_framebuffer_unbind(SPFrameBuffer self) {
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void sp_framebuffer_color_attachment_create(u32 *attachment, u32 width, u32 height) {
    if(!attachment)
        return;

    glCreateTextures(GL_TEXTURE_2D, 1, attachment);
    glBindTexture(GL_TEXTURE_2D, *attachment);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, *attachment, 0);
}
