#include "imgui/imgui.h"
#include "raylib.h"
#include "rlImGui/rlImGui.h"
#include <iostream>
#include <iterator>
#include <raylib.h>

using namespace std;

int g_brushSize = 1;
float g_imGuiColorFloat[4]{0.0f, 0.0f, 0.0f, 1.0f}; // BLACK Color
Color g_brushColor = BLACK;
bool g_isColorPickerPressed;

void ClearPixels(Color pixels[], int dataSize) {
  for (int i = 0; i < dataSize; i++) {
    pixels[i] = WHITE;
  }
}

void DrawUIEssentials() {
  ImGui::SetWindowPos(ImVec2(0.0f, 0.0f));
  ImGui::SliderInt("Brush Size", &g_brushSize, 1, 10);

  // Changing brush color only on release for optimization reasons
  if (ImGui::ColorPicker4("Brush color", g_imGuiColorFloat)) {
    if (!g_isColorPickerPressed) {
      g_isColorPickerPressed = true;
    }
  } else {
    if (g_isColorPickerPressed) {
      unsigned char r = g_imGuiColorFloat[0] * 255;
      unsigned char g = g_imGuiColorFloat[1] * 255;
      unsigned char b = g_imGuiColorFloat[2] * 255;
      unsigned char a = g_imGuiColorFloat[3] * 255;
      g_brushColor = {r, g, b, a};

      g_isColorPickerPressed = false;
    }
  }
}

//------------------------------------------------------------------------------------
// Program main entry point
//------------------------------------------------------------------------------------
int main(void) {
  // Initialization
  //--------------------------------------------------------------------------------------
  const int screenWidth = 800;
  const int screenHeight = 450;

  InitWindow(screenWidth, screenHeight, "Pixel Editor");

  SetTargetFPS(60); // Set our game to run at 60 frames-per-second

  int pixelsSize = screenWidth * screenHeight;
  Color pixelColors[pixelsSize];
  ClearPixels(pixelColors, pixelsSize);

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

    DrawUIEssentials();

    if (IsKeyPressed(KEY_Q)) {
      ClearPixels(pixelColors, pixelsSize);
      UpdateTexture(screenTexture, pixelColors);
    }
    DrawTexture(screenTexture, 0, 0, RAYWHITE);

    if (IsMouseButtonDown(MOUSE_BUTTON_LEFT)) {
      Vector2 mousePos = GetMousePosition();
      for (int i = mousePos.x - g_brushSize; i <= mousePos.x + g_brushSize;
           i++) {
        for (int j = mousePos.y - g_brushSize; j <= mousePos.y + g_brushSize;
             j++) {
          if (i >= 0 && i < screenWidth && j >= 0 && j < screenHeight) {
            pixelColors[i + j * screenWidth] = g_brushColor;
          }
        }
      }
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
