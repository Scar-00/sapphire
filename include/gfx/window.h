#ifndef _SAPPHIRE_WINDOW_H_
#define _SAPPHIRE_WINDOW_H_

#include "../core.h"

typedef struct SPWindow SPWindow;

typedef enum _SPWindowFlags {
    SP_WINDOW_NONE = 0,
    SP_WINDOW_RESIZE = BIT(0),
}_SPWindowFlags;

typedef void (*WinFn)(void);
typedef void (*WinTickFn)(f32 dt);
typedef s32 SPWindowFlag;

typedef void (*SPFramebufferCallback)(void *handle, int width, int height);
typedef void (*SPCursorposCallback)(void *handle, double xp, double yp);
typedef void (*SPKeyCallback)(void *handle, int key, int scancode, int action, int mods);
typedef void (*SPMouseCallback)(void *handle, int button, int action, int mods);
typedef void (*SPScrollCallback)(void *handle, double xo, double yo);

// window handling
SAPPHIRE_API SPWindow *sp_window_create(WinFn init, WinFn destroy, WinTickFn tick, WinFn update, WinFn render, const char *name, u32 width, u32 height, SPWindowFlag flags);
SAPPHIRE_API void sp_window_loop(void);
SAPPHIRE_API void sp_window_destroy(void);
SAPPHIRE_API SPWindow *sp_get(void);
SAPPHIRE_API f32 sp_delta_time(void);
SAPPHIRE_API void sp_forward_callbacks(SPFramebufferCallback fb_callback, SPCursorposCallback cursor_callback, SPKeyCallback key_callback, SPMouseCallback mouse_callback, SPScrollCallback scroll_callback);

// io & device api
SAPPHIRE_API s32 sp_key_pressed(s32 key);
SAPPHIRE_API s32 sp_key_down(s32 key);
SAPPHIRE_API s32 sp_button_pressed(s32 button);
SAPPHIRE_API s32 sp_button_down(s32 button);
SAPPHIRE_API f32 sp_mouse_x(void);
SAPPHIRE_API f32 sp_mouse_y(void);
SAPPHIRE_API f32 sp_mouse_delta_x(void);
SAPPHIRE_API f32 sp_mouse_delta_y(void);
SAPPHIRE_API s32 sp_mouse_wheel_offset(void);
SAPPHIRE_API f32 sp_window_size_x(void);
SAPPHIRE_API f32 sp_window_size_y(void);
SAPPHIRE_API void *sp_window_handle_get(void);
SAPPHIRE_API u64 sp_window_tick_rate(void);
SAPPHIRE_API void sp_window_tick_rate_set(u64 tps_desired);
SAPPHIRE_API u64 sp_window_frame_rate(void);

#endif
