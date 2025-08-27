#include <raylib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>

#define SCREEN_WIDTH 1200
#define SCREEN_HEIGHT 600

#define FRAME_SIZE 100.0f
#define CFRAME_SIZE 32.0f
#define BFRAME_SIZE 290.0f
#define B_FRAME_SIZE 250.0f



#define GRAVITY 9.8f
#define TIME_STEP 0.1f
#define P_HEIGHT 100.0f
#define JUMP_VELOCITY -45.0f

typedef struct
{
  float x,y;
  struct {
    float x,y;
  } velocity;
  bool onGround;
  Texture2D player_texture;
}Player;

enum Entities{
  PLAYER = 0,
  CROP ,
  BUDDY ,
  FOOD,
}Entities;
typedef enum AnimationType {
  REPEATING = 0,
  ONESHOT ,
} AnimationType;

typedef enum Direction {
  // used to flip the width component of a sprite's source rectangle, hence the
  // -1
  LEFT = -1,
  RIGHT = 1,
} Direction;

// the range domain of the animation if [first, last], inclusive on both ends
typedef struct Animation {
  // the index of the first frame
  int first;
  // the index of the last frame
  int last;
  // the current frame index
  int cur;
  // how far we jump when going to the next frame. For a normal, sequential
  // anim, this is 1
  int step;

  // how long each frame lasts
  float speed;
  // how much time is left on the current frame
  float duration_left;

  AnimationType type;
} Animation;

void animation_update(Animation *self) {
  float dt = GetFrameTime();
  self->duration_left -= dt;

  if (self->duration_left <= 0.0) {
    // go to the next frame and reset the state of the animation
    self->duration_left = self->speed;
    self->cur++;

    // CHALLENGE: find a way to turn these two if statements into one
    // (hint. use the value of step in the if statement?)
    if (self->cur > self->last) {
      // handle reaching the end of the animation going forwards
      switch (self->type) {
      case REPEATING:
        self->cur = self->first;
        break;
      case ONESHOT:
        self->cur = self->last;
        break;
      }
    } else if (self->cur <= self->first) {
      // handle reaching the end (which is the beginning) of an animation going
      // backwards
      switch (self->type) {
      case REPEATING:
        self->cur = self->last;
        break;
      case ONESHOT:
        self->cur = self->first;
        break;
      }
    }
  }
}

Rectangle animation_frame(Animation *self, int num_frames_per_row, int frame_size) {
  // memorize this! Converting from index -> coord and vice versa is extremely
  // common in game development For example, Minecraft stores a chunk in a 1D
  // array. The coordinates are retrieved using this method (with a third
  // dimension of course).
  int x = (self->cur % num_frames_per_row) * frame_size;
  int y = (self->cur / num_frames_per_row) * frame_size;

  return (Rectangle){
      .x = (float)x,
      .y = (float)y,
      .width = frame_size,
      .height = frame_size};
}

void LoadAssets()
{
    // Image angel = LoadImage("/resources/angelmouse.ase");
    // Texture2D player_texture = LoadTextureFromImage(angel);
}

void updatePlayer(Player *pl)
{
    float ground_y = SCREEN_HEIGHT - P_HEIGHT - FRAME_SIZE;

    // Gravity only if not on ground
    if (!pl->onGround)
    {
        pl->velocity.y += GRAVITY * TIME_STEP;   // Apply gravity
        pl->y += pl->velocity.y * TIME_STEP;     // Update position

        // Ground collision
        if (pl->y >= ground_y) {
            pl->y = ground_y;
            pl->velocity.y = 0;
            pl->onGround = true;
        }
    }

    // Horizontal movement
    if (IsKeyDown(KEY_LEFT) || IsKeyDown(KEY_A)) {
        pl->x -= 200 * GetFrameTime();
    }
    if (IsKeyDown(KEY_RIGHT) || IsKeyDown(KEY_D)) {
        pl->x += 200 * GetFrameTime();
    }

    // Clamp player within screen
    if (pl->x < 0) pl->x = 0;
    if (pl->x > SCREEN_WIDTH - FRAME_SIZE) pl->x = SCREEN_WIDTH - FRAME_SIZE;
  
}

