#include "../internal.h"
#include <gfx/window.h>
#include "Gaia/names.h"
#include "core.h"

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
SPWindow *window = NULL;

static void _error_callback(int code, const char *description) {
    fprintf(stderr, "GLFW error %d: %s\n", code, description);
}

static void _size_callback(GLFWwindow *handle, int width, int height) {
    glViewport(0, 0, width, height);
    window->size = (vec2s){{width, height}};
    array_pushback(window->events, SP_EVENT_WINDOW_RESIZED);
    if(window->fb_forward != NULL) {window->fb_forward(handle, width, height);}
}

static void _cursor_callback(GLFWwindow *handle, double xp, double yp) {
    vec2s pos = {{xp, yp}};

    window->mouse.delta = glms_vec2_sub(pos, window->mouse.pos);
    window->mouse.delta.x = clamp(window->mouse.delta.x, -100.0f, 100.0f);
    window->mouse.delta.y = clamp(window->mouse.delta.y, -100.0f, 100.0f);

    window->mouse.pos = pos;
    array_pushback(window->events, SP_EVENT_CURSOR_MOVED);
    if(window->cursor_forward != NULL) {window->cursor_forward(handle, xp, yp);}
}

static void _key_callback(GLFWwindow *handle, int key, int scancode, int action, int mods) {
    if(key < 0) {
        return;
    }

    switch (action) {
    case GLFW_PRESS:
        window->keyboard.keys[key].down = true;
        break;
    case GLFW_RELEASE:
        window->keyboard.keys[key].down = false;
        break;
    default: break;
    }
    array_pushback(window->events, SP_EVENT_KEY_PRESSED);
    if(window->key_forward != NULL) {window->key_forward(handle, key, scancode, action, mods); }
}

static void _mouse_callback(GLFWwindow *handle, int button, int action, int mods) {
    if(button < 0) {
        return;
    }

    switch (action) {
    case GLFW_PRESS:
        window->mouse.buttons[button].down = true;
        break;
    case GLFW_RELEASE:
        window->mouse.buttons[button].down = false;
        break;
    default: break;
    }
    array_pushback(window->events, SP_EVENT_BUTTON_PRESSED);
    if(window->mouse_forward != NULL) {window->mouse_forward(handle, button, action, mods);}
}

static void _scroll_callback(GLFWwindow* handle, double xoffset, double yoffset) {
    window->mouse_wheel = yoffset;
    if(window->scroll_forward) {window->scroll_forward(handle, xoffset, yoffset);}
}

static void _array_update(size_t size, SPButton *buttons) {
    for(size_t i = 0; i < size; i++) {
        buttons[i].pressed = buttons[i].down && !buttons[i].last;
        buttons[i].last = buttons[i].down;
    }
}

static void _array_tick(size_t size, SPButton *buttons) {
    for(size_t i = 0; i < size; i++) {
        buttons[i].pressed_tick = buttons[i].down && !buttons[i].last;
        buttons[i].last_tick = buttons[i].down;
    }
}

static void _init() {
    window->init();
}

static void _destroy() {
    gaia_array_loop(window->fonts, i) {
        sp_texture_destroy(window->fonts[i].texture);
    }
    gaia_array_destroy(window->fonts);
    window->destroy();
}

static void _tick() {
    window->ticks++;
    _array_tick(GLFW_MOUSE_BUTTON_LAST, window->mouse.buttons);
    _array_tick(GLFW_KEY_LAST, window->keyboard.keys);
    window->tick((f32)window->frame_delta / 10000000.0f);
}

static void _update() {
    _array_update(GLFW_MOUSE_BUTTON_LAST, window->mouse.buttons);
    _array_update(GLFW_KEY_LAST, window->keyboard.keys);
    window->update();
    // clear tmp data
    window->mouse.delta = glms_vec2_zero();
    window->mouse_wheel = 0;
}

static void _render() {
    window->frames++;
    window->render();
    array_length(window->events) = 0;
}

SAPPHIRE_API SPWindow *sp_window_create(WinFn init, WinFn destroy, WinTickFn tick, WinFn update, WinFn render, const char *name, u32 width, u32 height, SPWindowFlag flags) {
    // debugging
#ifdef SP_DEBUG
    arena_init();
#endif

    //setup glfw & glad
    glfwSetErrorCallback(_error_callback);
    if (!glfwInit()){
        fprintf(stderr, "error initializing GLFW\n");
        exit(1);
    }
    glfwWindowHint(GLFW_RESIZABLE, flags & SP_WINDOW_RESIZE ? GL_TRUE : GL_FALSE);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

    //default behavior for every window
    window = calloc(1, sizeof(*window));
    *window = (SPWindow){
        .name = name,
        .size = (vec2s){{width, height}},
        .id = 0,
        .init = init,
        .destroy = destroy,
        .tick = tick,
        .update = update,
        .render = render,
        .flags = flags,
        .frame_last = sp_time_now(),
        .second_last = sp_time_now(),
        .tps_desired = 60,
        .events = array_create(SPEvent, 20),
        .fb_forward = NULL,
        .cursor_forward = NULL,
        .key_forward = NULL,
        .mouse_forward = NULL,
        .scroll_forward = NULL,
        .fonts = gaia_array_create(struct SPFont, 2),
    };

    window->handle = glfwCreateWindow(window->size.x, window->size.y, window->name, NULL, NULL);

    if(window->handle == NULL) {
        glfwTerminate();
        sp_panic(SP_ERROR_GLFW, "could not create window[%s]", window->name);
    }

    glfwMakeContextCurrent(window->handle);

    // glfwSetWindowUserPointer(window->handle, window);
    glfwSetFramebufferSizeCallback(window->handle, _size_callback);
    glfwSetCursorPosCallback(window->handle, _cursor_callback);
    glfwSetKeyCallback(window->handle, _key_callback);
    glfwSetMouseButtonCallback(window->handle, _mouse_callback);
    glfwSetScrollCallback(window->handle, _scroll_callback);

    if(!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        glfwTerminate();
        sp_panic(SP_ERROR_GLAD, "could not init");
    }
    glfwSwapInterval(1);

    return window;
}

