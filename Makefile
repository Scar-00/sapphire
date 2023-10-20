CC = clang
AR = ar -rcs

CFLAGS = -std=c11 -O3 -g -Wall -Wextra -Wpedantic -Wstrict-aliasing -Wno-newline-eof -Wno-deprecated-declarations -DDLL_BUILD -DSP_DEBUG -Werror -Wno-gnu-zero-variadic-macro-arguments -Wno-unused-parameter -Wno-strict-prototypes
CFLAGS += -Iinclude
CFLAGS += -Ilib/cglm/include
CFLAGS += -Ilib/stb
CFLAGS += -Ilib/gaia/include
CFLAGS += -Ilib/freetype
CFLAGS += -Ilib/glad/include
CFLAGS += -Ilib/glfw/include

LDFLAGS = lib/cglm/libcglm.a
LDFLAGS += lib/gaia/libgaia.lib
LDFLAGS += lib/freetype/freetype.lib
LDFLAGS += lib/glad/src/glad.o
LDFLAGS += lib/glfw/libglfw3dll.a
LDFLAGS += -luuid

SRC = $(wildcard src/**/**/*.c) $(wildcard src/**/*.c) $(wildcard src/*.c)
DATA = $(wildcard src/**/**/*.o.json) $(wildcard src/**/*.o.json) $(wildcard src/*.o.json)
OBJ = $(SRC:.c=.o)

static: $(OBJ)
	$(AR) sapphire.lib $^ $(LDFLAGS)

dynamic: $(OBJ)
	$(CC) -shared -o sapphire.dll $^ $(LDFLAGS)

data: $(OBJ)
	sed -e '1 s/./[ '\\\n'{/' -e '$$ s/,$$/'\\\n'] /' $(DATA) > compile_commands.json

%.o: %.c
	$(CC) -o $@ -c $< -MJ $@.json $(CFLAGS)

clean:
	rm -rf $(OBJ) libgaia.a sapphire.dll libsapphire.lib libsapphire.exp
