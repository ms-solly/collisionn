#include "raylib.h"
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <math.h>
#include "rlgl.h"

#define SCREEN_WIDTH 1200
#define SCREEN_HEIGHT 600




void DrawSphereBasic(Color color);



int DrawCone() {
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "3D Cone");
    SetTargetFPS(60);

    const float sunRadius = 4.0f;
    const float earthRadius = 0.6f;
    const float moonRadius = 0.16f;
    const float earthOrbitRadius = 8.0f;
    const float moonOrbitRadius = 1.5f;

    Camera camera = {0};
    camera.position = (Vector3){16.0f,16.0f, 16.0f};
    camera.target = (Vector3){0.0f, 0.0f,0.0f};
    camera.up = (Vector3){0.0f, 1.0f,0.0f};
    camera.fovy = 45.0f;
    camera.projection = CAMERA_PERSPECTIVE;

    float rotationSpeed = 0.2f;
    float earthRotation = 0.0f;
    float earthOrbitRotation = 0.0f;
    float moonRotation = 0.0f;
    float moonOrbitRotation = 0.0f;


    while (!WindowShouldClose())
    {

        UpdateCamera(&camera, CAMERA_ORBITAL);

        earthRotation += (5.0f * rotationSpeed);
        earthOrbitRotation += (365/360.0f * (5.0f * rotationSpeed) * rotationSpeed);
        moonRotation += (2.0f * rotationSpeed);
        moonOrbitRotation += (8.0f * rotationSpeed);

        BeginDrawing();

        ClearBackground(BLACK);

        BeginMode3D(camera);
        
        rlPushMatrix();
          rlScalef(sunRadius, sunRadius, sunRadius);
          DrawSphereBasic(GOLD);
        rlPopMatrix();

        rlPushMatrix();
          rlRotatef(earthOrbitRotation, 0.0f, 1.0f, 0.0f);
          rlTranslatef(earthRadius, earthRadius, earthRadius);
          DrawSphereBasic(BLUE);
        rlPopMatrix();

        rlRotatef(moonOrbitRadius, 0.0f, 1.0f, 0.0f);
        rlTranslatef(moonOrbitRadius, 0.0f, 0.0f);
        rlRotatef(moonRotation, 0.0f, 1.0f, 0.0f);
        rlScalef(moonRadius,moonRadius, moonRadius);

        DrawSphereBasic(LIGHTGRAY);
        rlPopMatrix();

        DrawCircle3D((Vector3){0.0f,0.0f,0.0f},earthOrbitRadius, (Vector3){1,0,0}, 90.0f, Fade(RED,(0.0f)));
        DrawGrid(20, 1.0f);

        EndMode3D();
        rlDrawCall;
        DrawFPS(10,10);

        EndDrawing();
    }
    

    CloseWindow();
    return 0;
}

void DrawSphereBasic(Color color)
{
    int rings = 16;
    int slices = 16;

    rlCheckRenderBatchLimit((rings + 2)*slices * 6);
    rlBegin(RL_TRIANGLES);
      rlColor4ub(color.r, color.g, color.b , color.a);

      for (size_t i = 0; i < (rings + 2); i++)
      {
        for (size_t j = 0; j < slices; j++)
        {
            rlVertex3f(cos(DEG2RAD* (270 + (180/(rings + 1)) *  i ))  * sin(DEG2RAD*(j*360/slices)),
                       sin(DEG2RAD* (270 + (180/(rings = 1)) *  i)),
                       cos(DEG2RAD* (270 + (180/(rings + 1)) *  i))   * cos(DEG2RAD *(j*360/slices)));
            rlVertex3f(cos(DEG2RAD* (270 + (180/(rings + 1)) * (i+1)))* sin(DEG2RAD *((j+1 )*360/slices)),
                       sin(DEG2RAD* (270 + (180/(rings = 1)) * (i+1))),
                       cos(DEG2RAD* (270 + (180/(rings + 1)) * (i+1)))* cos(DEG2RAD *((j+1 )*360/slices)));
            rlVertex3f(cos(DEG2RAD* (270 + (180/(rings + 1)) * (i+1)))* sin(DEG2RAD *(j*360/slices)),
                       sin(DEG2RAD* (270 + (180/(rings = 1)) * (i+1))),
                       cos(DEG2RAD* (270 + (180/(rings + 1)) * (i+1)))* cos(DEG2RAD *(j *360/slices)));

            rlVertex3f(cos(DEG2RAD* (270 + (180/(rings + 1)) *  i ))  * sin(DEG2RAD *(j*360/slices)),
                       sin(DEG2RAD* (270 + (180/(rings = 1)) *  i)),
                       cos(DEG2RAD* (270 + (180/(rings + 1)) *  i))   * cos(DEG2RAD *(j*360/slices)));
            rlVertex3f(cos(DEG2RAD* (270 + (180/(rings + 1)) *  i))   * sin(DEG2RAD *((j+1 )*360/slices)),
                       sin(DEG2RAD* (270 + (180/(rings = 1)) *  i)),
                       cos(DEG2RAD* (270 + (180/(rings + 1)) *  i))   * cos(DEG2RAD *((j+1 )*360/slices)));
            rlVertex3f(cos(DEG2RAD* (270 + (180/(rings + 1))*  (i+1)))* sin(DEG2RAD*((j +1)*360/slices)),
                       sin(DEG2RAD* (270 + (180/(rings = 1)) * (i+1))),
                       cos(DEG2RAD* (270 + (180/(rings + 1)) * (i+1)))* cos(DEG2RAD *((j +1) *360/slices)));
        }
        
      }
      rlEnd();
}