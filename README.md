Build the `.tap` files using make:

```bash
make
```

Manual build command:

```bash
z88dk.zcc +zx -vn -clib=sdcc_iy -startup=31 raytracer.c -o raytracer.bin -create-app -lm
```
