#include "Context.h"

/*

TODO:
    + white outline for all weapons

*/

int main() {
    InitWindow(SCREEN_W, SCREEN_H, TITLE);
    ToggleFullscreen();
    
    ctx_t ctx;
    init_ctx(&ctx);
    ctx_loop(&ctx);
    
    // unreachable
}