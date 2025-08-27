#pragma once
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

// Update animation (advance frame)
void char_animation_update(Animation *self);

// Draw a sprite animation at given position, direction and frame details
void draw_animation(Texture2D texture, Animation *anim, float x, float y, int num_frames_per_row, float frame_size, Direction direction);
static Rectangle animation_frame(Animation *self, int num_frames_per_row, float frame_size);
void void char_animation_update(Animation *self);
void updatePlayer(Player *pl);

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