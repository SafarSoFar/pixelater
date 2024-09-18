#include "imgui/imgui.h"
#include "pixel-draw.h"
#include "raylib.h"
#include "rlImGui/rlImGui.h"
#include <algorithm>
#include <stack>
#include <memory>
#include <array>
#include <cmath>
#include <vector>
#include <cstring>
#include <iostream>
#include <iterator>
#include <raylib.h>

using namespace std;

bool operator==(Vector2 lhs, Vector2 rhs) {
  return lhs.x == rhs.x && lhs.y == rhs.y;
}


// temporary global logic
float g_imGuiColorFloat[4]{0.0f, 0.0f, 0.0f, 1.0f}; // BLACK Color
bool g_isColorPickerPressed;



void ClearPixels(Color pixels[], int dataSize) {
  for (int i = 0; i < dataSize; i++) {
    pixels[i] = WHITE;
  }
}

const int g_screenWidth = 1280;
const int g_screenHeight = 800;
const int g_pixelsSize = g_screenWidth * g_screenHeight;
Color g_mainCanvasPixels[g_pixelsSize];
Color g_tmpCanvasPixels[g_pixelsSize];

// using texture like global increase performance.
Texture2D g_mainCanvasTexture;
Texture2D g_tmpCanvasTexture;
bool g_isHoldingLMB;
bool g_canInteractWithCanvas = true;
Vector2 g_LMBHoldingFirstPos = {0.0f, 0.0f};
Vector2 g_lastMousePos = {0.0f, 0.0f};

// The only way is to use std::array instead of C-arrays and convert them along the way
vector<array<Color, g_pixelsSize>> previousCanvasColorPixels;


void AddCanvasToUndo(){
    array<Color,g_pixelsSize> prevArr;
    
    // Copying C-array content to std::array 
    copy(g_mainCanvasPixels, g_mainCanvasPixels+g_pixelsSize, prevArr.begin());

    // delete the limited step undo
    if(previousCanvasColorPixels.size() >= 30){
      previousCanvasColorPixels.erase(previousCanvasColorPixels.begin());
    }

    previousCanvasColorPixels.push_back(prevArr);

}

PixelDraw g_pixelDraw(g_screenWidth, g_screenHeight, g_tmpCanvasPixels, g_mainCanvasPixels);

void Draw() {
  switch (g_pixelDraw.curTool) {  
  case Line:
    g_pixelDraw.DrawWithLine(g_LMBHoldingFirstPos.x, g_LMBHoldingFirstPos.y, g_lastMousePos.x, g_lastMousePos.y);
    break;
  case Brush:
    g_pixelDraw.DrawWithBrush(g_lastMousePos.x, g_lastMousePos.y, g_pixelDraw.curDrawingColor);
  case Rect:
    g_pixelDraw.DrawWithRectangle();
    break;
  case Eraser:
    g_pixelDraw.DrawWithBrush(g_lastMousePos.x, g_lastMousePos.y, WHITE);
    break;
  case Circle:
    // The first mouse position is the center, if user is holding left alt - don't delete intermidiate steps
    if(IsKeyDown(KEY_LEFT_CONTROL)){
      g_pixelDraw.DrawCenteredCircle(g_LMBHoldingFirstPos.x, g_LMBHoldingFirstPos.y,
          g_lastMousePos.x, g_lastMousePos.y, g_pixelDraw.curDrawingColor, IsKeyDown(KEY_LEFT_ALT));
    }
    // Center is calculated as the midpoint of two mouse vectors, if user is holding left alt - don't delete intermidiate steps
    else{
      g_pixelDraw.DrawAndStretchCircle(g_LMBHoldingFirstPos.x, g_LMBHoldingFirstPos.y,
          g_lastMousePos.x, g_lastMousePos.y, g_pixelDraw.curDrawingColor, IsKeyDown(KEY_LEFT_ALT));
    }
    break;
  case Fill:
    g_pixelDraw.FillWithColor(g_lastMousePos.x, g_lastMousePos.y, g_pixelDraw.curDrawingColor);
    break;
  }
  UpdateTexture(g_tmpCanvasTexture, &g_tmpCanvasPixels);
}

void UndoControl(){
  if(IsKeyDown(KEY_LEFT_CONTROL) && IsKeyPressed(KEY_Z) && previousCanvasColorPixels.size()){


    auto prev = previousCanvasColorPixels.back();
    previousCanvasColorPixels.pop_back();
    /*previousCanvasColorPixels.pop();*/

    memcpy(g_mainCanvasPixels, &prev, g_pixelsSize * sizeof(Color));

    memcpy(g_tmpCanvasPixels, g_mainCanvasPixels, g_pixelsSize * sizeof(Color));

    UpdateTexture(g_mainCanvasTexture, &g_mainCanvasPixels);
    /*UpdateTexture(g_tmpCanvasTexture, &g_tmpCanvasPixels);*/
  }
}

void DrawAndControlGUI() {
  ImGui::SetWindowPos(ImVec2(0.0f, 0.0f));

  ImGui::SliderInt("Brush Size", &g_pixelDraw.curToolSize, 1, 20);


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
      g_pixelDraw.curDrawingColor = {r, g, b, a};

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
    g_pixelDraw.curTool = Tool::Line;
  }
  if (ImGui::Button("Brush")) {
    g_pixelDraw.curTool = Tool::Brush;
  }
  if(ImGui::CollapsingHeader("Brush Shapes")){
    if (ImGui::Button("Brush: Circle")) {
      g_pixelDraw.curBrushShape = BrushShape::CircleBrush;
    }
    if (ImGui::Button("Brush: Square")) {
      g_pixelDraw.curBrushShape = BrushShape::SquareBrush;
    }
  }
  if (ImGui::Button("Eraser")) {
    g_pixelDraw.curTool = Tool::Eraser;
  }
  if (ImGui::Button("Circle")) {
    g_pixelDraw.curTool = Tool::Circle;
  }
  if (ImGui::Button("Fill")) {
    g_pixelDraw.curTool = Tool::Fill;
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


  ImGui:ImGui::StyleColorsDark();


  ImGuiIO& io = ImGui::GetIO();



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

    UndoControl();

    /*DrawSizeCursor();*/


    if (IsMouseButtonDown(MOUSE_BUTTON_LEFT) && !io.WantCaptureMouse) {

      if (!g_isHoldingLMB) {
        g_isHoldingLMB = true;
        g_LMBHoldingFirstPos = g_lastMousePos;
      }

      DrawTexture(g_tmpCanvasTexture, 0,0,WHITE);
      Draw();

    } else {

      if (g_isHoldingLMB) {
        g_isHoldingLMB = false;
      }

      DrawTexture(g_mainCanvasTexture, 0, 0, WHITE);

    }
    if(IsMouseButtonReleased(MOUSE_BUTTON_LEFT)){

      AddCanvasToUndo();

      memcpy(g_mainCanvasPixels, g_tmpCanvasPixels, g_pixelsSize * sizeof(Color));

      
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
