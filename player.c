#pragma once
typedef struct
{
  float x,y;
  struct {
    float x,y;
  } velocity;
  bool onGround;
  Texture2D player_texture;
}Player;

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

