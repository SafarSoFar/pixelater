#include "imgui/imgui.h"
#include "raylib.h"
#include "rlImGui/rlImGui.h"
#include <iostream>
#include <iterator>
#include <raylib.h>

using namespace std;

enum Tool {
  Brush,
  Line,
  Rect,
};

// temporary global logic
int g_brushSize = 1;
float g_imGuiColorFloat[4]{0.0f, 0.0f, 0.0f, 1.0f}; // BLACK Color
Color g_brushColor = BLACK;
bool g_isColorPickerPressed;

void ClearPixels(Color pixels[], int dataSize) {
  for (int i = 0; i < dataSize; i++) {
    pixels[i] = WHITE;
  }
}

void DrawAndControlGUI() {
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

const int g_screenWidth = 800;
const int g_screenHeight = 450;
const int g_pixelsSize = g_screenWidth * g_screenHeight;
Color g_pixelColors[g_pixelsSize];
Tool g_curTool = Tool::Brush;
// using texture like global increase performance.
Texture2D g_screenTexture;
bool g_isHoldingLMB;
Vector2 g_LMBHoldingFirstPos = {0.0f,0.0f};
Vector2 g_LMBHoldingLastPos = {0.0f,0.0f};


void DrawWithBrush() {
  for (int i = g_LMBHoldingLastPos.x - g_brushSize; i <= g_LMBHoldingLastPos.x + g_brushSize; i++) {
    for (int j = g_LMBHoldingLastPos.y - g_brushSize; j <= g_LMBHoldingLastPos.y + g_brushSize; j++) {
      if (i >= 0 && i < g_screenWidth && j >= 0 && j < g_screenHeight) {
        g_pixelColors[i + j * g_screenWidth] = g_brushColor;
      }
    }
  }
  UpdateTexture(g_screenTexture, &g_pixelColors);
}


void DrawWithLine(){
  if(!g_isHoldingLMB){
    g_LMBHoldingFirstPos = GetMousePosition();
  }
  g_LMBHoldingLastPos = GetMousePosition();
}

void DrawWithRectangle(){
  
}

void Draw(){
  switch(g_curTool){
    case Brush: DrawWithBrush();
        break;
    case Line: DrawWithLine();
        break;
    case Rect: DrawWithRectangle();
        break;
  }
}
//------------------------------------------------------------------------------------
// Program main entry point
//------------------------------------------------------------------------------------
int main(void) {
  // Initialization
  //--------------------------------------------------------------------------------------

  InitWindow(g_screenWidth, g_screenHeight, "Pixel Editor");

  ClearPixels(g_pixelColors, g_pixelsSize);

  Image g_image = {g_pixelColors, g_screenWidth, g_screenHeight, 1,
                   PIXELFORMAT_UNCOMPRESSED_R8G8B8A8};

  g_screenTexture = LoadTextureFromImage(g_image);
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

    DrawAndControlGUI();

    DrawTexture(g_screenTexture, 0, 0, RAYWHITE);

    if (IsMouseButtonDown(MOUSE_BUTTON_LEFT)) {
      if(!g_isHoldingLMB){
        g_isHoldingLMB = true;
        g_LMBHoldingFirstPos = GetMousePosition();
      }
      g_LMBHoldingLastPos = GetMousePosition();
      DrawWithBrush();
    }
    else{
      if(g_isHoldingLMB){
        g_isHoldingLMB = false;
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
