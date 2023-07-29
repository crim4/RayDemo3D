#ifndef LIBS_RAYLIB_C
#define LIBS_RAYLIB_C

#include "../Libs/RayLib/Include/raylib.h"
#include "../Libs/RayLib/Include/raymath.h"
#include "../Libs/RayLib/Include/rlgl.h"
#include <stdint.h>

static inline Vector2 vec2(float x, float y) {
    return (Vector2) {
        .x = x,
        .y = y
    };
}

static inline Vector3 vec3(float x, float y, float z) {
    return (Vector3) {
        .x = x,
        .y = y,
        .z = z
    };
}

static inline Rectangle rect(Vector2 pos, Vector2 size) {
    return (Rectangle) {
        .x = pos.x,
        .y = pos.y,
        .width = size.x,
        .height = size.y
    };
}

static inline Vector2 scalar_to_vec2(float scalar) {
    return vec2(scalar, scalar);
}

static inline Vector3 scalar_to_vec3(float scalar) {
    return vec3(scalar, scalar, scalar);
}

static inline Color color(uint8_t r, uint8_t g, uint8_t b, uint8_t a) {
    return (Color) {
        .r = r,
        .g = g,
        .b = b,
        .a = a
    };
}

#endif