int main() {
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "C raylib template");
    SetTargetFPS(60);
    Player player;
    player.x = 10;
    player.y = SCREEN_HEIGHT - P_HEIGHT - FRAME_SIZE -120;
    player.velocity.x = 0;
    player.velocity.y = 0;
    player.onGround = true;
    // when no need to modify image should directly load the texture saving cpu memory of image space
    // player.player_texture = LoadTexture("resources/angelcatt.png");
    Texture2D angel_texture = LoadTexture("resources/angelcatt.png");
    player.player_texture = LoadTexture("resources/chomp_idle.png");
    Texture2D brocolli_texture = LoadTexture("resources/brocollli.png");
    Texture2D brocolli_char_texture = LoadTexture("resources/brocolli_char.png");
        // Animation anim = (Animation){
        //     .first = 0,
        //     .last = 3,
        //     .cur = 0,
        //     .step = 1,
        //     .speed = 0.1,
        //     .duration_left = 0.1,
        //     .type = REPEATING,
        // };

        // Animation anim2 = (Animation){
        //     .first = 0,
        //     .last = 3,
        //     .cur = 0,
        //     .step = 1,
        //     .speed = 0.1,
        //     .duration_left = 0.1,
        //     .type = ONESHOT,
        // };

        // Animation reverse_anim = (Animation){
        //     .first = 0,
        //     .last = 3,
        //     .cur = 3,
        //     .step = -1,
        //     .speed = 0.1,
        //     .duration_left = 0.1,
        //     .type = REPEATING,
        // };
        Animation anim = {0, 3, 0, 1, 0.1, 0.1, REPEATING};
        Animation anim2 = {0, 3, 0, 1, 0.1, 0.1, ONESHOT};
        Animation reverse_anim = {0, 3, 3, -1, 0.1, 0.1, REPEATING};
        Animation chomp_anim = {0, 34, 0, 1, 0.1, 0.1, REPEATING};
        Animation brocolli_anim = {0, 3, 0, 1, 0.2, 0.2, REPEATING};
        Animation brocolli_char_anim = {0, 3, 0, 1, 0.2, 0.2, REPEATING};
        Direction player_direction = LEFT;



    // // 
    // Image angel = LoadImage("resources/angelcatt.png");
    // Texture2D player_texture = LoadTextureFromImage(angel);
    // UnloadImage(angel); 

    while (!WindowShouldClose())
    {
    // if (IsKeyPressed(KEY_SPACE)) {
    //   anim.cur = anim.first;
    // }

    // if (IsKeyPressed(KEY_LEFT || KEY_A)) {
    //   player_direction = LEFT;
    // }
    // if (IsKeyPressed(KEY_RIGHT || KEY_D)) {
    //   player_direction = RIGHT;
    // }
    // if (IsKeyPressed(KEY_X))
    // {
    //     anim2.cur= anim2.first;
    // }
            if (IsKeyPressed(KEY_SPACE) && player.onGround) {
            player.velocity.y = JUMP_VELOCITY;
            player.onGround = false;
        }

        if (IsKeyPressed(KEY_LEFT) || IsKeyPressed(KEY_A)) {
            player_direction = LEFT;
        }
        if (IsKeyPressed(KEY_RIGHT) || IsKeyPressed(KEY_D)) {
            player_direction = RIGHT;
        }
        if (IsKeyPressed(KEY_X)) {
            anim2.cur = anim2.first;
        }

        animation_update(&anim);
        animation_update(&anim2);
        animation_update(&chomp_anim);
        animation_update(&brocolli_anim);
        animation_update(&brocolli_char_anim);

        updatePlayer(&player);


        BeginDrawing();

        ClearBackground(GREEN);
/** 
        // DrawTextureV(player_texture,(Vector2){10,10},WHITE);
        DrawRectangleV((Vector2){0,GetScreenHeight() - P_HEIGHT},(Vector2){GetScreenWidth(), P_HEIGHT}, GRAY);


        Rectangle player_frame = animation_frame(&anim, 4);
        player_frame.width *= player_direction;
        Rectangle kk = {10, 10, 100, 100};
        Rectangle kk2 = {200, 10, 100, 100};

        // playing animation
        DrawTexturePro(player.player_texture, player_frame, kk, (Vector2){0, 0}, 0.0f, WHITE);

        // attack animation
        // DrawTexturePro(player.player_texture, animation_frame(&anim2, 4), kk2, (Vector2){0, 0}, 0.0f, WHITE);
        DrawTexturePro(player.player_texture, 
          animation_frame(&anim, 4),
          (Rectangle){10, 10, 100, 100}, 
          (Vector2){0, 0}, 0.0f,
           WHITE);
**/

        DrawRectangleV((Vector2){0, SCREEN_HEIGHT - P_HEIGHT}, (Vector2){SCREEN_WIDTH, P_HEIGHT}, GRAY);

        // Angel animation
        Rectangle player_frame = animation_frame(&anim, 4, CFRAME_SIZE);
        player_frame.width *= player_direction; // Flip horizontally
        Rectangle angel_rect = {player.x , player.y, CFRAME_SIZE +200, CFRAME_SIZE+200};
        DrawTexturePro(player.player_texture, player_frame, angel_rect, (Vector2){0, 0}, 0.0f, WHITE);

        Rectangle brocolli_frame = animation_frame(&brocolli_anim, 4, BFRAME_SIZE);
        Rectangle brocolli_rect = {-player.x+300, player.y - 200, BFRAME_SIZE, BFRAME_SIZE};
        DrawTexturePro(brocolli_texture, brocolli_frame, brocolli_rect, (Vector2){200, 0}, 0.0f, WHITE);

        Rectangle brocollichar_frame = animation_frame(&brocolli_char_anim, 4, B_FRAME_SIZE);
        Rectangle brocollichar_rect = {-player.x+500, player.y - 200, B_FRAME_SIZE, B_FRAME_SIZE};
        DrawTexturePro(brocolli_char_texture, brocollichar_frame, brocollichar_rect, (Vector2){200, 0}, 0.0f, WHITE);
        // Draw controls
        DrawText("Press SPACE to jusmp (Angel)", 10, 10, 20, BLACK);
        DrawText("Press A/D or LEFT/RIGHT to move (Angel)", 10, 40, 20, BLACK);
        DrawFPS(10, SCREEN_HEIGHT - 30);
        EndDrawing();

        
        }

    UnloadTexture(player.player_texture);
    // UnloadImage(angel);
    UnloadTexture(player.player_texture);
    CloseWindow();
    return 0;
}
