#include "raylib.h"
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <math.h>
#include "rlgl.h"

#define SCREEN_WIDTH 1200
#define SCREEN_HEIGHT 600

typedef struct
{
    float x, y;
} Vectorr2;

typedef struct 
{
    Vectorr2 min, max;
} AABB;


bool is_particle_colliding(int , int , int , int );
bool is_line_colliding(int , int , int , int );
bool is_rect_rect_colliding(Rectangle, Rectangle);
bool is_circ_circ_colliding(Vector2, Vector2, int, int);
bool test_AABB_overlap(AABB *a, AABB *b);

bool is_particle_colliding(int ax, int ay, int bx, int by)
{
    if (ax == bx && ay == by)
    {
        return true;
    }
    else
    {
        return false;
    }
    //  or
    //  return (ax == bx) && (ay == by)
}

bool is_rect_rect_colliding(Rectangle a, Rectangle b)
{
    if (a.x < b.x + b.width && a.x + a.width > b.x && a.y < b.y + b.height && a.y + a.height > b.y )
    {
        return true;
    }
    else
    {
        return false;
    }
    // or 
    // return (a.x < b.x + b.width && a.x + a.width > b.x && a.y < b.y + b.height && a.y + a.height > b.y) ? true : false;
}

bool is_circ_circ_colliding(Vector2 a, Vector2 b, int r1, int r2)
{
    int dx = a.x - b.x;
    int dy = a.y - b.y;
    int dis = sqrtf(dx * dx + dy * dy);
    
    return dis < r1 + r2 ? true : false;
}

bool test_AABB_overlap(AABB *a, AABB *b)
{
    float d1x = b->min.x - a->max.x ;
    float d1y = b->min.y - a->max.y ;
    float d2x = a->min.x - a->max.x ;
    float d2y = a->max.y - a->max.y ;

    d1x > 0.0f || d1y > 0.0f ?  false : true;
    d2x > 0.0f || d2y > 0.0f ? false : true;

    return true;
}



int isColliding() {
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Collision Detection");
    SetTargetFPS(60);



    while (!WindowShouldClose())
    {

        BeginDrawing();

        ClearBackground(BLACK);

        // DrawText("C Template", GetScreenWidth()/2 - MeasureText("C Template",100), GetScreenHeight()/2, 100, WHITE);
        // DrawPixel(40,100,WHITE);
        // DrawPixelV(GetMousePosition(),WHITE);
        // DrawText(TextFormat("isParticleColliding : %d",isParticleColliding(40, 100, GetMouseX(), GetMouseY())),10, 10, 30, LIGHTGRAY);

        // DrawLine();

        // DrawRectangle(200, 300, 400, 200, YELLOW);
        // DrawRectangle(GetMouseX(), GetMouseY(), 300, 200, RED);
        // DrawText(TextFormat("is_rect_rect_colliding : %d",is_rect_rect_colliding((Rectangle){200, 300, 400, 200}, (Rectangle){GetMouseX(), GetMouseY(), 300, 200})),10, 10, 30, LIGHTGRAY);

        DrawCircleV((Vector2){100, 200}, 20, PINK);
        DrawCircleV(GetMousePosition(), 30, GREEN);
        DrawText(TextFormat("is_rect_rect_colliding : %d",is_circ_circ_colliding((Vector2){100, 200},GetMousePosition(), 20 , 30)),10, 10, 30, LIGHTGRAY);


        EndDrawing();
    }
    

    CloseWindow();
    return 0;
}
