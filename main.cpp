#include "imgui/imgui.h"
#include "rlImGui/imgui_impl_raylib.h"
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

#define STB_IMAGE_IMPLEMENTATION
#include "stb/stb_image.h"
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb/stb_image_write.h"

using namespace std;

bool operator==(Vector2 lhs, Vector2 rhs) {
  return lhs.x == rhs.x && lhs.y == rhs.y;
}


// temporary global logic
float g_imGuiColorFloat[4]{0.0f, 0.0f, 0.0f, 1.0f}; // BLACK Color
bool g_isColorPickerPressed;




int g_screenWidth = 900;
int g_screenHeight = 800;
int g_canvasWidth = 600;
int g_canvasHeight = 600;
int g_canvasPixelsSize = g_canvasWidth * g_canvasHeight;
Color *g_mainCanvasPixels = new Color[g_canvasPixelsSize];
Color *g_tmpCanvasPixels = new Color[g_canvasPixelsSize];

// using texture like global increase performance.
Texture2D g_mainCanvasTexture;
Texture2D g_tmpCanvasTexture;
Texture2D g_transparentTexture;
bool g_isHoldingLMB;
bool g_canInteractWithCanvas = true;
Vector2 g_LMBHoldingFirstPos = {0.0f, 0.0f};
Vector2 g_lastMousePos = {0.0f, 0.0f};

// The only way is to use std::array instead of C-arrays and convert them along the way
vector<vector<Color>> previousCanvasColorPixels;


void SavePixelArt(){
  stbi_write_png("test.png", g_canvasWidth,  g_canvasHeight, 4,  g_mainCanvasPixels, g_canvasWidth*4);
}


void AddCanvasToUndo(){
    vector<Color> prevArr(g_mainCanvasPixels, g_mainCanvasPixels+g_canvasPixelsSize);

    // Copying C-array content to std::array 
    /*copy(g_mainCanvasPixels, g_mainCanvasPixels+g_pixelsSize, prevArr.begin());*/
    /*memcpy(prevArr, &g_mainCanvasPixels, g_pixelsSize * sizeof(Color));*/

    // delete the limited step undo
    if(previousCanvasColorPixels.size() >= 30){
      previousCanvasColorPixels.erase(previousCanvasColorPixels.begin());
    }

    previousCanvasColorPixels.push_back(prevArr);

}


PixelDraw g_pixelDraw(g_canvasWidth, g_canvasHeight, g_tmpCanvasPixels, g_mainCanvasPixels);

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
  UpdateTexture(g_tmpCanvasTexture, g_tmpCanvasPixels);
}

void UndoControl(){
  if(IsKeyDown(KEY_LEFT_CONTROL) && IsKeyPressed(KEY_Z) && previousCanvasColorPixels.size()){

    auto prev = previousCanvasColorPixels.back();
    previousCanvasColorPixels.pop_back();

    memcpy(g_mainCanvasPixels, prev.data(), g_canvasPixelsSize * sizeof(Color));
    /*memcpy(g_mainCanvasPixels, prev, g_pixelsSize * sizeof(Color));*/

    memcpy(g_tmpCanvasPixels, g_mainCanvasPixels, g_canvasPixelsSize * sizeof(Color));

    UpdateTexture(g_mainCanvasTexture, g_mainCanvasPixels);
    UpdateTexture(g_tmpCanvasTexture, g_tmpCanvasPixels);
  }
}

void SetTransparentTexture(){
  Color* transparentTexturePixels = new Color[g_canvasPixelsSize]; 
  int blockSize = 16;
  int count = 0;
  bool isGray = true;
  for(int i = 0; i < g_canvasWidth; i++){
    for(int j = 0; j < g_canvasHeight; j++){
      if(count >= blockSize){
        isGray = !isGray;
        count = 0;
      }
      transparentTexturePixels[i+j*g_canvasWidth] = isGray ? GRAY : WHITE;
      count++;
    }
  }
  UpdateTexture(g_transparentTexture,transparentTexturePixels);

}


