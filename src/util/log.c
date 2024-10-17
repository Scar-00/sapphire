#include "../internal.h"

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
    sp_set_os_term_color(c);
    //SetConsoleTextAttribute(GetStdHandle(STD_ERROR_HANDLE), c);
}

void sp_panic_expl(SPError err, u32 line, const char *file, const char *message, ...) {
    //sp_color_set(RED);
    va_list args;
    va_start(args, message);
    String stream = string_vformat(message, args);
    fprintf(stderr, "[ERROR]: %s --> %s\n", sp_error_to_string(err), string_cstr(&stream));
    fprintf(stderr, "[%s: %d]\n", file, line);
    va_end(args);
    string_destroy(&stream);
    //sp_color_set(DEFAULT);
    exit((s32)err);
}

void sp_info(const char *message, ...) {
    sp_color_set(CYAN);
    va_list args;
    va_start(args, message);
    String string = string_vformat(message, args);
    fprintf(stderr, "[INFO]: %s\n", string_cstr(&string));
    va_end(args);
    string_destroy(&string);
    sp_color_set(DEFAULT);
}

void sp_warn(const char *message, ...) {
    sp_color_set(YELLOW);
    va_list args;
    va_start(args, message);
    String string = string_vformat(message, args);
    fprintf(stderr, "[WARN]: %s\n", string_cstr(&string));
    va_end(args);
    string_destroy(&string);
    sp_color_set(DEFAULT);
}
