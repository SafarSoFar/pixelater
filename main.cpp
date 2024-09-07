#include "imgui/imgui.h"
#include "raylib.h"
#include "rlImGui/rlImGui.h"
#include <iostream>
#include <raylib.h>

using namespace std;

//------------------------------------------------------------------------------------
// Program main entry point
//------------------------------------------------------------------------------------
int main(void) {
  // Initialization
  //--------------------------------------------------------------------------------------
  const int screenWidth = 800;
  const int screenHeight = 450;

  int brushSize = 1;

  InitWindow(screenWidth, screenHeight, "Pixel Editor");

  SetTargetFPS(60); // Set our game to run at 60 frames-per-second

  rlImGuiSetup(true);

  //--------------------------------------------------------------------------------------

  // Main game loop
  while (!WindowShouldClose()) // Detect window close button or ESC key
  {
    // Update
    //----------------------------------------------------------------------------------
    // TODO: Update your variables here
    //----------------------------------------------------------------------------------

    // Draw
    //----------------------------------------------------------------------------------
    ClearBackground(RAYWHITE);
    BeginDrawing();
    rlImGuiBegin();

    ImGui::SliderInt("Brush Size", &brushSize, 1, 10);

    if (IsKeyPressed(KEY_Q)) {
      ClearBackground(RAYWHITE);
    }

    if (IsMouseButtonDown(MOUSE_BUTTON_LEFT)) {
      Vector2 mousePos = GetMousePosition();
      for (int i = mousePos.x - brushSize; i <= mousePos.x + brushSize; i++) {
        for (int j = mousePos.y - brushSize; j <= mousePos.y + brushSize; j++) {
          if (i >= 0 && i < screenWidth && j >= 0 && j < screenHeight) {
            DrawPixel(i, j, BLACK);
          }
        }
      }
    }

    rlImGuiEnd();
    EndDrawing();
    //----------------------------------------------------------------------------------
  }

  // De-Initialization
  //--------------------------------------------------------------------------------------
  rlImGuiShutdown();
  CloseWindow(); // Close window and OpenGL context
  //--------------------------------------------------------------------------------------

  return 0;
}
