#include "Base.c"
#include "RayLib.c"

#define SCREEN_W ((float)1200)
#define SCREEN_H ((float)650)
#define TITLE ((char const*)"Demo")

#define BACKGROUND_COLOR ((Color){30, 30, 30, 255})

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

#define UI_EDGE_OFFSET ((float)40)
#define UI_DEBUG_FONT_SIZE ((float)25)
#define UI_DEBUG_FONT_SPACING ((float)5)
#define UI_DEBUG_TEXT_MAX_LENGTH ((int)40)

#define WEAPONS_COUNT ((uint8_t)2)
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

    weapons_t weapons;
    // index to ctx_t.weapons
    uint8_t selected_weapon;

    Camera3D camera;
} ctx_t;

void listen_for_exit(ctx_t* ctx);

bool is_input_exit() {
    return IsKeyDown(KEY_LEFT_CONTROL) && IsKeyPressed(KEY_Q);
}

bool is_input_zoom_in() {
    return IsKeyDown(KEY_LEFT_CONTROL) && IsKeyDown(KEY_RIGHT_BRACKET);
}

bool is_input_zoom_out() {
    return IsKeyDown(KEY_LEFT_CONTROL) && IsKeyDown(KEY_SLASH);
}

bool is_input_switch_next_weapon() {
    return IsKeyDown(KEY_LEFT_CONTROL) && IsKeyPressed(KEY_TAB);
}

bool is_input_switch_previous_weapon() {
    return
        IsKeyDown(KEY_LEFT_CONTROL) &&
        IsKeyDown(KEY_LEFT_SHIFT) &&
        IsKeyPressed(KEY_TAB);
}

bool is_fovy_in_bounds(float fovy) {
    return IS_IN_INCLUSIVE_RANGE(fovy, 10, 100);
}

float delta_time() {
    return GetFrameTime();
}

void update_camera_from_zoom(float* fovy, float zoom_state) {
    float const r = *fovy + ZOOM_STEP * delta_time() * ZOOM_DELTATIME_FACTOR * zoom_state;

    if (!is_fovy_in_bounds(r))
        return;

    *fovy = r;
}

void draw_test_cube() {
    Vector3 const pos = vec3(0, 1, 0);
    Vector3 const size = vec3(2, 2, 2);
    DrawCubeV(pos, size, DARKGREEN);

    Vector3 const wires_size = Vector3Add(size, scalar_to_vec3(0.3));
    DrawCubeWiresV(pos, wires_size, BLACK);
}

Model ctx_cur_weapon(ctx_t* ctx) {
    return ctx->weapons.models[ctx->selected_weapon];
}

float ctx_cur_weapon_scale(ctx_t* ctx) {
    return ctx->weapons.scales[ctx->selected_weapon];
}

char const* ctx_cur_weapon_name(ctx_t* ctx) {
    return ctx->weapons.names[ctx->selected_weapon];
}

void ctx_draw_current_weapon(ctx_t* ctx) {
    Vector3 const pos = scalar_to_vec3(0);
    // the fovy value the model should start
    // to get faded
    float const fading_fovy_limit = 13;

    uint8_t model_alpha;
    if (ctx->camera.fovy < fading_fovy_limit)
        model_alpha = FROM_XRANGE_TO_YRANGE(
            ctx->camera.fovy,
            ZOOM_MIN, fading_fovy_limit,
            0, 255
        );
    else
        model_alpha = 255;

    const uint8_t model_alpha_inversed = 255 - model_alpha;

    DrawModel(
        ctx_cur_weapon(ctx),
        pos,
        ctx_cur_weapon_scale(ctx),
        color(255, 255, 255, model_alpha)
    );
    DrawModelWires(
        ctx_cur_weapon(ctx),
        pos,
        ctx_cur_weapon_scale(ctx),
        color(255, 255, 255, model_alpha_inversed)
    );
}

void ctx_zoom_smoothly(float* zoom_state, float target) {
    *zoom_state = Lerp(
        *zoom_state,
        target,
        ZOOM_SMOOTH_STEP * delta_time() * ZOOM_DELTATIME_FACTOR
    );
}

