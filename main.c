#include <raylib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>

#define SCREEN_WIDTH 1200
#define SCREEN_HEIGHT 600

//--------frame sizes will make them customizable later instead of using multiple micros will use only one macro(all sprites same sized) or will manually enter the size of frame. not sure -------
#define FRAME_SIZE 100.0f
#define CFRAME_SIZE 32.0f
#define BFRAME_SIZE 290.0f
#define B_FRAME_SIZE 250.0f


#define GRAVITY 9.8f
#define TIME_STEP 0.1;
#define P_HEIGHT 300.0f
#define JUMP_VELOCITY -45.0f
#define MAX_BUILDINGS   100


#define MAX_ANIMALS 50
#define MAX_CROPS 100


// ---- crop growth rates ----
#define GROWTH_RATE_PLANTED 0.001f
#define GROWTH_RATE_SEEDLING 0.002f
#define GROWTH_RATE_GROWING 0.003f
#define GROWTH_RATE_MATURE 0.001f

#define SUPPORT_FILEFORMAT_MP3// too enable this audio format
//--------types---------
typedef struct Position{
    float x,y,z;
}Position;
typedef struct Velocity{
    float x,y,z;
}Velocity;
typedef enum Direction {
  // used to flip the width component of a sprite's source rectangle, hence the -1
    LEFT = -1,
    RIGHT = 1,
} Direction;
typedef enum AnimationType {
    REPEATING = 0,
    ONESHOT ,
} AnimationType;

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

//---------data--------------

// -------- entities-----------
typedef struct PlayerData{
    //Position position;
    Vector2 position;
    Velocity velocity;
    bool onGround;
    Texture2D player_texture;
    Rectangle frame;
    int selected_item;
    Animation anim;
    Direction direction;
}PlayerData;

typedef enum Entities{
    PLAYER = 0,
    ANIMAL,
    CROP ,
    BUDDY ,
    FOOD,
}Entities;

typedef struct AnimalData{
    Vector2 position[MAX_ANIMALS];
    Texture2D texture[MAX_ANIMALS];
    Velocity velocity[MAX_ANIMALS];
    float move_timer[MAX_ANIMALS];
    float direction_change_time[MAX_ANIMALS];
    //char name[20];
    float happiness[MAX_ANIMALS];
    float hunger[MAX_ANIMALS];
    bool isFed[MAX_ANIMALS];
    Rectangle interactionZone[MAX_ANIMALS];
    Animation anims[MAX_ANIMALS];
    bool isActive[MAX_ANIMALS];
    int count;
}AnimalData;
typedef enum CropState {
    CROP_PLANTED = 0,
    CROP_SEEDLING,
    CROP_GROWING,
    CROP_MATURE,
    CROP_READY,
    CROP_EMPTY
} CropState;
typedef struct CropData {
    Vector2 position[MAX_CROPS];
    CropState state[MAX_CROPS];
    float growth[MAX_CROPS];
    //Texture2D texture;
    Animation anims[MAX_CROPS];
    bool isActive[MAX_CROPS];
    int count;
} CropData;

typedef struct GameState {
    AnimalData animal;
    CropData crop;
    PlayerData player;
    Camera2D camera;
    Texture2D animalTexture;
    Texture2D cropTexture;
    Texture2D playerTexture;

    Rectangle buildings[MAX_BUILDINGS];
    Color buildColor[MAX_BUILDINGS];
    int buildingCount;
} GameState;


//--------items-----------
typedef struct Grass {
    Vector2 position;
    Color color;
    float width;
    float height;
} Grass;
typedef struct GrassField {
    Grass *grasses;
    int count;
} GrassField;


// ------ data modifiers ----------
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
      .width = (float)frame_size,
      .height = (float)frame_size};
}


