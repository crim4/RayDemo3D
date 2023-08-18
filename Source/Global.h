#include "RayLib.h"

inline bool is_input_exit() {
    return IsKeyDown(KEY_LEFT_CONTROL) && IsKeyPressed(KEY_Q);
}

inline bool should_exit() {
    return
        is_input_exit() ||
        WindowShouldClose();
}