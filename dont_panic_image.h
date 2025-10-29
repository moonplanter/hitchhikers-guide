#ifndef DONT_PANIC_IMAGE_H
#define DONT_PANIC_IMAGE_H

#include <stdint.h>

extern const uint32_t dont_panic_compressed[];
extern const uint32_t dont_panic_size;

void draw_compressed_image(const uint32_t* compressed, uint32_t size);

#endif
