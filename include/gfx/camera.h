#ifndef _SAPPHIRE_CAMERA_H_
#define _SAPPHIRE_CAMERA_H_

#include "../sp_core.h"

typedef struct SPCamera SPCamera;

typedef enum SPRotationAxis {
    ROTATION_X,
    ROTATION_Y,
    ROTATION_Z
}SPRotationAxis;

typedef enum SPCameraType {
    SP_CAMERA_ORTHO,
    SP_CAMERA_PERSPECTIVE,
}SPCameraType;

SAPPHIRE_API SPCamera *sp_camera_create(SPCameraType type);
SAPPHIRE_API void sp_camera_destroy(SPCamera *camera);
SAPPHIRE_API void sp_camera_update(SPCamera *camera);

SAPPHIRE_API void sp_camera_move(SPCamera *camera ,float x, float y, float z);
SAPPHIRE_API void sp_camera_turn(SPCamera *camera, f32 pitch, f32 yaw);
SAPPHIRE_API void sp_camera_zoom(SPCamera *camera, f32 delta);
SAPPHIRE_API void sp_camera_rotate(SPCamera *camera, f32 degree, SPRotationAxis axis);

SAPPHIRE_API f32 sp_camera_zoom_get(SPCamera *camera);
SAPPHIRE_API f32 sp_camera_pos_x(SPCamera *camera);
SAPPHIRE_API f32 sp_camera_pos_y(SPCamera *camera);
SAPPHIRE_API f32 sp_camera_pos_z(SPCamera *camera);

#endif
