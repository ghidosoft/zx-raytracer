// Ported from https://gabrielgambetta.com/zx-raytracer-3-src.html

#include <arch/zx.h>
#include <math.h>
#include <string.h>
#include "raytracer.h"

struct sphere spheres[] = {
    {0, -1, 4, SQUARED(1.f), INK_RED},
    {2, 0, 4, SQUARED(1.f), INK_BLUE},
    {-2, 0, 4, SQUARED(1.f), INK_GREEN},
    {0, -5001, 0, SQUARED(5000.f), INK_YELLOW}
};
const uint8_t spheres_n = sizeof(spheres) / sizeof(spheres[0]);

uint8_t trace_ray(float dx, float dy) {
    uint8_t color = PAPER_BLACK;
    float mt = 1e10f;
    const float a = 2 * (dx * dx + dy * dy + 1.f);

    for (uint8_t i = 0; i < spheres_n; i++) {
        const float cx = spheres[i].x;
        const float cy = spheres[i].y;
        const float cz = spheres[i].z;
        const float b = 2 * (dx * cx + dy * cy + cz);
        const float c = (cx * cx + cy * cy + cz * cz) - spheres[i].radius_squared;

        float d = b * b - 2 * a * c;
        if (d < 0) {
            continue;
        }

        d = sqrt(d);
        float t = (b + d) / a;
        if (t > 0 && t < mt) {
            color = spheres[i].color;
            mt = t;
        }
        t = (b - d) / a;
        if (t > 0 && t < mt) {
            color = spheres[i].color;
            mt = t;
        }
    }

    return color;
}

uint8_t color_pixels[64];
uint8_t color_usage[8];

int main(void) {
    zx_cls(PAPER_WHITE | INK_BLACK);

    for (int16_t x = 0; x < 256; x += 8) {
        for (int16_t y = 0; y < 176; y += 8) {
            const uint8_t ctl = trace_ray((x - 128) / 256.f, (y - 88) / -256.f);
            const uint8_t ctr = trace_ray((x - 128 + 7) / 256.f, (y - 88) / -256.f);
            const uint8_t cbl = trace_ray((x - 128) / 256.f, (y - 88 + 7) / -256.f);
            const uint8_t cbr = trace_ray((x - 128 + 7) / 256.f, (y - 88 + 7) / -256.f);

            if (ctl == cbr && ctl == ctr && ctl == cbl) {
                // All corners same color -> set block background
                unsigned char* aaddr = zx_saddr2aaddr(zx_pxy2saddr(x, y));
                (*aaddr) = (ctl << 3) | BRIGHT;
                continue;
            }

            memset(color_usage, 0, 8);

            uint8_t ci = 0;
            for (int16_t u = x; u < x + 8; u++) {
                const float dx = (u - 128) / 256.f;
                for (int16_t v = y; v < y + 8; v++) {
                    uint8_t color;
                    switch (ci) {
                        case 0:
                            color = ctl;
                            break;
                        case 7:
                            color = cbl;
                            break;
                        case 56:
                            color = ctr;
                            break;
                        case 63:
                            color = cbr;
                            break;
                        default:
                            color = trace_ray(dx, (v - 88) / -256.f);
                            break;
                    }
                    
                    color_usage[color]++;
                    color_pixels[ci++] = color;
                }
            }

            // Find the most and second frequent color in this 8x8 block
            uint8_t mfc = 0;
            uint8_t color_paper = 0;
            for (uint8_t c = 0; c < 8; c++) {
                if (color_usage[c] > mfc) {
                    mfc = color_usage[c];
                    color_paper = c;
                }
            }

            mfc = 0;
            uint8_t color_ink = 0;
            for (uint8_t c = 0; c < 8; c++) {
                if (c != color_paper && color_usage[c] > mfc) {
                    mfc = color_usage[c];
                    color_ink = c;
                }
            }

            // Paper is the most frequent color, ink the second
            *zx_saddr2aaddr(zx_pxy2saddr(x, y)) = (color_paper << 3) | color_ink | BRIGHT;

            // Plot all secondary colors
            ci = 0;
            for (int16_t u = x; u < x + 8; u++) {
                for (int16_t v = y; v < y + 8; v++) {
                    if (color_pixels[ci] != color_paper) {
                        *zx_pxy2saddr(u, v) |= zx_px2bitmask(u);
                    }
                    ci++;
                }
            }
        }
    }

    return 0;
}
