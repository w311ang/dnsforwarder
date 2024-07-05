mingw32-make -e ARCH_EXT=_amd64 -e CFLAGS="-m64 -Wall -DWIN32_LEAN_AND_MEAN" -e LDFLAGS="-m64 -static-libgcc" %*
