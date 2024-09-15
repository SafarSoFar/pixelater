#include "imgui/imgui.h"
#include "pixel-draw.h"
#include "raylib.h"
#include "rlImGui/rlImGui.h"
#include <cmath>
#include <vector>
#include <queue>
#include <cstring>
#include <iostream>
#include <iterator>
#include <raylib.h>

using namespace std;

bool operator==(Vector2 lhs, Vector2 rhs) {
  return lhs.x == rhs.x && lhs.y == rhs.y;
}


bool operator==(Color lhs, Color rhs){
  return lhs.r == rhs.r && lhs.g == rhs.g && lhs.b == rhs.b && lhs.a == rhs.a;
}



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
bool g_isFillingCanvas = false;

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

/*void RecursiveFill(int x, int y, Color colorToFill, Color fillColor){*/
/*  g_tmpCanvasPixels*/
/*}*/

void FillWithColor(Color fillColor){
  if(g_isFillingCanvas)
    return;

  g_isFillingCanvas = true;
  int originX = g_lastMousePos.x;
  int originY = g_lastMousePos.y;
  Color colorToFill = g_mainCanvasPixels[originX + originY * g_screenWidth];

  // Worst case scenario fill memory usage - fill full screen
  vector<vector<bool>> isVis(g_screenWidth, vector<bool>(g_screenHeight, false));
  queue<pair<int,int>> q;

  q.push({originX, originY});
  isVis[originX][originY] = true;

  while(q.size()){

    pair<int,int> coords = q.front();
    g_tmpCanvasPixels[coords.first + coords.second * g_screenWidth] = fillColor;

    q.pop();

    if(coords.first+1 < g_screenWidth && 
    g_tmpCanvasPixels[coords.first+1 + coords.second * g_screenWidth] == colorToFill && !isVis[coords.first+1][coords.second]){

      q.push({coords.first+1, coords.second});
      isVis[coords.first+1][coords.second] = true;
    }
    if(coords.first-1 >= 0 && g_tmpCanvasPixels[coords.first-1 + coords.second * g_screenWidth] == colorToFill && !isVis[coords.first-1][coords.second]){
      q.push({coords.first-1, coords.second});
      isVis[coords.first-1][coords.second] = true;
    }

    if(coords.second+1 < g_screenHeight && g_tmpCanvasPixels[coords.first + coords.second+1 * g_screenWidth] == colorToFill && !isVis[coords.first][coords.second+1]){
      q.push({coords.first, coords.second+1});
      isVis[coords.first][coords.second+1] = true;
    }
    if(coords.second-1 >= 0 && g_tmpCanvasPixels[coords.first + coords.second-1 * g_screenWidth] == colorToFill && !isVis[coords.first][coords.second-1]){
      q.push({coords.first, coords.second-1});
      isVis[coords.first][coords.second-1] = true;
    }
  }
  g_isFillingCanvas = false;
  /*thread::slle*/
}


void DrawCircle(int originX, int originY, int size, Color color){
  for(double angle = 0; angle<2*PI; angle+=0.01){
    for(int radius = 0; radius < size; radius++){
      int x0 = originX + radius*cos(angle);
      int y0 = originY + radius*sin(angle);
      g_tmpCanvasPixels[x0 + y0 * g_screenWidth] = color;  
    }
  }
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

  if(g_brushShape == BrushShape::SquareBrush){
    DrawFilledSquare(g_lastMousePos.x, g_lastMousePos.y, g_brushSize, colorToDraw);
  }
  else{
    DrawCircle(g_lastMousePos.x, g_lastMousePos.y, g_brushSize, colorToDraw);
  }

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
}

void DrawWithRectangle() {}




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
    //DrawCircle();
    break;
  case Fill:
    FillWithColor(g_drawingColor);
    break;
  }
  UpdateTexture(g_tmpCanvasTexture, &g_tmpCanvasPixels);
}

void DrawAndControlGUI() {
  ImGui::SetWindowPos(ImVec2(0.0f, 0.0f));
  ImGui::SliderInt("Brush Size", &g_brushSize, 1, 20);

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
  if (ImGui::Button("Fill")) {
    g_curTool = Tool::Fill;
  }
  if (ImGui::Button("Brush: Circle")) {
    g_brushShape = BrushShape::CircleBrush;
  }
  if (ImGui::Button("Brush: Square")) {
    g_brushShape = BrushShape::SquareBrush;
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
