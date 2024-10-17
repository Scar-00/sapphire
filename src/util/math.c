#include <math.h>
#include <util/math.h>
#include "../internal.h"
#include <time.h>
#include <stdlib.h>

inline vec4s sp_rgb_to_vec4s(f32 r, f32 g, f32 b) {
    return (vec4s){{(float)r / (float)255, (float)g / (float)255, (float)b / (float)255, 1.0f}};
}

vec4s sp_hex_to_vec4(u32 hex) {
    return (vec4s){{
        ((hex >> 24) & 0xFF) / 255.0f,
        ((hex >> 16) & 0xFF) / 255.0f,
        ((hex >> 8) & 0xFF) / 255.0f,
        ((hex >> 0) & 0xFF) / 255.0f,
    }};
}

s32 sp_hash_str(const char *str) {
    s32 value = -1;
    for(u32 i = 0; i < strlen(str); i++) {
        value += str[i] * 0x0bfa10 / 3;
    }

    return value;
}

u64 sp_time_now(void) {
    struct timespec ts = {0};
    timespec_get(&ts, TIME_UTC);
    return (ts.tv_sec * NS_PER_SECOND) + ts.tv_nsec;
}

static vec3s sp_screen_to_ndc_pos(vec2s pos) {
    return (vec3s){{
        (2.0f * pos.x) / sp_window_size_x() - 1.0,
        1.0f - (2.0f * pos.y) / sp_window_size_y(),
        1.0f
    }};
}

//return the sign bit of a number as an s32
static s32 sign(f32 v) {
    return ((f32)0 < v) - (v < (f32)0);
}

/*UUID gen_uuid() {
    UUID uuid;
    UuidCreate(&uuid);
    return uuid;
}*/

//[TODO!]check how this works!!!
// finds the smallest possible t such that s + t * ds is an integer
static vec3s intbound(vec3s s, vec3s ds) {
    vec3s v;
    #if defined(__clang__)
    #pragma clang loop unroll_count(3)
    #elif defined(__GNUC__)
    #pragma GCC unroll 3
    #endif
    for (size_t i = 0; i < 3; i++) {
        v.raw[i] = (ds.raw[i] > 0 ? (ceilf(s.raw[i]) - s.raw[i]) : (s.raw[i] - floorf(s.raw[i]))) / fabsf(ds.raw[i]);
    }
    return v;
}

//{{floorf(ray.x), floorf(ray.y), floorf(ray.z)}};

SAPPHIRE_API bool sp_cast_ray(SPCamera *camera, SPPos ray, SPIntersectionFunc func, void *args) {
    vec3s pos_curr = {{sp_camera_pos_x(camera), sp_camera_pos_y(camera), sp_camera_pos_z(camera)}};
    //vec3s dir = glms_vec3_add((vec3s){{ray.x, -ray.y, ray.z}}, sp_camera_dir_get(camera));
    vec3s dir = {{ray.x, -ray.y, ray.z}};
    vec3s step = {{sign(ray.x), sign(ray.y), sign(ray.z)}};


    vec3s tmax = intbound((vec3s){{ray.x, -ray.y, ray.z}}, dir);
    vec3s tdelta = glms_vec3_div(step, dir);
    f32 radius = 50 / glms_vec3_norm(dir);

    sp_info("ray -> | x: %.3f | y: %.3f | z: %.3f |", ray.x, ray.y, ray.z);
    sp_info("dir -> | x: %.3f | y: %.3f | z: %.3f |", dir.x, dir.y, dir.z);
    sp_warn("step -> | x: %.3f | y: %.3f | z: %.3f |", step.x, step.y, step.z);
    sp_warn("origin -> | x: %.3f | y: %.3f | z: %.3f |", pos_curr.x, pos_curr.y, pos_curr.z);
    //sp_warn("origin -> | x: %.3f | y: %.3f | z: %.3f |", pos_curr.x, pos_curr.y, pos_curr.z);

    s32 iter = 0;
    while(true) {
        if(func(args, (s32)(pos_curr.x), (s32)(pos_curr.y), (s32)(pos_curr.z)))
            return true;

        if (tmax.x < tmax.y) {
            if (tmax.x < tmax.z) {
                if (tmax.x > radius) {
                    break;
                }

                pos_curr.x += step.x;
                tmax.x += tdelta.x;
            }else {
                if(tmax.z > radius) {
                    break;
                }

                pos_curr.z += step.z;
                tmax.z += tdelta.z;
            }
        }else {
            if(tmax.y < tmax.z) {
                if(tmax.y > radius) {
                    break;
                }

                pos_curr.y += step.y;
                tmax.y += tdelta.y;
            }else {
                if(tmax.z > radius) {
                    break;
                }

                pos_curr.z += step.z;
                tmax.z += tdelta.z;
            }
        }
        sp_warn("tmax -> | x: %.3f | y: %.3f | z: %.3f |", tmax.x, tmax.y, tmax.z);
        iter++;
        if(iter > 100)
            sp_panic(SP_ERROR_SP, "Test", "");
    }
    return false;
}

SAPPHIRE_API SPPos sp_screen_to_world_pos(const SPCamera *camera, f32 x, f32 y) {
    vec3s ndc = sp_screen_to_ndc_pos((vec2s){{x, y}});
    vec4s ray_clip = {{ndc.x, ndc.y, -1, 1}};
    vec4s ray_eye = glms_mat4_mulv(glms_mat4_inv(sp_camera_proj_get(camera)), ray_clip);
    ray_eye.z = -1;
    ray_eye.w = 0;
    vec3s ray_world = glms_vec3(glms_mat4_mulv(glms_mat4_inv(sp_camera_view_get(camera)), ray_eye));
    glms_vec3_normalize(ray_world);

    return (SPPos){{ray_world.x, ray_world.y, ray_world.z}};
}
