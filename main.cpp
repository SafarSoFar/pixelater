#include "imgui/imgui.h"
#include "raylib.h"
#include "rlImGui/rlImGui.h"
#include <cmath>
#include <cstring>
#include <iostream>
#include <iterator>
#include <raylib.h>

using namespace std;

bool operator==(Vector2 lhs, Vector2 rhs) {
  return lhs.x == rhs.x && lhs.y == rhs.y;
  
}

enum Tool {
  Brush,
  Line,
  Rect,
  Eraser,
  Circle,
};

enum BrushShape{
  SquareBrush,
  CircleBrush
};

// temporary global logic
int g_brushSize = 1;
float g_imGuiColorFloat[4]{0.0f, 0.0f, 0.0f, 1.0f}; // BLACK Color
Color g_drawingColor = BLACK;
bool g_isColorPickerPressed;


void ClearPixels(Color pixels[], int dataSize) {
  for (int i = 0; i < dataSize; i++) {
    pixels[i] = WHITE;
  }
}

const int g_screenWidth = 800;
const int g_screenHeight = 450;
const int g_pixelsSize = g_screenWidth * g_screenHeight;
Color g_mainCanvasPixels[g_pixelsSize];
Color g_tmpCanvasPixels[g_pixelsSize];
Tool g_curTool = Tool::Brush;
BrushShape g_brushShape = BrushShape::SquareBrush;

// using texture like global increase performance.
Texture2D g_mainCanvasTexture;
Texture2D g_tmpCanvasTexture;
Vector2 g_lastMousePos = {0.0f, 0.0f};
bool g_isHoldingLMB;
Vector2 g_LMBHoldingFirstPos = {0.0f, 0.0f};
Vector2 g_LMBHoldingLastPos = {0.0f, 0.0f};

bool IsOutsideOfScreen(int x, int y){
  return x < 0 || y < 0 || x >= g_screenWidth || y >= g_screenHeight; 
}

void DrawFilledSquare(int originX, int originY, int size, Color color){
    for (int i = originX - size;
        i <= originX + size; i++) {
      for (int j = originY - size;
          j <= originY + size; j++) {
        if (!IsOutsideOfScreen(i, j)){
          g_tmpCanvasPixels[i + j * g_screenWidth] = color;
        }
      }
    }
}

void DrawWithBrush(Color colorToDraw) {
  /*if(g_brushShape == BrushShape::SquareBrush){
    for (int i = g_lastMousePos.x - g_brushSize;
        i <= g_lastMousePos.x + g_brushSize; i++) {
      for (int j = g_lastMousePos.y - g_brushSize;
          j <= g_lastMousePos.y + g_brushSize; j++) {
        if (!IsOutsideOfScreen(i, j)){
          g_tmpCanvasPixels[i + j * g_screenWidth] = g_drawingColor;
        }
      }
    }
  }*/

  if(g_brushShape == BrushShape::SquareBrush){
    DrawFilledSquare(g_lastMousePos.x, g_lastMousePos.y, g_brushSize, colorToDraw);
  }

  UpdateTexture(g_tmpCanvasTexture, &g_tmpCanvasPixels);
}

void DrawWithLine() {

  float x0 = g_LMBHoldingFirstPos.x;
  float x1 = g_LMBHoldingLastPos.x;
  float y0 = g_LMBHoldingFirstPos.y;
  float y1 = g_LMBHoldingLastPos.y;

  // checking screen boundaries to prevent segfault
  if(IsOutsideOfScreen(x0, y0) || IsOutsideOfScreen(x1, y1)){
    return;
  }

  // resetting the buffer to draw only one line at the time
  memcpy(g_tmpCanvasPixels, g_mainCanvasPixels, g_pixelsSize * sizeof(Color));

  float x = x1 - x0;
  float y = y1 - y0;

  const float max = std::max(std::fabs(x), std::fabs(y));

  x /= max;
  y /= max;

  for (int i = 0; i < max; i++) {

    for(int width = x0 - g_brushSize; width <= x0 + g_brushSize; width++){
      for(int height = y0 - g_brushSize; height <= y0 + g_brushSize; height++){
        if(!IsOutsideOfScreen(width, height)){
          g_tmpCanvasPixels[(int)width + (int)height * g_screenWidth] = g_drawingColor;
        }
      }
    }

    x0 += x;
    y0 += y;

  }
  UpdateTexture(g_tmpCanvasTexture, &g_tmpCanvasPixels);
}

