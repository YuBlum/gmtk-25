if [ "$#" -ne 1 ]; then
  echo "usage: ./build win|lin"
  exit 1
fi

OUT="game"
FLAGS="-Wall -Wextra -Werror -Wpedantic -std=c23"
DEF="-DDEV"
SRC=$(find ./src/ -type f -name "*.c" | tr '\n' ' ')
LIBS="\
-L./vendor/glfw/ \
"
INCS="\
-I./vendor/glfw/ \
-I./include/ \
-I./vendor/glad/include \
-I./vendor/stb_image \
-I./vendor/miniaudio \
"

if [ "$1" = "win" ]; then
  DEF+=" -DWINDOWS"
  LIBS+="\
vendor/glad/win-glad.o \
vendor/stb_image/win-stb_image.o \
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
  LIBS+="\
vendor/glad/lin-glad.o \
vendor/stb_image/lin-stb_image.o \
vendor/miniaudio/lin-miniaudio.o \
-llin-glfw3 \
-lm \
"
  CC=gcc
else
  echo "invalid target platform $1"
fi

$CC $FLAGS $DEF $SRC $LIBS $INCS -o $OUT