void GetMousePosRelativeToCanvas(int canvasPosX, int canvasPosY){
  Vector2 toWindowPos = GetMousePosition();
  g_lastMousePos.x = toWindowPos.x-canvasPosX;
  g_lastMousePos.y = toWindowPos.y-canvasPosY;
}

void DrawAndControlGUI() {
  ImGui::Begin("Tool panel");

  ImGui::SetWindowPos(ImVec2(0.0f, 0.0f));

  ImGui::SliderInt("Tool Size", &g_pixelDraw.curToolSize, 1, 20);


  if(ImGui::Button("Save to PNG")){
    SavePixelArt();
  }


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
    g_pixelDraw.ClearPixels();
    UpdateTexture(g_mainCanvasTexture, g_mainCanvasPixels);
    UpdateTexture(g_tmpCanvasTexture, g_mainCanvasPixels);
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
  ImGui::End();
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

  /*SetConfigFlags(FLAG_WINDOW_RESIZABLE | FLAG_WINDOW_HIGHDPI);*/
  InitWindow(g_screenWidth, g_screenHeight, "Pixel Editor");

  int canvasPosX = g_screenWidth/2-g_canvasWidth/2;
  int canvasPosY = g_screenHeight/2-g_canvasHeight/2;

  ImGui::CreateContext();
  ImGuiIO& io = ImGui::GetIO(); (void)io;
  io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;

  ImGui::StyleColorsDark();

  ImGui_ImplRaylib_Init();
  io.Fonts->AddFontDefault();
  Imgui_ImplRaylib_BuildFontAtlas();

  g_pixelDraw.ClearPixels();

  Image g_image = {g_mainCanvasPixels, g_canvasWidth, g_canvasHeight, 1,
                   PIXELFORMAT_UNCOMPRESSED_R8G8B8A8};

  g_mainCanvasTexture = LoadTextureFromImage(g_image);
  g_tmpCanvasTexture = LoadTextureFromImage(g_image);

  g_transparentTexture = LoadTextureFromImage(g_image);
  SetTransparentTexture();

  SetTargetFPS(60); // Set our game to run at 60 frames-per-second






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

    DrawTexture(g_transparentTexture, canvasPosX,canvasPosY,WHITE);

    ImGui_ImplRaylib_ProcessEvents();
    ImGui_ImplRaylib_NewFrame();
    ImGui::NewFrame();
    DrawAndControlGUI();

    ImGui::Render();
    BeginDrawing();


    
    GetMousePosRelativeToCanvas(canvasPosX, canvasPosY);

    UndoControl();

    /*DrawSizeCursor();*/

    

    if (IsMouseButtonDown(MOUSE_BUTTON_LEFT) && !io.WantCaptureMouse) {

      if (!g_isHoldingLMB) {
        g_isHoldingLMB = true;
        g_LMBHoldingFirstPos = g_lastMousePos;
      }

      DrawTexture(g_tmpCanvasTexture, canvasPosX,canvasPosY,WHITE);
      Draw();

    } else {

      if (g_isHoldingLMB) {
        g_isHoldingLMB = false;
      }

      DrawTexture(g_mainCanvasTexture, canvasPosX, canvasPosY, WHITE);

    }
    if(IsMouseButtonReleased(MOUSE_BUTTON_LEFT)){

      AddCanvasToUndo();

      memcpy(g_mainCanvasPixels, g_tmpCanvasPixels, g_canvasPixelsSize * sizeof(Color));

      
      UpdateTexture(g_mainCanvasTexture, g_mainCanvasPixels);


      
    }

    ImGui_ImplRaylib_RenderDrawData(ImGui::GetDrawData());
    EndDrawing();
    //----------------------------------------------------------------------------------
  }

  // De-Initialization
  //--------------------------------------------------------------------------------------
  ImGui_ImplRaylib_Shutdown();
  ImGui::DestroyContext();
  CloseWindow(); // Close window and OpenGL context
  //--------------------------------------------------------------------------------------

  return 0;
}
