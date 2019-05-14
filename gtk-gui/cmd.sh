set -e

CFLAGS="$(pkg-config --cflags gtk+-3.0)"
  LIBS="$(pkg-config --libs gtk+-3.0)"

files=""
files="$files gui.c"
files="$files gui-top.c"
files="$files canvas.c"
files="$files gtk-main.c"
gcc -g $CFLAGS $files  $LIBS -lm

files="$files README"
files="$files gui-top.h"
files="$files canvas.h"
files="$files gui.h"
files="$files cmd.sh"
files="$files set-header-tpl.h"
files="$files set-body-tpl.h"
tar -cf gtk-gui.tar $files
