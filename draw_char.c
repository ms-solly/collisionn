#include "draw_animation.h"

// memorize this! Converting from index -> coord and vice versa is extremely
// common in game development For example, Minecraft stores a chunk in a 1D
// array. The coordinates are retrieved using this method (with a third
// dimension of course).
static Rectangle animation_frame(Animation *self, int num_frames_per_row, float frame_size) {
    int x = (self->cur % num_frames_per_row) * frame_size;
    int y = (self->cur / num_frames_per_row) * frame_size;
    return (Rectangle){ .x = (float)x, .y = (float)y, .width = frame_size, .height = frame_size };
}

// CHALLENGE: find a way to turn these two if statements into one
// (hint. use the value of step in the if statement?)
// handle reaching the end of the animation going forwards
// handle reaching the end (which is the beginning) of an animation going
// backwards
void char_animation_update(Animation *self) {
    float dt = GetFrameTime();
    self->duration_left -= dt;
    if (self->duration_left <= 0.0f) {
        // go to the next frame and reset the state of the animation
        self->duration_left = self->speed;
        self->cur += self->step;
        if (self->step > 0 && self->cur > self->last) {
            switch (self->type) {
                case REPEATING: self->cur = self->first; break;
                case ONESHOT:   self->cur = self->last; break;
            }
        } else if (self->step < 0 && self->cur < self->first) {
            switch (self->type) {
                case REPEATING: self->cur = self->last; break;
                case ONESHOT:   self->cur = self->first; break;
            }
        }
    }
}



void draw_animation(const char *fileName, Animation *anim, float x, float y, int num_frames_per_row, float frame_size, Direction direction) {
    Rectangle src = animation_frame(anim, num_frames_per_row, frame_size);
    src.width *= direction; // Flip horizontally if needed
    Rectangle dst = { x, y, frame_size, frame_size };
    DrawTexturePro(LoadTexture(fileName), src, dst, (Vector2){0,0}, 0.0f, WHITE);
}

