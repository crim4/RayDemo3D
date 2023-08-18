#include "Base.h"
#include "RayLib.h"

#define SCREEN_W ((float)1680)
#define SCREEN_H ((float)1050)
#define TITLE ((char const*)"Demo")

#define BACKGROUND_COLOR ((Color){15, 15, 15, 255})

#define FONT_RESOLUTION ((int)128)
#define FONT_SPACING ((float)1)

#define ZOOM_STEP ((float)0.08)
#define ZOOM_IN_TARGET ((float)-1)
#define ZOOM_OUT_TARGET ((float)+1)
#define ZOOM_STOP_TARGET ((float)0)
#define ZOOM_SMOOTH_STEP ((float)0.0023)
#define ZOOM_MAX ((float)110)
#define ZOOM_MIN ((float)10)
#define ZOOM_DELTATIME_FACTOR ((float)946)

#define UI_EDGE_OFFSET ((float)65)
#define UI_DEBUG_FONT_SIZE ((float)25)
#define UI_DEBUG_FONT_SPACING ((float)5)
#define UI_DEBUG_TEXT_MAX_LENGTH ((int)40)

#define WEAPONS_COUNT ((uint8_t)3)
#define WEAPON_SWITCH_DIRECTION_NEXT ((int8_t)+1)
#define WEAPON_SWITCH_DIRECTION_PREVIOUS ((int8_t)-1)
#define WEAPON_INFO_FONT_SIZE ((float)(UI_DEBUG_FONT_SIZE * 1.4))
#define WEAPON_NAME_COLOR ((Color){200, 120, 65, 255})

typedef struct {
    Model models[WEAPONS_COUNT];
    float scales[WEAPONS_COUNT];
    char const* names[WEAPONS_COUNT];
} weapons_t;

// context for the ctx
typedef struct {
    Font font;
    // RenderTexture2D screen_shader_target;
    // Shader shader;

    weapons_t weapons;
    // index to ctx_t.weapons
    uint8_t selected_weapon;

    Camera3D camera;
} ctx_t;

void init_ctx(ctx_t* ctx);
void ctx_exit(ctx_t* ctx);
void ctx_loop(ctx_t* ctx);