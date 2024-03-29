CC=z88dk.zcc
CFLAGS = +zx -vn -O3 -clib=sdcc_iy -startup=30 -create-app
LIBS=-lm

OUT_DIR=out

all: $(OUT_DIR)/raytracer1 $(OUT_DIR)/raytracer3

$(OUT_DIR)/raytracer1: raytracer1.c
	@mkdir -p $(@D)
	$(CC) $(CFLAGS) $(LIBS) $^ -o $@ -create-app

$(OUT_DIR)/raytracer3: raytracer3.c
	@mkdir -p $(@D)
	$(CC) $(CFLAGS) $(LIBS) $^ -o $@ -create-app

clean:
	rm -rf $(OUT_DIR)
