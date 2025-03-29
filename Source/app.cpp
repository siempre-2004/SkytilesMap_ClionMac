#include "app.h"
#include "raylib.h"
#include <vector>
#include "particle.h"
#include <iostream>

constexpr int min_offset_x = 50;
constexpr int max_offset_x = 800;
constexpr int min_offset_y = 50;
constexpr int max_offset_y = 800;

float Lerp(float start, float end, float amount) {
    return (1.0f - amount) * start + amount * end;
}

float Damp(float source, float target, float lambda, float deltaTime) {
    return Lerp(source, target, 1 - expf(-lambda * deltaTime));
}

std::vector<Particle> particles;

void App::init()
{
    SetWindowState(FLAG_VSYNC_HINT);
    SetTargetFPS(1080);

    camera.zoom = (float)CAMERA_ZOOM;

    map_center_x = (float)(current_tilemap->width * TILE_SIZE * ZOOM_MUTLIPLE) / 2.0f;
    map_center_y = (float)(current_tilemap->height * TILE_SIZE * ZOOM_MUTLIPLE) / 2.0f;
    camera.target = { map_center_x, map_center_y };
    camera.offset = { GetScreenWidth() / 2.0f, GetScreenHeight() / 2.0f };

    spritesheet = LoadTexture("tilesheet_packed.png");

    background_old = BackgroundOne;
    background_new = LoadTexture("BackgroundOne.png");

    background = LoadTexture("sky.png");
    BackgroundOne = LoadTexture("BackgroundOne.png");
    BackgroundThree = LoadTexture("BackgroundThree.png");

    tilemap_load = loadTileMapFromJSON("tilemap.json");

    int tile_width = spritesheet.width / 12;
    int tile_height = spritesheet.height / 11;

    player_sprite = get_sprite_from_sheet(0, 5, tile_width, tile_height);
    player_position = { 400, 300 };


    for (int y = 0; y < MAP_MATERIAl_WIDTH; y++)
    { //cut the spritesheet and assign them
        for (int x = 0; x < MAP_MATERIAl_HEIGHT; x++)
        {
            Tile tile = {};
            tile.sprite = get_sprite_from_sheet(x, y, tile_width, tile_height);
            tile.solid = false;
            Apptileset.add_tile(tile);
        }
    }
    current_tilemap->randomize(Apptileset);
}

