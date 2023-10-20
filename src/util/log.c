#include "../internal.h"
#include <Windows.h>

typedef enum Color {
    DARK_BLUE = 1,
    GREEN,
    CYAN,
    RED,
    PURPLE,
    YELLOW,
    LIGHT_GREY,
    GREY,
    BLUE,
    LIGHT_GREEN,
    LIGHT_BLUE,
    LIGHT_RED,
    LIGHT_PURPLE,
    LIGHT_YELLOW,
    WHITE,
    DEFAULT = LIGHT_GREY,
}Color;

const char *sp_error_to_string(SPError err) {
    const char *errors[] = {
        "unknown",
        "GLFW",
        "GLAD",
        "Sapphire",
        "IO",
        "Font",
    };

    return errors[err];
}

static void sp_color_set(Color c) {
    SetConsoleTextAttribute(GetStdHandle(STD_ERROR_HANDLE), c);
}

void sp_panic_expl(SPError err, u32 line, const char *file, const char *message, ...) {
    sp_color_set(RED);
    va_list args;
    va_start(args, message);
    StringStream stream = stringstream_init_va(message, args);
    fprintf(stderr, "[ERROR]: %s --> %s\n", sp_error_to_string(err), stream.c_str);
    fprintf(stderr, "[%s: %d]\n", file, line);
    stringstream_destroy(&stream);
    va_end(args);
    sp_color_set(DEFAULT);
    exit((s32)err);
}

void sp_info(const char *message, ...) {
    sp_color_set(CYAN);
    va_list args;
    va_start(args, message);
    fprintf(stderr, "[INFO]: %s\n", string_init_va(message, args).c_str);
    va_end(args);
    sp_color_set(DEFAULT);
}

void sp_warn(const char *message, ...) {
    sp_color_set(YELLOW);
    va_list args;
    va_start(args, message);
    fprintf(stderr, "[WARN]: %s\n", string_init_va(message, args).c_str);
    va_end(args);
    sp_color_set(DEFAULT);
}
