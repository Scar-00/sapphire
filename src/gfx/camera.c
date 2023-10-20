#include <gfx/camera.h>
#include <math.h>
#include "../internal.h"
#include "cglm/struct/affine.h"
#include "cglm/struct/cam.h"
#include "cglm/struct/mat4.h"
#include "cglm/struct/vec2.h"
#include "cglm/struct/vec3.h"
#include "cglm/types-struct.h"
#include "cglm/types.h"
#include "core.h"
#include "gfx/window.h"
#include "util/math.h"
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

struct SPCamera {
    SPCameraType type;
    SPViewProj view_proj;
    vec3s pos;
    f32 aspect;
    union {
        struct SPPerspective {
            vec3s dir, up, right;
            f32 pitch, yaw ,fov, znear, zfar;
        }perspective;

        struct SPOrtho {
            vec2s min, max;
            f32 zoom;
        }ortho;
    };
};

SAPPHIRE_API SPCamera *sp_camera_create(SPCameraType type) {
    SPCamera *self = calloc(1, sizeof(*self));
    *self = (SPCamera){
        .type = type,
        .pos = glms_vec3_zero(),
        .view_proj = {
            .proj = glms_mat4_identity(),
            .view = glms_mat4_identity(),
        },
        .aspect = (f32)sp_get()->size.x / (f32)sp_get()->size.y,
    };
    switch (type) {
    case SP_CAMERA_PERSPECTIVE :
        self->perspective = (struct SPPerspective){
            .pitch = 0.0f,
            .yaw = 135.0f,
            .fov = 45.0f,
            .znear = 0.001f,
            .zfar = 1000.0f,
        };
    break;
    case SP_CAMERA_ORTHO:
        self->ortho = (struct SPOrtho){
            .min = glms_vec2_zero(),
            .max = {{sp_get()->size.x, sp_get()->size.y}},
            .zoom = 1.0f,
        };
    break;
    }
    sp_camera_update(self);
    return self;
}

SAPPHIRE_API void sp_camera_destroy(SPCamera *camera) {
    free(camera);
}

SAPPHIRE_API void sp_camera_update(SPCamera *camera) {
    switch(camera->type) {
    case SP_CAMERA_ORTHO:
        if(sp_window_event(SP_EVENT_WINDOW_RESIZED)) {
            sp_info("Window resize event");
            camera->ortho.max = (vec2s){{sp_get()->size.x, sp_get()->size.y}};
        }

        mat4s transform = glms_mat4_mul(glms_translate(glms_mat4_identity(), camera->pos), glms_rotate(glms_mat4_identity(), 0, (vec3s){{0, 0, 1}}));
        camera->view_proj.view = glms_mat4_inv(transform);
        //camera->view_proj.proj = glms_ortho(camera->ortho.min.x, camera->ortho.max.x, camera->ortho.min.y, camera->ortho.max.y, -100.0f, 100.0f);
        camera->view_proj.proj = glms_ortho(camera->ortho.min.x * camera->ortho.zoom, camera->ortho.max.x * camera->ortho.zoom, camera->ortho.min.y * camera->ortho.zoom, camera->ortho.max.y * camera->ortho.zoom, -100.0f, 100.0f);
        //camera->view_proj.proj = glms_rotate(glms_mat4_identity(), glm_rad(-45), (vec3s){{0, 0, 1}});
        //camera->view_proj.proj = glms_ortho(-camera->aspect * camera->ortho.zoom, camera->aspect *camera->ortho.zoom, -camera->ortho.zoom, camera->ortho.zoom, -1.0f, 1.0f);
        //sp_info("zoom -> %f", camera->ortho.zoom);
        break;
    case SP_CAMERA_PERSPECTIVE:
        camera->perspective.pitch = clamp(camera->perspective.pitch, -PI_2, PI_2);
        camera->perspective.yaw = (camera->perspective.yaw < 0 ? TAU : 0.0f) + fmod(camera->perspective.yaw, TAU);

        camera->perspective.dir = (vec3s) {{
            cosf(camera->perspective.pitch) * sinf(camera->perspective.yaw),
            sinf(camera->perspective.pitch),
            cosf(camera->perspective.pitch) * cosf(camera->perspective.yaw),
        }};

        glms_vec3_normalize(camera->perspective.dir);
        camera->perspective.right = glms_vec3_cross((vec3s){{0.0f, 1.0f, 0.0f}}, camera->perspective.dir);
        camera->perspective.up = glms_vec3_cross(camera->perspective.dir, camera->perspective.right);

        camera->view_proj.view = glms_lookat(camera->pos, glms_vec3_add(camera->pos, camera->perspective.dir), camera->perspective.up);
        //TODO: does this need to use rad ????
        camera->view_proj.proj = glms_perspective(glm_rad(camera->perspective.fov), camera->aspect, camera->perspective.znear, camera->perspective.zfar);
        //sp_info("cam -> %.2f | %.2f | %.2f", camera->pos.x, camera->pos.y, camera->pos.z);
        //sp_info("pitch: %f | yaw: %f\n", camera->perspective.pitch, camera->perspective.yaw);
        break;
    }
}

SAPPHIRE_API void sp_camera_move(SPCamera *camera, float x, float y, float z) {
    camera->pos.x += x;
    camera->pos.y += y;
    camera->pos.z += z;
}

SAPPHIRE_API void sp_camera_turn(SPCamera *camera, f32 pitch, f32 yaw) {
    assert(camera->type == SP_CAMERA_PERSPECTIVE && "cannot turn camera of type ortho");

    camera->perspective.pitch += pitch;
    camera->perspective.yaw += yaw;
}

SAPPHIRE_API void sp_camera_zoom(SPCamera *camera, f32 delta) {
    camera->ortho.zoom += delta;
    if(camera->ortho.zoom < 0.00001f) {
        camera->ortho.zoom = 0.00001f;
    }
}

vec3s axi[] = {
    {{1, 0, 0}},
    {{0, 1, 0}},
    {{0, 0, 1}},
};

SAPPHIRE_API void sp_camera_rotate(SPCamera *camera, f32 degree, SPRotationAxis axis) {
    camera->view_proj.view = glms_mat4_inv(glms_mat4_mul(glms_mat4_inv(camera->view_proj.view), glms_rotate(glms_mat4_identity(), glm_rad(degree), axi[axis])));
}

SPViewProj sp_camera_viewproj_get(const SPCamera *camera) {
    return camera->view_proj;
}

mat4s sp_camera_proj_get(const SPCamera *camera) {
    return camera->view_proj.proj;
}

mat4s sp_camera_view_get(const SPCamera *camera) {
    return camera->view_proj.view;
}

vec3s sp_camera_dir_get(const SPCamera *camera) {
    return camera->perspective.dir;
}

SPCameraType sp_camera_type_get(SPCamera const *camera) {
    return camera->type;
}

SAPPHIRE_API f32 sp_camera_zoom_get(SPCamera *camera) {
    return camera->ortho.zoom;
}

SAPPHIRE_API f32 sp_camera_pos_x(SPCamera *camera) {
    return camera->pos.x;
}

SAPPHIRE_API f32 sp_camera_pos_y(SPCamera *camera) {
    return camera->pos.y;
}

SAPPHIRE_API f32 sp_camera_pos_z(SPCamera *camera) {
    return camera->pos.z;
}
