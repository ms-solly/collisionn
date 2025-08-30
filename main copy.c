// A small, readable, data-oriented example using raylib
#include <raylib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>

// ------------------------------------------------------------
// Config (keep all tunables here)
// ------------------------------------------------------------
#define SCREEN_WIDTH 1200
#define SCREEN_HEIGHT 600

// Logical ground height (a flat floor at the bottom of the screen)
#define GROUND_HEIGHT 35.0f // previously P_HEIGHT

// Source sprite frame size (square spritesheets)
#define CFRAME_SIZE 32.0f

// Physics
#define GRAVITY 9.8f
#define JUMP_VELOCITY -45.0f
#define MOVE_SPEED 200.0f

// ------------------------------------------------------------
// Animation primitives
// ------------------------------------------------------------
typedef enum AnimationType {
  REPEATING = 1,
  ONESHOT = 2,
} AnimationType;

typedef enum Direction {
  // used to flip the width component of a sprite's source rectangle, hence the
  // -1
  LEFT = -1,
  RIGHT = 1,
} Direction;


// the range domain of the animation is [first, last], inclusive
typedef struct Animation {
  int first;           // index of first frame
  int last;            // index of last frame
  int cur;             // current frame index
  int step;            // index increment each frame (1 for forward, -1 for reverse)
  float speed;         // seconds per frame
  float time_left;     // time remaining on current frame
  AnimationType type;  // repeat vs one-shot
} Animation;

typedef struct SpriteSheet {
  Texture2D texture;
  int frames_per_row;
  int frame_size; // pixels (square source frames)
} SpriteSheet;

static inline void animation_update(Animation *self) {
  float dt = GetFrameTime();
  self->time_left -= dt;

  if (self->time_left > 0.0f) return;

  // advance frame and reset timer
  self->time_left += self->speed; // carry over small negative remainder
  self->cur += self->step;

  // unified boundary handling using sign of step
  if (self->step > 0 && self->cur > self->last) {
    if (self->type == REPEATING) self->cur = self->first; else self->cur = self->last;
  } else if (self->step < 0 && self->cur < self->first) {
    if (self->type == REPEATING) self->cur = self->last; else self->cur = self->first;
  }
}

static inline Rectangle animation_src_rect(const Animation *self, const SpriteSheet *sheet) {
  // Convert linear index -> sprite sheet coordinates
  int x = (self->cur % sheet->frames_per_row) * sheet->frame_size;
  int y = (self->cur / sheet->frames_per_row) * sheet->frame_size;
  return (Rectangle){ (float)x, (float)y, (float)sheet->frame_size, (float)sheet->frame_size };
}

// ------------------------------------------------------------
// Player
// ------------------------------------------------------------
typedef struct Player {
  Vector2 pos;
  Vector2 vel;
  bool on_ground;
  Direction facing;
  SpriteSheet sheet;
  Animation anim;
} Player;

static inline float ground_y(void) {
  return SCREEN_HEIGHT - GROUND_HEIGHT - CFRAME_SIZE; // 32px tall sprite
}

static void player_init(Player *p, Texture2D texture) {
  p->pos = (Vector2){10, ground_y()};
  p->vel = (Vector2){0, 0};
  p->on_ground = true;
  p->facing = RIGHT;
  p->sheet = (SpriteSheet){ .texture = texture, .frames_per_row = 4, .frame_size = (int)CFRAME_SIZE };
  p->anim = (Animation){ .first = 0, .last = 3, .cur = 0, .step = 1, .speed = 0.1f, .time_left = 0.1f, .type = REPEATING };
}

static void player_input(Player *p) {
  float dt = GetFrameTime();
  float dx = 0.0f;
  if (IsKeyDown(KEY_LEFT) || IsKeyDown(KEY_A)) { dx -= MOVE_SPEED * dt; p->facing = LEFT; }
  if (IsKeyDown(KEY_RIGHT) || IsKeyDown(KEY_D)) { dx += MOVE_SPEED * dt; p->facing = RIGHT; }
  p->pos.x += dx;

  if (IsKeyPressed(KEY_SPACE) && p->on_ground) {
    p->vel.y = JUMP_VELOCITY;
    p->on_ground = false;
  }
}

static void player_physics(Player *p) {
  float dt = GetFrameTime();
  if (!p->on_ground) {
    p->vel.y += GRAVITY * dt;
    p->pos.y += p->vel.y * dt;
    // floor collision
    if (p->pos.y >= ground_y()) {
      p->pos.y = ground_y();
      p->vel.y = 0.0f;
      p->on_ground = true;
    }
  }

  // keep within screen horizontally
  if (p->pos.x < 0) p->pos.x = 0;
  if (p->pos.x > SCREEN_WIDTH - CFRAME_SIZE) p->pos.x = SCREEN_WIDTH - CFRAME_SIZE;
}

static void player_update(Player *p) {
  player_input(p);
  player_physics(p);
  animation_update(&p->anim);
}

static void player_draw(const Player *p) {
  Rectangle src = animation_src_rect(&p->anim, &p->sheet);
  src.width *= (float)p->facing; // flip when facing left
  Rectangle dst = { p->pos.x, p->pos.y, CFRAME_SIZE, CFRAME_SIZE };
  DrawTexturePro(p->sheet.texture, src, dst, (Vector2){0,0}, 0.0f, WHITE);
}

// ------------------------------------------------------------
// Simple actor: Broccoli sprite
// ------------------------------------------------------------
typedef struct ActorSprite {
  Vector2 pos;
  SpriteSheet sheet;
  Animation anim;
} ActorSprite;

static inline void actor_update(ActorSprite *a) {
  animation_update(&a->anim);
}

static inline void actor_draw(const ActorSprite *a) {
  Rectangle src = animation_src_rect(&a->anim, &a->sheet);
  Rectangle dst = { a->pos.x, a->pos.y, CFRAME_SIZE, CFRAME_SIZE };
  DrawTexturePro(a->sheet.texture, src, dst, (Vector2){0,0}, 0.0f, WHITE);
}

int main(void) {
  InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Raylib demo - data oriented");
  SetTargetFPS(60);

  // Load textures once, own them in small structs
  Texture2D texPlayer = LoadTexture("resources/chomp_idle.png");
  Texture2D texBroccoli = LoadTexture("resources/Vegetables_/One/Broccoli.png");

  Player player; 
  player_init(&player, texPlayer);

  ActorSprite broccoli = {
    .pos = (Vector2){200, ground_y()},
    .sheet = (SpriteSheet){ .texture = texBroccoli, .frames_per_row = 4, .frame_size = (int)CFRAME_SIZE },
    .anim = (Animation){ .first = 0, .last = 0, .cur = 0, .step = 1, .speed = 0.2f, .time_left = 0.2f, .type = REPEATING }
  };

  while (!WindowShouldClose()) {
    // Update
    player_update(&player);
    actor_update(&broccoli);

    // Draw
    BeginDrawing();
      ClearBackground(GREEN);

      // Ground
      DrawRectangleV((Vector2){0, SCREEN_HEIGHT - GROUND_HEIGHT}, (Vector2){SCREEN_WIDTH, GROUND_HEIGHT}, GRAY);

      // Entities
      player_draw(&player);
      actor_draw(&broccoli);

      // UI
      DrawText("SPACE = jump", 10, 10, 20, BLACK);
      DrawText("A/D or Left/Right = move", 10, 40, 20, BLACK);
      DrawFPS(10, SCREEN_HEIGHT - 30);
    EndDrawing();
  }

  // Cleanup
  UnloadTexture(texPlayer);
  UnloadTexture(texBroccoli);

  CloseWindow();
  return 0;
}
