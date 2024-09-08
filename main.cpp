#include "imgui/imgui.h"
#include "raylib.h"
#include "rlImGui/rlImGui.h"
#include <iostream>
#include <raylib.h>

using namespace std;

void UpdateTexture() {}

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

  Color pixelColors[screenWidth * screenHeight];

  for (int i = 0; i < screenWidth; i++) {
    for (int j = 0; j < screenHeight; j++) {
      pixelColors[i * screenHeight + j] = WHITE;
    }
  }

  Image image = {pixelColors, screenWidth, screenHeight, 1,
                 PIXELFORMAT_UNCOMPRESSED_R8G8B8A8};
  Texture2D screenTexture = LoadTextureFromImage(image);

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
    ImGui::SetWindowPos(ImVec2(0.0f, 0.0f));
    ImGui::SliderInt("Brush Size", &brushSize, 1, 10);

    if (IsKeyPressed(KEY_Q)) {
      ClearBackground(RAYWHITE);
    }
    DrawTexture(screenTexture, 0, 0, RAYWHITE);

    if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
      /*Vector2 mousePos = GetMousePosition();*/
      /*for (int i = mousePos.x - brushSize; i <= mousePos.x + brushSize; i++)
       * {*/
      /*  for (int j = mousePos.y - brushSize; j <= mousePos.y + brushSize; j++)
       * {*/
      /*    if (i >= 0 && i < screenWidth && j >= 0 && j < screenHeight) {*/
      /*      pixelColors[i * screenHeight + j] = BLACK;*/
      /*      UpdateTexture(screenTexture, pixelColors);*/
      /*    }*/
      /*  }*/
      /*}*/
      Vector2 mousePos = GetMousePosition();
      pixelColors[(int)mousePos.x + (int)mousePos.y * screenWidth] = BLACK;

      UpdateTexture(screenTexture, &pixelColors);
      /*pixelColors[(int)mousePos.x * screenHeight + (int)mousePos.y] = BLACK;*/
      /*UpdateTexture(screenTexture, pixelColors);*/
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
