if [ "$#" -ne 1 ]; then
  echo "usage: ./build win|lin"
  exit 1
fi

OUT="game"
FLAGS="-Wall -Wextra -Werror -Wpedantic -std=c23"
DEF="-DDEV"
SRC=$(find ./src/ -type f -name "*.c" | tr '\n' ' ')
LIBS="\
-L./vendor/ \
"
INCS="\
-I./vendor/ \
"

if [ "$1" = "win" ]; then
  DEF+=" -DWINDOWS"
  LIBS+="\
-lwin-glfw3 \
-lgdi32 \
"
  OUT+=".exe"
  CC=x86_64-w64-mingw32-gcc
elif [ "$1" = "lin" ]; then
  DEF+=" -DLINUX"
  LIBS+="\
-llin-glfw3 \
-lm \
"
  CC=gcc
else
  echo "invalid target platform $1"
fi

$CC $FLAGS $DEF $SRC $LIBS $INCS -o $OUT
