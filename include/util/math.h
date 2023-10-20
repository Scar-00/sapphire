#ifndef _SAPPHIRE_MATH_H_
#define _SAPPHIRE_MATH_H_

#include <stdlib.h>
#include <stdbool.h>
#include "../gfx/camera.h"

/*#define clamp(x, mn, mx) ({\
    __typeof__ (x) _x = (x); \
    __typeof__ (mn) _mn = (mn); \
    __typeof__ (mx) _mx = (mx); \
    max(_mn, min(_mx, _x)); })
*/
#define clamp(x, mn, mx) (max(mn, min(mx, x)))

typedef union SPPos {
    f32 raw[3];
    struct {
        f32 x, y, z;
    };
}SPPos;

/*typedef struct SPRay {

}SPRay;*/

//TODO: LOOK AT QUATERNIONS

//maybe use this for intersection checking ?
/*glm_ray_triangle(float *origin, float *direction, float *v0, float *v1, float *v2, float *d);*/
typedef bool (*SPIntersectionFunc)(void *args, f32 x, f32 y, f32 z);

SAPPHIRE_API bool sp_cast_ray(SPCamera *camera, SPPos ray, SPIntersectionFunc func, void *args);
SAPPHIRE_API SPPos sp_screen_to_world_pos(const SPCamera *camera, f32 x, f32 y);

#endif