/**
void updatePlayer(Player *pl)
{
    float ground_y = P_HEIGHT - (CFRAME_SIZE*2);

    // Gravity only if not on ground
    if (!pl->onGround)
    {
        pl->velocity.y += GRAVITY * TIME_STEP;   // Apply gravity
        pl->y += pl->velocity.y * TIME_STEP;     // Update position

        // Ground collision
        if (pl->y +64 >= P_HEIGHT) {
            pl->y = P_HEIGHT-64.0f;
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
    if (IsKeyDown(KEY_UP) || IsKeyDown(KEY_W)) {
        pl->y -= 200 * GetFrameTime();
    }
    if (IsKeyDown(KEY_DOWN) || IsKeyDown(KEY_S)) {
        pl->y += 200 * GetFrameTime();
    }
    // Clamp player within screen
//    if (pl->x < 0) pl->x = 0;
//    if (pl->x > SCREEN_WIDTH - FRAME_SIZE) pl->x = SCREEN_WIDTH - FRAME_SIZE;
  
}
**/
void init_game_state(GameState *state){
    state->player.position = (Vector2){40, P_HEIGHT - CFRAME_SIZE * 2};
    state->player.anim = (Animation){0, 3, 0, 1, 0.1, 0.1, REPEATING};
    state->player.direction = RIGHT;
    state->player.selected_item = 0;

    state->camera.target = state->player.position;
    state->camera.offset = (Vector2){SCREEN_WIDTH / 2.0f, SCREEN_HEIGHT / 2.0f};
    state->camera.rotation = 0.0f;
    state->camera.zoom = 1.0f;

    state->animal.count = 0;
    state->crop.count = 0;
   

    state->playerTexture = LoadTexture("resources/chomp_idle.png");
    state->animalTexture = LoadTexture("resources/bunny.png");
    state->cropTexture = LoadTexture("resources/plant.png");

    // Add initial animal
    for (int i = 0; i < 3 && state->animal.count < MAX_ANIMALS; i++) {
        int idx = state->animal.count;
        state->animal.position[idx] = (Vector2){(float)300 + i * 150, P_HEIGHT - CFRAME_SIZE * 2};
        state->animal.happiness[idx] = 0.5f + (i * 0.1f);
        state->animal.hunger[idx] = 0.7f - (i * 0.1f);
        state->animal.anims[idx] = (Animation){0, 3, 0, 1, 0.2, 0.2, REPEATING};
        state->animal.interactionZone[idx] = (Rectangle){
            state->animal.position[idx].x - 50,
            state->animal.position[idx].y - 50,
            100, 100
        };
        state->animal.isActive[idx] = true;
        state->animal.count++;
    }

    // Add initial plants
    for (int i = 0; i < 5 && state->crop.count < MAX_CROPS; i++) {
        int idx = state->crop.count;
        state->crop.position[idx] = (Vector2){400 + i * 80,P_HEIGHT - CFRAME_SIZE * 2};
        state->crop.state[idx] = CROP_GROWING;
        state->crop.growth[idx] = 0.2f * (i + 1);
        state->crop.anims[idx] = (Animation){0, 2, 0, 1, 0.3, 0.3, REPEATING};
        state->crop.isActive[idx] = true;
        state->crop.count++;
    }
    
    // Create background buildings
    state->buildingCount = MAX_BUILDINGS;
    int spacing = 0;
    
    for (int i = 0; i < state->buildingCount; i++) {
        state->buildings[i].width = (float)GetRandomValue(50, 200);
        state->buildings[i].height = (float)GetRandomValue(100, 800);
        state->buildings[i].y = SCREEN_HEIGHT - 300.0f - state->buildings[i].height;
        state->buildings[i].x = -6000.0f + spacing;
        
        spacing += (int)state->buildings[i].width;
        
        state->buildColor[i] = (Color){
            (unsigned char)GetRandomValue(200, 240),
            (unsigned char)GetRandomValue(200, 240),
            (unsigned char)GetRandomValue(200, 250),
            255};
    }


}
int add_animal(GameState *state, Vector2 position) {
    if (state->animal.count < MAX_ANIMALS) {
        int idx = state->animal.count;
        state->animal.position[idx] = position;
        state->animal.happiness[idx] = 0.5f;
        state->animal.hunger[idx] = 0.7f;
        state->animal.anims[idx] = (Animation){0, 3, 0, 1, 0.2, 0.2, REPEATING};
        state->animal.interactionZone[idx] = (Rectangle){
            position.x - 50,
            position.y - 50,
            100, 100
        };
        state->animal.isActive[idx] = true;
        state->animal.count++;
        return idx;
    }
    return -1;
}
int add_crop(GameState *state, Vector2 position) {
    if (state->crop.count < MAX_CROPS) {
        int idx = state->crop.count;
        state->crop.position[idx] = position;
        state->crop.state[idx] = CROP_SEEDLING;
        state->crop.growth[idx] = 0.0f;
        state->crop.anims[idx] = (Animation){0, 2, 0, 1, 0.3, 0.3, REPEATING};
        state->crop.isActive[idx] = true;
        state->crop.count++;
        return idx;
    }
    return -1;
}
void update_player(PlayerData *player) {
    float speed = 200.0f * GetFrameTime();

    if (IsKeyDown(KEY_LEFT) || IsKeyDown(KEY_A)) {
        player->position.x -= speed;
        player->direction = LEFT;
    }
    if (IsKeyDown(KEY_RIGHT) || IsKeyDown(KEY_D)) {
        player->position.x += speed;
        player->direction = RIGHT;
    }
    if (IsKeyDown(KEY_UP) || IsKeyDown(KEY_W)) {
        player->position.y -= speed;
    }
    if (IsKeyDown(KEY_DOWN) || IsKeyDown(KEY_S)) {
        player->position.y += speed;
    }

    // Update animation
    animation_update(&player->anim);
}
void update_animal(AnimalData *animal) {
    for (int i = 0; i < animal->count; i++) {
        if (!animal->isActive[i]) continue;
        
        // Update needs (process all hunger values)
        animal->hunger[i] -= 0.0005f * GetFrameTime();
        if (animal->hunger[i] < 0) animal->hunger[i] = 0;
        
        // Update happiness based on hunger (process all happiness values)
        if (animal->hunger[i] < 0.3f) {
            animal->happiness[i] -= 0.0003f * GetFrameTime();
        }
        
        // Update animation (process all animations)
        animation_update(&animal->anims[i]);
        
        // Update interaction zone position (process all interaction zones)
        animal->interactionZone[i].x = animal->position[i].x - 50;
        animal->interactionZone[i].y = animal->position[i].y - 50;
    }
}

