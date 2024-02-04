LIBS=-lm
CFLAGS = +zx -vn -clib=sdcc_iy -startup=31 -create-app
CC=z88dk.zcc
OUT_DIR=out

all: $(OUT_DIR)/raytracer

$(OUT_DIR)/raytracer: raytracer.c
	@mkdir -p $(@D)
	$(CC) $(CFLAGS) $(LIBS) $^ -o $@ -create-app

clean:
	rm -rf $(OUT_DIR)