void ctx_handle_zoom(ctx_t* ctx) {
    static float smooth_zoom_state = 0;

    if (is_input_zoom_in())
        ctx_zoom_smoothly(&smooth_zoom_state, ZOOM_IN_TARGET);
    else if (is_input_zoom_out())
        ctx_zoom_smoothly(&smooth_zoom_state, ZOOM_OUT_TARGET);
    else
        ctx_zoom_smoothly(&smooth_zoom_state, ZOOM_STOP_TARGET);

    update_camera_from_zoom(&ctx->camera.fovy, smooth_zoom_state);
}

void ctx_switch_weapon(ctx_t* ctx, int8_t switch_direction) {
    int16_t const r = ctx->selected_weapon + (int16_t)switch_direction;

    if (!IS_IN_RANGE(r, 0, WEAPONS_COUNT))
        return;

    ctx->selected_weapon = r;
}

void ctx_handle_weapon_switch(ctx_t* ctx) {
    if (is_input_switch_previous_weapon())
        ctx_switch_weapon(ctx, WEAPON_SWITCH_DIRECTION_PREVIOUS);
    else if (is_input_switch_next_weapon())
        ctx_switch_weapon(ctx, WEAPON_SWITCH_DIRECTION_NEXT);
}

void ctx_update(ctx_t* ctx) {
    ctx_handle_zoom(ctx);
    ctx_handle_weapon_switch(ctx);
}

void ctx_drawing_update(ctx_t* ctx) {
    ctx_draw_current_weapon(ctx);
}

void clear_bg() {
    ClearBackground(BACKGROUND_COLOR);
}

void ui_draw_fps(Font font) {
    char buf[UI_DEBUG_TEXT_MAX_LENGTH];
    sprintf_s(buf, sizeof(buf), "fps: %d", GetFPS());

    DrawTextEx(
        font,
        buf,
        scalar_to_vec2(UI_EDGE_OFFSET),
        UI_DEBUG_FONT_SIZE,
        UI_DEBUG_FONT_SPACING,
        GRAY
    );
}

float measure_text_width(Font font, char const* buf) {
    return MeasureTextEx(
        font,
        buf,
        UI_DEBUG_FONT_SIZE,
        UI_DEBUG_FONT_SPACING
    ).x;
}

float calculate_zoom_percentage_from_fovy(float fovy) {
    return FROM_XRANGE_TO_YRANGE(
        ZOOM_MAX - fovy + ZOOM_MIN,
        ZOOM_MIN, ZOOM_MAX,
        0, 100
    );
}

void ui_draw_zoom_percentage(Font font, float fovy) {
    float const zoom_percentage = calculate_zoom_percentage_from_fovy(fovy);
    char buf[UI_DEBUG_TEXT_MAX_LENGTH];
    sprintf_s(buf, sizeof(buf), "zoom: %.0f%%", zoom_percentage);

    DrawTextEx(
        font,
        buf,
        vec2(
            SCREEN_W - UI_EDGE_OFFSET - measure_text_width(font, buf),
            UI_EDGE_OFFSET
        ),
        UI_DEBUG_FONT_SIZE,
        UI_DEBUG_FONT_SPACING,
        GRAY
    );
}

void ui_draw_weapon_number(
    Font font,
    uint8_t number
) {
    char buf[UI_DEBUG_TEXT_MAX_LENGTH];
    sprintf_s(buf, sizeof(buf), "(%d of %d)", number, WEAPONS_COUNT);
    Vector2 const text_size = MeasureTextEx(
        font, buf, WEAPON_INFO_FONT_SIZE, UI_DEBUG_FONT_SPACING
    );

    DrawTextEx(
        font,
        buf,
        vec2(
            SCREEN_W - UI_EDGE_OFFSET - text_size.x,
            SCREEN_H - UI_EDGE_OFFSET - text_size.y
        ),
        WEAPON_INFO_FONT_SIZE,
        UI_DEBUG_FONT_SPACING,
        GRAY
    );
}

void ui_draw_weapon_name(Font font, char const* name) {
    Vector2 const text_size = MeasureTextEx(
        font, name, WEAPON_INFO_FONT_SIZE, UI_DEBUG_FONT_SPACING
    );

    DrawTextEx(
        font,
        name,
        vec2(
            UI_EDGE_OFFSET,
            SCREEN_H - UI_EDGE_OFFSET - text_size.y
        ),
        WEAPON_INFO_FONT_SIZE,
        UI_DEBUG_FONT_SPACING,
        WEAPON_NAME_COLOR
    );
}

