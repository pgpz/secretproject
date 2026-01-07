#include "raylib.h"
#include "rcamera.h"
#include "rlgl.h"
#include "raymath.h"
#define MAX_COLUMNS 20

#if defined(PLATFORM_DESKTOP)
    #define GLSL_VERSION 330
#else
    #define GLSL_VERSION 100
#endif

Model skybox = { 0 };
Shader shdrCubemap = { 0 };
char skyboxFilename[256] = { 0 };
bool useHDR = false;

Shader pixelShader = { 0 };
RenderTexture2D target = { 0 };


static void DrawCrosshair(int screenWidth, int screenHeight)
{
    int cx = screenWidth / 2;
    int cy = screenHeight / 2;

    int size  = 10;
    int gap   = 4;
    int thick = 2;

    Color color = RAYWHITE;

    // horizontal
    DrawRectangle(cx - size - gap, cy - thick/2, size, thick, color);
    DrawRectangle(cx + gap,        cy - thick/2, size, thick, color);

    // vertical
    DrawRectangle(cx - thick/2, cy - size - gap, thick, size, color);
    DrawRectangle(cx - thick/2, cy + gap,        thick, size, color);
}



int main(void)
{
    const int screenWidth = 1920;
    const int screenHeight = 1080;

    InitWindow(screenWidth, screenHeight, "camera test");
    target = LoadRenderTexture(screenWidth, screenHeight);
    SetTextureFilter(target.texture, TEXTURE_FILTER_POINT);


    InitAudioDevice();

    Music music = LoadMusicStream("resources/leekspin.mp3");

    PlayMusicStream(music);

    float timePlayed = 0.0f;
    bool pause = false;
    float pan = 0.0f;
    SetMusicPan(music, pan);
    float volume = 0.3f;
    SetMusicVolume(music, volume);
    

    pixelShader = LoadShader(
    0,
    TextFormat("resources/shaders/glsl%i/pixelizer.fs", GLSL_VERSION)
    );

    Camera camera = { 0 };
    camera.position = (Vector3){ 0.0f, 2.0f, 4.0f };
    camera.target = (Vector3){ 0.0f, 2.0f, 0.0f };
    camera.up = (Vector3){ 0.0f, 1.0f, 0.0f };
    camera.fovy = 75.0f;
    camera.projection = CAMERA_PERSPECTIVE;

    int cameraMode = CAMERA_FIRST_PERSON;

    float heights[MAX_COLUMNS] = { 0 };
    Vector3 positions[MAX_COLUMNS] = { 0 };
    Color colors[MAX_COLUMNS] = { 0 };

    for (int i = 0; i < MAX_COLUMNS; i++)
    {
        heights[i] = (float)GetRandomValue(1, 12);
        positions[i] = (Vector3){ (float)GetRandomValue(-15, 15), heights[i]/2.0f, (float)GetRandomValue(-15, 15) };
        colors[i] = (Color){ GetRandomValue(20, 255), GetRandomValue(10, 55), 20, 255 };
    }

    DisableCursor();

    SetTargetFPS(240);

    while (!WindowShouldClose())
    {

        UpdateMusicStream(music);
        if (IsKeyPressed(KEY_ONE))
        {
            cameraMode = CAMERA_FREE;
            camera.up = (Vector3){ 0.0f, 1.0f, 0.0f };
        }

        if (IsKeyPressed(KEY_TWO))
        {
            cameraMode = CAMERA_FIRST_PERSON;
            camera.up = (Vector3){ 0.0f, 1.0f, 0.0f };
        }

        if (IsKeyPressed(KEY_THREE))
        {
            cameraMode = CAMERA_THIRD_PERSON;
            camera.up = (Vector3){ 0.0f, 1.0f, 0.0f }; 
        }

        if (IsKeyPressed(KEY_FOUR))
        {
            cameraMode = CAMERA_ORBITAL;
            camera.up = (Vector3){ 0.0f, 1.0f, 0.0f };
        }
        if (IsKeyDown(KEY_DOWN))
        {
            volume -= 0.05f;
            if (volume < 0.0f) volume = 0.0f;
            SetMusicVolume(music, volume);
        }
        else if (IsKeyDown(KEY_UP))
        {
            volume += 0.05f;
            if (volume > 1.0f) volume = 1.0f;
            SetMusicVolume(music, volume);
        }

        if (IsKeyPressed(KEY_P))
        {
            if (camera.projection == CAMERA_PERSPECTIVE)
            {
                cameraMode = CAMERA_THIRD_PERSON;
                camera.position = (Vector3){ 0.0f, 2.0f, -100.0f };
                camera.target = (Vector3){ 0.0f, 2.0f, 0.0f };
                camera.up = (Vector3){ 0.0f, 1.0f, 0.0f };
                camera.projection = CAMERA_ORTHOGRAPHIC;
                camera.fovy = 20.0f; 
                CameraYaw(&camera, -135*DEG2RAD, true);
                CameraPitch(&camera, -45*DEG2RAD, true, true, false);
            }
            else if (camera.projection == CAMERA_ORTHOGRAPHIC)
            {
                cameraMode = CAMERA_THIRD_PERSON;
                camera.position = (Vector3){ 0.0f, 2.0f, 10.0f };
                camera.target = (Vector3){ 0.0f, 2.0f, 0.0f };
                camera.up = (Vector3){ 0.0f, 1.0f, 0.0f };
                camera.projection = CAMERA_PERSPECTIVE;
                camera.fovy = 60.0f;
            }
        }

        timePlayed = GetMusicTimePlayed(music)/GetMusicTimeLength(music);
        if (timePlayed > 1.0f) timePlayed = 1.0f;


        UpdateCamera(&camera, cameraMode);
	
        BeginTextureMode(target);

            ClearBackground(BLACK);

            BeginMode3D(camera);


                DrawPlane((Vector3){ 0.0f, 0.0f, 0.0f }, (Vector2){ 32.0f, 32.0f }, RED); 
                DrawCube((Vector3){ -16.0f, 2.5f, 0.0f }, 1.0f, 5.0f, 32.0f, BLACK);
                DrawCube((Vector3){ 16.0f, 2.5f, 0.0f }, 1.0f, 5.0f, 32.0f, BLACK);
                DrawCube((Vector3){ 0.0f, 2.5f, 16.0f }, 32.0f, 5.0f, 1.0f, BLACK);

                for (int i = 0; i < MAX_COLUMNS; i++)
                {
                    DrawCube(positions[i], 2.0f, heights[i], 2.0f, colors[i]);
                    DrawCubeWires(positions[i], 2.0f, heights[i], 2.0f, MAROON);
                }

                if (cameraMode == CAMERA_THIRD_PERSON)
                {
                    DrawCube(camera.target, 0.5f, 0.5f, 0.5f, PURPLE);
                    DrawCubeWires(camera.target, 0.5f, 0.5f, 0.5f, DARKPURPLE);
                }

            EndMode3D();
        EndTextureMode();

            

           BeginDrawing();
                ClearBackground(BLACK);

                BeginShaderMode(pixelShader);
                  DrawTextureRec(
                      target.texture,
                      (Rectangle){ 0, 0, (float)target.texture.width, -(float)target.texture.height },
                      (Vector2){ 0, 0 },
                      WHITE
                  );

          EndShaderMode();

          DrawCrosshair(screenWidth, screenHeight);

          

        EndDrawing();
    }

    UnloadMusicStream(music);
    CloseAudioDevice();
    CloseWindow();

    return 0;
}