void update_crops(CropData *crop) {
    for (int i = 0; i < crop->count; i++) {
        if (!crop->isActive[i]) continue;
        
        // Update plant growth (process all growth values)
        if (crop->state[i] == CROP_GROWING) {
            crop->growth[i] += 0.001f * GetFrameTime();
            if (crop->growth[i] >= 1.0f) {
                crop->state[i] = CROP_READY;
            }
        }
        
        // Update animation (process all animations)
        animation_update(&crop->anims[i]);
    }
}

void check_interactions(GameState *state) {
    Rectangle playerRect = {
        state->player.position.x, 
        state->player.position.y, 
        CFRAME_SIZE * 2, 
        CFRAME_SIZE * 2
    };
    
    // Check animal interactions (process all animal)
    for (int i = 0; i < state->animal.count; i++) {
        if (!state->animal.isActive[i]) continue;
        
        if (CheckCollisionRecs(playerRect, state->animal.interactionZone[i])) {
            if (IsKeyPressed(KEY_E)) {
                // Feed animal (update hunger)
                state->animal.hunger[i] += 0.3f;
                if (state->animal.hunger[i] > 1.0f) state->animal.hunger[i] = 1.0f;
                
                // Pet animal (update happiness)
                state->animal.happiness[i] += 0.1f;
                if (state->animal.happiness[i] > 1.0f) state->animal.happiness[i] = 1.0f;
            }
        }
    }
    
    // Check plant interactions (process all plants)
    for (int i = 0; i < state->crop.count; i++) {
        if (!state->crop.isActive[i]) continue;
        
        Rectangle plantRect = {
            state->crop.position[i].x, 
            state->crop.position[i].y, 
            40, 40
        };
        
        if (CheckCollisionRecs(playerRect, plantRect)) {
            if (IsKeyPressed(KEY_E) && state->crop.state[i] == CROP_READY) {
                // Harvest plant (update state)
                state->crop.state[i] = CROP_EMPTY;
                state->crop.growth[i] = 0;
                // TODO: Add to player inventory
            }
        }
    }
    
    if (IsKeyPressed(KEY_P)) {
        add_crop(state, state->player.position);
    }
}

void draw_animal(AnimalData *animal, Texture2D texture) {
    for (int i = 0; i < animal->count; i++) {
        if (!animal->isActive[i]) continue;
        
        Rectangle animalFrame = animation_frame(&animal->anims[i], 4, 162.0f);
        float scale = 0.2f; // scale down to half size
        Rectangle destRect = {animal->position[i].x, animal->position[i].y, animalFrame.width * scale, animalFrame.height * scale};
        DrawTexturePro(texture, animalFrame, destRect, (Vector2){0, 0}, 0.0f, WHITE);

        // Draw happiness meter
        DrawRectangle(animal->position[i].x, animal->position[i].y - 20, 
                     (int)(50 * animal->happiness[i]), 5, GREEN);

        // Draw hunger meter
        DrawRectangle(animal->position[i].x, animal->position[i].y - 15, 
                     (int)(50 * animal->hunger[i]), 5, BLUE);
    }
}