void App::update()
{
    Vector2 mouse_world_pos = GetScreenToWorld2D(GetMousePosition(), camera);
    Vector2 selected_tile = current_tilemap->world_to_tile(mouse_world_pos.x, mouse_world_pos.y);

    if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
        int tile_id = current_tilemap->get_tile(selected_tile.x, selected_tile.y);
        last_clicked_tile = selected_tile;
        std::cout << "Selected Tile at (" << selected_tile.x << "," << selected_tile.y << ") with ID: " << tile_id << std::endl;

        spawn_particle({mouse_world_pos.x, mouse_world_pos.y}, 80.0f);
    }

    for (int i = 1; i <= 10; i++) {
        if (IsKeyPressed(KEY_ONE + (i - 1))) {
            selected_tile_id = i;
            std::cout << "Selected Tile ID: " << selected_tile_id << std::endl;
        }
    }

    if (IsKeyPressed(KEY_W) && selected_tile_id != -1 && player_coins >= 5) {
        if (last_clicked_tile.x >= 0 && last_clicked_tile.y >= 0) {
            current_tilemap->set_tile(last_clicked_tile.x, last_clicked_tile.y, selected_tile_id);
            player_coins -= 5; // **扣除币值**
            std::cout << "Tile replaced at (" << last_clicked_tile.x << "," << last_clicked_tile.y << ") with ID " << selected_tile_id << ". Coins left: " << player_coins << std::endl;

            spawn_particle({mouse_world_pos.x, mouse_world_pos.y}, 100.0f);
        }
    }

    static Vector2 camera_velocity = { 0.0f, 0.0f }; // camera speed
    map_center_x = (float)(current_tilemap->width * TILE_SIZE * ZOOM_MUTLIPLE) / 2.0f;
    map_center_y = (float)(current_tilemap->height * TILE_SIZE * ZOOM_MUTLIPLE) / 2.0f;

    Vector2 move_direction = {};
    static Vector2 target_position = camera.target;// smooth the target position of the follow
    player_position = camera.target;
    float move_speed = 500.0f;
    float damping_factor = 3.0f;

    if (IsKeyDown(KEY_LEFT)) target_position.x -= move_speed * GetFrameTime();
    if (IsKeyDown(KEY_RIGHT)) target_position.x += move_speed * GetFrameTime();
    if (IsKeyDown(KEY_UP)) target_position.y -= move_speed * GetFrameTime();
    if (IsKeyDown(KEY_DOWN)) target_position.y += move_speed * GetFrameTime();

    camera.target.x = Damp(camera.target.x, target_position.x, damping_factor, GetFrameTime());
    camera.target.y = Damp(camera.target.y, target_position.y, damping_factor, GetFrameTime());

    if (IsKeyPressed(KEY_B)) {
        background_old = background;
        background = background_new;
        transition_alpha = 0.0f;
    }

    transition_alpha = Damp(transition_alpha, 1.0f, 2.0f, GetFrameTime());

    static bool boundary_limit_enabled = false;
    static bool boundary_limit_init = false;
    static Vector2 old_camera_target = camera.target;

    if (!boundary_limit_init) {
        old_camera_target = camera.target;
        boundary_limit_init = !boundary_limit_init;
    }

    if (IsKeyPressed(KEY_C) && current_tilemap == &small_tilemap)
    {
        Vector2 vtarget = old_camera_target;
        old_camera_target = camera.target;

        boundary_limit_enabled = !boundary_limit_enabled;

        if (boundary_limit_enabled) {
            camera.target.x = (float)((current_tilemap->width * TILE_SIZE * ZOOM_MUTLIPLE) / (2.0f));
            camera.target.y = (float)((current_tilemap->height * TILE_SIZE * ZOOM_MUTLIPLE) / (2.0f));
        }
        else {
            camera.target = vtarget;
        }
        target_position = camera.target;
    }

    if (boundary_limit_enabled)
    {
        float min_x = 0, min_y = 0;
        float max_x = (float)(current_tilemap->width * TILE_SIZE * ZOOM_MUTLIPLE) / 2.0f;
        float max_y = (float)(current_tilemap->height * TILE_SIZE * ZOOM_MUTLIPLE) / 2.0f;
        float offset_x = (GetScreenWidth() / camera.zoom - (float)(current_tilemap->width * TILE_SIZE * ZOOM_MUTLIPLE)) / 2.0f;
        float offset_y = (GetScreenHeight() / camera.zoom - (float)(current_tilemap->height * TILE_SIZE * ZOOM_MUTLIPLE)) / 2.0f;
        if (offset_x >= 0) {
            min_x = max_x;
        }
        else {
            min_x = max_x + offset_x; // making the camera target back to center
            max_x = max_x - offset_x;
        }

        if (offset_y >= 0) {
            min_y = max_y;
        }
        else {
            min_y = max_y + offset_y;
            max_y = max_y - offset_y;
        }
        camera.target.x = Clamp(camera.target.x, min_x, max_x);
        camera.target.y = Clamp(camera.target.y, min_y, max_y);
    }

    if (IsKeyPressed(KEY_F)) {
        culling_enabled = !culling_enabled;
        if (!culling_enabled) {
            perform_culling = false;
        }
    }

    if (IsKeyPressed(KEY_G)) {
        if (culling_enabled) {
            perform_culling = !perform_culling;
        }
        else {}
    }

    if (IsKeyPressed(KEY_SPACE)) {
        current_tilemap->randomize(Apptileset);
    }

    //using space key to change from smalltilemap to bigger one
    if (IsKeyPressed(KEY_TAB)) {
        if (current_tilemap == &small_tilemap) {
            current_tilemap = &large_tilemap;
        }
        else {
            current_tilemap = &small_tilemap;
        }

        map_center_x = (float)(current_tilemap->width * TILE_SIZE * ZOOM_MUTLIPLE) / 2.0f;
        map_center_y = (float)(current_tilemap->height * TILE_SIZE * ZOOM_MUTLIPLE) / 2.0f;
        camera.target = { map_center_x, map_center_y };
        target_position = camera.target;
    }

    if (IsKeyPressed(KEY_R)) { //Pressing R key to load the 128*128 map
        medium_tilemap = loadTileMapFromJSON("tilemap.json");
        current_tilemap = &medium_tilemap;
        map_center_x = (float)(current_tilemap->width * TILE_SIZE * ZOOM_MUTLIPLE) / 2.0f;
        map_center_y = (float)(current_tilemap->height * TILE_SIZE * ZOOM_MUTLIPLE) / 2.0f;
        camera.target = { map_center_x, map_center_y };
        target_position = camera.target;
    }

    Vector2 particle_position = { (float)(current_tilemap->width * TILE_SIZE * ZOOM_MUTLIPLE) / 2.0f ,
        (float)(current_tilemap->height * TILE_SIZE * ZOOM_MUTLIPLE) / 2.0f };
    spawn_particle({ (float)(rand() % 50 - 25), (float)(rand() % 50 - 25) }, 70.0f);//Velocity(100,50), life_time 5 seconds
    update_particles(GetFrameTime());
}

