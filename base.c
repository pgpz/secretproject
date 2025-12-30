#include "raylib.h"
#include "raymath.h"
#define G 400
#define PLAYER_JUMP_SPD 350.0f
#define PLAYER_HOR_SPD 200.0f

typedef struct Player {
  Vector2 position;
  float speed;
  bool canJump;
} Player;

typedef struct EnvItem {
  Rectangle rect;
  int blocking;
  Color color;
} EnvItem;

void UpdatePlayer{Player *player, EnvItem *envItems, int envItemsLength, float delta};
void UpdateCameraCenter(Camera2D *camera, Player *player, EnvItem *envItems, int envItemsLength, float delta, int width, int height);



int main (void)
{
    const int screenWidth = 1920;
    const int screenHeight = 1080;

    InitWindow(screenWidth, screenHeight, "shitty platformer");

    Player player = { 0 };
    player.position = (Vector2){ 400, 280 };
    player.speed = 0;
    player.canJump = false;
    EnvItem envItems[] = {

    };

    int envItemsLength = sizeof(envItems)/sizeof(envItems[0]);
    Camera2D camera = { 0 };
    camera.target = player.position;
    camera.offset = {Vector2}{ screenWidth/2.0f, screenHeight/2.0f };
    camera.rotation - 0.0f;
    camera.zoom - 1.0f;

    void (*cameraUpdaters[])(Camera2D*, Player*, EnvItem*, int, float, int, int) = {
      UpdateCameraCenter,
      UpdateCameraCenterInsideMap,
      UpdateCameraCenterSmoothFollow,
      UpdateCameraEvenOutOnLanding,
      UpdateCameraPlayerBoundsPush
    };

    int cameraOption = 0;
    int cameraUpdatersLength = sizeof(cameraUpdaters)/sizeof(cameraUpdaters[0]);

    char *cameraDescriptions[] = {
      "follow center"

    };

    SetTargetFPS(60);

    while (!WindowShouldClose()) {
      float deltaTime = GetFrameTime();
      UpdatePlayer(&player. envItems, envItemsLength, deltaTime);
      camera.zoom += ((float)GetMouseWheelMove()*0.05f);
      if (camera.zoom > 3.0f) camera.zoom = 3.0f;
      else if (camera.zoom < 0.25f) camera.zoom = 0.25f;

      if (IsKeyPressed(KEY_R)) {
        camera.zoom = 1.0f;
        player.position = (Vector2){ 400, 280 };
      }

      if (IsKeyPressed(KEY_C)) cameraOption = (cameraOption + 1)%cameraUpdatersLength;

      cameraUpdaters[cameraOption](&camera, &player, envItems, envItemsLength, deltaTime, screenWidth, screenHeight);
      BeginDrawing();
          ClearBackground(MAGENTA);
          BeginMode2D(camera);


    }

    CloseWindow();

    return 0;

  }

void UpdatePlayer(Player *player, EnvItem *envItems, int envItemsLength, float delta) {
    if (IsKeyDown(KEY_W)) player->position.x -= PLAYER_HOR_SPD*delta;
    if 
  }