// Draw all plants (process all plants in a tight loop)
void draw_crops(CropData *crops, Texture2D texture) {
    for (int i = 0; i < crops->count; i++) {
        if (!crops->isActive[i] || crops->state[i] == CROP_EMPTY) continue;

        Vector2 pos = crops->position[i];
        float stemHeight = 30;
        float stemWidth = 6;
        Color stemColor = DARKGREEN;
        Color leafColor = GREEN;
        if (crops->state[i] == CROP_SEEDLING) {
            stemColor = LIME;
            leafColor = LIME;
        }
        if (crops->state[i] == CROP_READY) {
            stemColor = DARKGREEN;
            leafColor = GREEN;
        }

        // Draw stem
        DrawRectangle((int)(pos.x + 17), (int)(pos.y + 10), (int)stemWidth, (int)stemHeight, stemColor);

        // Draw leaves (two circles)
        DrawCircle((int)(pos.x + 20), (int)(pos.y + 10), 10, leafColor);
        DrawCircle((int)(pos.x + 27), (int)(pos.y + 15), 7, leafColor);

        // Optionally, draw a fruit if ready
        if (crops->state[i] == CROP_READY) {
            DrawCircle((int)(pos.x + 20), (int)(pos.y + 5), 5, ORANGE);
        }

        // Growth bar for growing crops
        if (crops->state[i] == CROP_GROWING) {
            DrawRectangle(pos.x, pos.y - 10, (int)(40 * crops->growth[i]), 5, YELLOW);
        }
    }
}

void draw_UI(GameState *state) {
    DrawText("Vegan Animal Sanctuary", 10, 10, 20, BLACK);
    DrawText("Press E to interact with animal and plants", 10, 40, 20, BLACK);
    DrawText("Press P to plant a new seed", 10, 70, 20, BLACK);
    DrawText("WASD or Arrow Keys to move", 10, 100, 20, BLACK);
    
    // Draw animal info if near one
    Rectangle playerRect = {
        state->player.position.x, 
        state->player.position.y, 
        CFRAME_SIZE * 2, 
        CFRAME_SIZE * 2
    };
    
    for (int i = 0; i < state->animal.count; i++) {
        if (!state->animal.isActive[i]) continue;

        if (CheckCollisionRecs(playerRect, state->animal.interactionZone[i])) {
            DrawText("Press E to care for animal", 10, 130, 20, BLACK);
            break;
        }
    }
    
    DrawFPS(10, SCREEN_HEIGHT - 30);
}


int main() {
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "C raylib template");



    SetTargetFPS(60);

    GameState state;
    init_game_state(&state);

    // temporary audio just for me to feel better while running the game
    InitAudioDevice();
    Music music = LoadMusicStream("./resources/music/melancholy.mp3.mp3"); // or "audio.ogg"
    PlayMusicStream(music);


    while (!WindowShouldClose())
    {
        UpdateMusicStream(music);
        update_player(&state.player);
        update_animal(&state.animal);
        update_crops(&state.crop);
        check_interactions(&state);
        state.camera.target = state.player.position;
        

        BeginDrawing();

            ClearBackground(GREEN);

            BeginMode2D(state.camera);
                DrawRectangle(-6000, P_HEIGHT, 13000, 8000, DARKGRAY);
                for (int i = 0; i < state.buildingCount; i++) {
                    DrawRectangleRec(state.buildings[i], state.buildColor[i]);
                }

                draw_crops(&state.crop, state.cropTexture);
                draw_animal(&state.animal, state.animalTexture);
        
                Rectangle playerFrame = animation_frame(&state.player.anim, 4, CFRAME_SIZE);
                playerFrame.width *= (float)state.player.direction;
                Rectangle destRect = {state.player.position.x, state.player.position.y, CFRAME_SIZE * 2, CFRAME_SIZE * 2};
                DrawTexturePro(state.playerTexture, playerFrame, destRect, (Vector2){0, 0}, 0.0f, WHITE);
        
            EndMode2D();
            draw_UI(&state);

        EndDrawing();

        }


    UnloadMusicStream(music);
    CloseAudioDevice();
    UnloadTexture(state.playerTexture);
    UnloadTexture(state.animalTexture);
    UnloadTexture(state.cropTexture);


    CloseWindow();
    return 0;
}