void App::render()
{
    BeginDrawing();
    BeginMode2D(camera);
    ClearBackground(RAYWHITE);

    int bg_width = background.width * 2;
    int bg_height = background.height * 2;
    float cam_x = camera.target.x;
    float cam_y = camera.target.y;
    //background image scrolls according to the movement of the camera position

    static float smooth_bg_x = cam_x;
    static float smooth_bg_y = cam_y;

    smooth_bg_x = Damp(smooth_bg_x, cam_x, 2.0f, GetFrameTime());
    smooth_bg_y = Damp(smooth_bg_y, cam_y, 2.0f, GetFrameTime());

    float bg_offset_x = cam_x - fmodf(cam_x, (float)bg_width);
    float bg_offset_y = cam_y - fmodf(cam_y, (float)bg_height);

    static float blend_factor = 0.0f;
    blend_factor = Damp(blend_factor, 1.0f, 2.0f, GetFrameTime());

    for (int y = -1; y <= 1; y++)//3x3 grid traversal
    {
        for (int x = -1; x <= 1; x++)
        {
            Vector2 bg_position = { bg_offset_x + x * bg_width, bg_offset_y + y * bg_height };
            Color old_color = { 255, 255, 255, (unsigned char)((1.0f - transition_alpha) * 255) };
            Color new_color = { 255, 255, 255, (unsigned char)(transition_alpha * 255) };

            DrawTextureEx(background_old, bg_position, 0.0f, 2.0f, old_color);
            DrawTextureEx(background, bg_position, 0.0f, 2.0f, new_color);
        }
    }

    if (culling_enabled) {
        current_tilemap->render_culling_chunks(spritesheet, camera, Apptileset, perform_culling); //mod202501
    }
    else {
        current_tilemap->render_visible_chunks(spritesheet, camera, Apptileset);
    }

    DrawText(TextFormat("Coins: %d", player_coins), 10, 10, 20, YELLOW);

    DrawSprite(spritesheet, player_sprite, player_position, 2.0f, WHITE);
    render_particles(camera);
    EndMode2D();
    DrawFPS(10, 10);
    EndDrawing();
}

App::~App() {
    UnloadTexture(spritesheet);
    UnloadTexture(background);
    UnloadTexture(BackgroundOne);
    UnloadTexture(BackgroundThree);
    particles.clear();
}

int main()
{
    const int screenWidth = 1200;
    const int screenHeight = 900;

    InitWindow(screenWidth, screenHeight, "zhehan_hu_render_assignment");

    App app;
    app.init();

    while (!WindowShouldClose()) {
        app.update();
        app.render();
    }

    CloseWindow();
    return 0;
}