SAPPHIRE_API void sp_window_loop(void) {
    _init();

    while (!glfwWindowShouldClose(window->handle)) {
        const u64 now = sp_time_now();

        window->frame_delta = now - window->frame_last;
        window->frame_last = now;

        if(now - window->second_last > NS_PER_SECOND) {
            window->fps = window->frames;
            window->tps = window->ticks;
            window->frames = 0;
            window->ticks = 0;
            window->second_last = now;
        }

        const u64 NS_PER_TICK = (NS_PER_SECOND / window->tps_desired);
        u64 tick_time = window->frame_delta + window->tick_remainder;
        while(tick_time > NS_PER_TICK) {
            _tick();
            tick_time -= NS_PER_TICK;
        }
        window->tick_remainder = max(tick_time, 0);
        _update();
        _render();
        glfwSwapBuffers(window->handle);
        glfwPollEvents();
    }
}

SAPPHIRE_API void sp_window_destroy(void) {
    _destroy();
    glfwTerminate();
    free(window);
}

SAPPHIRE_API SPWindow *sp_get(void) {
    return window;
}

SAPPHIRE_API f32 sp_delta_time() {
    return 0.f;
}

SAPPHIRE_API void sp_forward_callbacks(SPFramebufferCallback fb_callback, SPCursorposCallback cursor_callback, SPKeyCallback key_callback, SPMouseCallback mouse_callback, SPScrollCallback scroll_callback) {
    window->fb_forward = fb_callback;
    window->cursor_forward = cursor_callback;
    window->key_forward = key_callback;
    window->mouse_forward = mouse_callback;
    window->cursor_forward = scroll_callback;
}

SAPPHIRE_API s32 sp_key_pressed(s32 key) {
    //key = toupper(key);
    if(!(key > 0))
        return false;

    return window->keyboard.keys[key].pressed;
}

SAPPHIRE_API s32 sp_key_down(s32 key) {
    //key = toupper(key);
    if(!(key > 0))
        return false;

    return window->keyboard.keys[key].down;
}

SAPPHIRE_API s32 sp_button_pressed(s32 button) {
    if(button < 0)
        return false;

    return window->mouse.buttons[button].pressed;
}

SAPPHIRE_API s32 sp_button_down(s32 button) {
    if(button < 0)
        return false;

    return window->mouse.buttons[button].down;
}

SAPPHIRE_API f32 sp_mouse_x(void) {
    return window->mouse.pos.x;
}

SAPPHIRE_API f32 sp_mouse_y(void) {
    return window->mouse.pos.y;
}

SAPPHIRE_API f32 sp_mouse_delta_x(void) {
    return window->mouse.delta.x;
}

SAPPHIRE_API f32 sp_mouse_delta_y(void) {
    return window->mouse.delta.y;
}

SAPPHIRE_API vec2s sp_mouse_pos(void) {
    return window->mouse.pos;
}

SAPPHIRE_API s32 sp_mouse_wheel_offset(void) {
    return window->mouse_wheel;
}

SAPPHIRE_API f32 sp_window_size_x(void) {
    return window->size.x;
}

SAPPHIRE_API f32 sp_window_size_y(void) {
    return window->size.y;
}

SAPPHIRE_API void *sp_window_handle_get(void) {
    return window->handle;
}

SAPPHIRE_API u64 sp_window_tick_rate(void) {
    return window->tps;
}

SAPPHIRE_API void sp_window_tick_rate_set(u64 tps_desired) {
    window->tps_desired = tps_desired;
}

SAPPHIRE_API u64 sp_window_frame_rate(void) {
    return window->fps;
}

static void remove_event(size_t index) {
    for(size_t i = index; i < array_length(window->events) - 1; i++) {
        window->events[i] = window->events[i + 1];
    }
}

bool sp_window_event(SPEvent event) {
    size_t event_count = array_length(window->events);
    if(event_count == 0)
        return 0;

    for(size_t i = 0; i < event_count; i++) {
        if(window->events[i] == event) {
            return 1;
        }
    }
    return 0;
}

bool sp_window_handle_event(SPEvent e) {
    size_t event_count = array_length(window->events);
    if(event_count == 0)
        return 0;

    for(size_t i = 0; i < event_count; i++) {
        if(window->events[i] == e) {
            remove_event(i);
            return 1;
        }
    }
    return 0;
}

void sp_window_add_font(SPFont font) {
    if(!window)
        return;

    gaia_array_pushback(window->fonts, font);
}

SPFont *sp_window_font_back(void) {
    if(!window)
        return NULL;
    return &window->fonts[gaia_array_length(window->fonts) - 1];
}
