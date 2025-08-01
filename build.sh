if [ "$#" -ne 1 ]; then
  echo "usage: ./build win|lin"
  exit 1
fi

OUT="game"
FLAGS="-Wall -Wextra -Werror -Wpedantic -std=c23"
DEF="-DDEV"
SRCS=$(find ./src/ -type f -name "*.c" ! -path "./src/engine/x64win.c" ! -path "./src/engine/x64lin.c" | tr '\n' ' ')
LIBS="\
-L./vendor/glfw/ \
"
INCS="\
-I./vendor/glfw/ \
-I./include/ \
-I./vendor/glad/include \
-I./vendor/miniaudio \
"

if [ "$1" = "win" ]; then
  DEF+=" -DWINDOWS"
  SRCS+=" ./src/engine/x64win.c"
  LIBS+="\
vendor/glad/win-glad.o \
vendor/miniaudio/win-miniaudio.o \
-static-libgcc \
-lwin-glfw3 \
-lgdi32 \
-static \
-lpthread \
"
  OUT+=".exe"
  CC=x86_64-w64-mingw32-gcc
elif [ "$1" = "lin" ]; then
  DEF+=" -DLINUX"
  SRCS+=" ./src/engine/x64lin.c"
  LIBS+="\
vendor/glad/lin-glad.o \
vendor/miniaudio/lin-miniaudio.o \
-llin-glfw3 \
-lm \
"
  CC=gcc
else
  echo "invalid target platform $1"
fi

rm -f $OUT
$CC $FLAGS $DEF $SRCS $LIBS $INCS -o $OUT
./$OUT