void ctx_draw_ui(ctx_t* ctx) {
    Font const font = ctx->font;

    ui_draw_fps(font);
    ui_draw_zoom_percentage(font, ctx->camera.fovy);
    ui_draw_weapon_name(font, ctx_cur_weapon_name(ctx));
    ui_draw_weapon_number(font, ctx->selected_weapon + 1);
}

void ctx_internal_update(ctx_t* ctx) {
    UpdateCamera(&ctx->camera, CAMERA_ORBITAL);

    BeginDrawing();
        clear_bg();
        ctx_update(ctx);

        BeginMode3D(ctx->camera);
            ctx_drawing_update(ctx);
        EndMode3D();

        ctx_draw_ui(ctx);
    EndDrawing();
}

void ctx_loop(ctx_t* ctx) {
    for (;;) {
        listen_for_exit(ctx);
        ctx_internal_update(ctx);
    }
}

void init_ctx_weapon(
    ctx_t* ctx,
    uint8_t weapon_index,
    char const* texture_path,
    char const* model_path,
    char const* name,
    float scale
) {
    // loading texture
    Texture2D const weapon_texture = LoadTexture(texture_path);
    // loading model
    ctx->weapons.models[weapon_index] = LoadModel(model_path);
    // setting the model's texture
    ctx->weapons.models[weapon_index].materials[0].maps[MATERIAL_MAP_DIFFUSE].texture = weapon_texture;

    // setting miscs
    ctx->weapons.names[weapon_index] = name;
    ctx->weapons.scales[weapon_index] = scale;
}

void deinit_ctx_weapon(ctx_t* ctx, uint8_t weapon_index) {
    // unloading texture
    UnloadTexture(
        ctx->weapons.models[weapon_index].materials[0].maps[MATERIAL_MAP_DIFFUSE].texture
    );
    // unloading model
    UnloadModel(ctx->weapons.models[weapon_index]);
}

void init_ctx_weapons(ctx_t* ctx) {
    init_ctx_weapon(
        ctx, 0,
        "Res/Pistol/BaseColor.png", "Res/Pistol/Model.obj",
        "PISTOL", 1
    );
    init_ctx_weapon(
        ctx, 1,
        "Res/MultiPistol/BaseColor.png", "Res/MultiPistol/Model.obj",
        "MULTI PISTOL", 2
    );
}

void deinit_ctx_weapons(ctx_t* ctx) {
    for (uint8_t i = 0; i < WEAPONS_COUNT; i++)
        deinit_ctx_weapon(ctx, i);
}

void init_ctx(ctx_t* ctx) {
    InitWindow(SCREEN_W, SCREEN_H, TITLE);
    SetExitKey(KEY_NULL);

    ctx->font = LoadFontEx("Res/IBM3270.ttf", FONT_RESOLUTION, NULL, 0);
    SetTextureFilter(ctx->font.texture, TEXTURE_FILTER_BILINEAR);

    init_ctx_weapons(ctx);
    ctx->selected_weapon = GetRandomValue(0, WEAPONS_COUNT - 1);

    ctx->camera = (Camera3D) {
        .position = vec3(-10, 15, -10),
        .target = vec3(0, 0, 0),
        .up = vec3(0, 1, 0),
        .fovy = 45,
        .projection = CAMERA_PERSPECTIVE
    };
}

void deinit_ctx(ctx_t* ctx) {
    UnloadFont(ctx->font);
    deinit_ctx_weapons(ctx);

    CloseWindow();
}

void ctx_exit(ctx_t* ctx) {
    deinit_ctx(ctx);
    exit(0);
}

void listen_for_exit(ctx_t* ctx) {
    if (is_input_exit())
        ctx_exit(ctx);

    if (WindowShouldClose())
        ctx_exit(ctx);
}

int main() {
    ctx_t ctx;
    init_ctx(&ctx);
    ctx_loop(&ctx);
    deinit_ctx(&ctx);

    return 0;
}