void DrawWithRectangle() {}

void DrawWithCircle(){
  for(double angle = 0; angle<2*PI; angle+=0.001){
  int x0 = g_lastMousePos.x + g_brushSize*cos(angle);
  int y0 = g_lastMousePos.y + g_brushSize*sin(angle);
    g_tmpCanvasPixels[x0 + y0 * g_screenWidth] = g_drawingColor;  
  }
  UpdateTexture(g_tmpCanvasTexture, &g_tmpCanvasPixels);
}

void DrawWithSquareBrush(){

}

void DrawWithCircleBrush(){

}

void Draw() {
  switch (g_curTool) {  
  case Line:
    DrawWithLine();
    break;
  case Brush:
    DrawWithBrush(g_drawingColor);
  case Rect:
    DrawWithRectangle();
    break;
  case Eraser:
    DrawWithBrush(WHITE);
    break;
  case Circle:
    DrawWithCircle();
    break;
  }
}

void DrawAndControlGUI() {
  ImGui::SetWindowPos(ImVec2(0.0f, 0.0f));
  ImGui::SliderInt("Brush Size", &g_brushSize, 1, 10);

  // Color choosing for drawing
  // Changing color only on release for optimization reasons
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
      g_drawingColor = {r, g, b, a};

      g_isColorPickerPressed = false;
    }
  }

  if (ImGui::Button("Clear Canvas")) {
    ClearPixels(g_mainCanvasPixels, g_pixelsSize);
    ClearPixels(g_tmpCanvasPixels, g_pixelsSize);
    UpdateTexture(g_mainCanvasTexture, &g_mainCanvasPixels);
    UpdateTexture(g_tmpCanvasTexture, &g_mainCanvasPixels);
  }
  if (ImGui::Button("Line")) {
    g_curTool = Tool::Line;
  }
  if (ImGui::Button("Brush")) {
    g_curTool = Tool::Brush;
  }
  if (ImGui::Button("Eraser")) {
    g_curTool = Tool::Eraser;
  }
  if (ImGui::Button("Circle")) {
    g_curTool = Tool::Circle;
  }
}


// buggy
/*void DrawSizeCursor(){*/
/*  DrawCircleLines(g_lastMousePos.x, g_lastMousePos.y, g_brushSize, BLACK);*/
/*}*/

//------------------------------------------------------------------------------------
// Program main entry point
//------------------------------------------------------------------------------------
int main(void) {
  // Initialization
  //--------------------------------------------------------------------------------------

  InitWindow(g_screenWidth, g_screenHeight, "Pixel Editor");

  ClearPixels(g_mainCanvasPixels, g_pixelsSize);

  Image g_image = {g_mainCanvasPixels, g_screenWidth, g_screenHeight, 1,
                   PIXELFORMAT_UNCOMPRESSED_R8G8B8A8};

  g_mainCanvasTexture = LoadTextureFromImage(g_image);
  g_tmpCanvasTexture = LoadTextureFromImage(g_image);
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
    ClearBackground(WHITE);
    BeginDrawing();
    rlImGuiBegin();

    DrawAndControlGUI();


    g_lastMousePos = GetMousePosition();

    /*DrawSizeCursor();*/


    if (IsMouseButtonDown(MOUSE_BUTTON_LEFT)) {
      if (!g_isHoldingLMB) {
        g_isHoldingLMB = true;
        g_LMBHoldingFirstPos = g_lastMousePos;
      }
      g_LMBHoldingLastPos = g_lastMousePos;

      DrawTexture(g_tmpCanvasTexture, 0,0,WHITE);
      Draw();

    } else {

      if (g_isHoldingLMB) {
        g_isHoldingLMB = false;
      }

      DrawTexture(g_mainCanvasTexture, 0, 0, WHITE);

    }
    
    if(IsMouseButtonReleased(MOUSE_BUTTON_LEFT)){
      /*g_mainCanvasPixels = g_tmpCanvasPixels;*/
      memcpy(g_mainCanvasPixels, g_tmpCanvasPixels, g_pixelsSize * sizeof(Color));
      //g_mainCanvasPixels = g_tmpCanvasPixels;
      UpdateTexture(g_mainCanvasTexture, &g_mainCanvasPixels);
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
