#include "../internal.h"
#include <stdio.h>
#include <stdlib.h>

char *sp_file_read(const char *path) {
    FILE *fd;
    fopen_s(&fd, path, "r");

    size_t file_size = 0;

    if(!fd)
        sp_panic(SP_ERROR_IO, "Could not open file [%s]", path);

    fseek(fd, 0, SEEK_END);
    file_size = ftell(fd);

    if(file_size == 0)
        sp_panic(SP_ERROR_IO, "Cannot read empty file [%s]", path);

    rewind(fd);

    char *file_data = calloc(file_size + 1, sizeof(char));
    fread_s(file_data, file_size + 1, sizeof(char), file_size + 1, fd);
    if(!file_data)
        sp_panic(SP_ERROR_IO, "Could not read file [%s]", path);

    return file_data;
}
