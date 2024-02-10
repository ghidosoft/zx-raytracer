// Ported from https://gabrielgambetta.com/zx-raytracer.html

#include <arch/zx.h>
#include <intrinsic.h>
#include <math.h>
#include <stdio.h>
#include "raytracer.h"

const float rox = 0, roy = 0, roz = 0;
const float tmin = 0, tmax = 10000;

struct sphere spheres[] = {
    {0, -1, 4, SQUARED(1.f), PAPER_RED | BRIGHT},
    {2, 0, 4, SQUARED(1.f), PAPER_BLUE | BRIGHT},
    {-2, 0, 4, SQUARED(1.f), PAPER_GREEN | BRIGHT},
    {0, -5001, 0, SQUARED(5000.f), PAPER_YELLOW | BRIGHT}
};
const uint8_t spheres_n = sizeof(spheres) / sizeof(spheres[0]);

uint8_t trace_ray(float rdx, float rdy, float rdz) {
    uint8_t color = PAPER_BLACK;
    float min_t = tmin;

    for (uint8_t i = 0; i < spheres_n; i++) {
        const float cox = rox - spheres[i].x;
        const float coy = roy - spheres[i].y;
        const float coz = roz - spheres[i].z;

        const float eqa = rdx * rdx + rdy * rdy + rdz * rdz;
        const float eqb = 2 * (rdx * cox + rdy * coy + rdz * coz);
        const float eqc = (cox * cox + coy * coy + coz * coz) - spheres[i].radius_squared;

        const float disc = eqb * eqb - 4 * eqa * eqc;
        if (disc < 0) {
            continue;
        }

        const float disc_sqrt = sqrt(disc);

        const float t1 = (-eqb + disc_sqrt) / 2 * eqa;
        const float t2 = (-eqb - disc_sqrt) / 2 * eqa;

        if (t1 >= tmin && t1 <= tmax && (t1 < min_t || color == PAPER_BLACK)) {
            color = spheres[i].color;
            min_t = t1;
        }
        if (t2 >= tmin && t2 <= tmax && (t2 < min_t || color == PAPER_BLACK)) {
            color = spheres[i].color;
            min_t = t2;
        }
    }

    return color;
}

unsigned char* memory = 0;

int main(void) {
    zx_cls(PAPER_WHITE | INK_BLACK);

    // Reset frame counter
    memory[23672] = memory[23673] = memory[23674] = 0;

    // Re-enable interrupt (for the ROM frame counter)
    intrinsic_ei();

    for (uint8_t x = 0; x < 32; x++) {
        for (uint8_t y = 0; y < 22; y++) {
            float rdx = ((float) x - 16.f) / 32.f;
            float rdy = (11.f - (float) y) / 32.f;
            float rdz = 1.f;

            uint8_t color = trace_ray(rdx, rdy, rdz);

            *zx_pxy2aaddr(x * 8, y * 8) = color;
        }
    }

    intrinsic_di();

    const uint32_t end = ((uint32_t)(memory[23674]) << 16) | ((uint32_t)(memory[23673]) << 8) | memory[23672];
    const uint32_t elapsed = end / 50;
    printf("Elapsed %d", elapsed);

    return 0;
}
