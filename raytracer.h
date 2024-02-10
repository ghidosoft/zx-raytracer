#ifndef RAYTRACER_H__
#define RAYTRACER_H__

#include <arch/zx.h>

struct sphere {
    float x, y, z;
    float radius_squared;
    uint8_t color;
};

#define SQUARED(x) ((x) * (x))

#endif
