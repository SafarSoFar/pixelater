#include "imgui/imgui.h"

#include "rlImGui/imgui_impl_raylib.h"
#include "pixel-draw.h"
#include "raylib.h"
#include <iostream>
#include <sstream>
#include <vector>
#include <cstring>
#include "IconsFontAwesome5.h"
#include <raylib.h>
#include <raymath.h>

/*#define STB_IMAGE_IMPLEMENTATION*/
#include "stb/stb_image.h"
/*#define STB_IMAGE_WRITE_IMPLEMENTATION*/
#include "stb/stb_image_write.h"

using std::vector;

#define MAX_OUTPUT_FILE_SIZE 10


// global logic in order to make web build as well

char g_outputFileName[MAX_OUTPUT_FILE_SIZE];

int g_screenWidth = 1280;
int g_screenHeight = 900;
int g_canvasWidth = 512;
int g_canvasHeight = 512;
int g_pixelBlockSize = g_canvasWidth/32; // basic 32x32
int g_canvasPixelsSize = g_canvasWidth * g_canvasHeight;

float g_imGuiColorFloat[4]{0.0f, 0.0f, 0.0f, 1.0f}; // BLACK Color
float g_canvasScale = 1.0f;
float g_canvasScaleMin = 0.2f;
float g_canvasScaleMax = 5.0f;

// using texture like global increase performance.
Texture2D g_mainCanvasTexture;
Texture2D g_tmpCanvasTexture;
Texture2D g_transparentTexture;

ImFont* g_iconFont; 
ImFont* g_textFont; 
Font g_rlFontIcons;

bool g_isHoldingLMB;
bool g_isMouseDraggingCanvas = false;
bool g_canInteractWithCanvas = true;
bool g_isSaveImageWindowOpen = false;
bool g_isNewCanvasWindowOpen = false;
bool g_isColorPickerPressed;


Vector2 g_canvasPos;
Vector2 g_canvasPosBeforeDrag;
Vector2 g_LMBHoldingFirstPos = Vector2Zero();
Vector2 g_secondLastMousePos = Vector2Zero();
Vector2 g_lastMousePos = Vector2Zero();
Vector2 g_secondLastMousePosOnCanvas = Vector2Zero();
Vector2 g_lastMousePosOnCanvas = Vector2Zero();
Vector2 g_verticalCenterLineStart = Vector2Zero();
Vector2 g_verticalCenterLineEnd = Vector2Zero();
Vector2 g_horizontalCenterLineStart = Vector2Zero();
Vector2 g_horizontalCenterLineEnd = Vector2Zero();

Color *g_mainCanvasPixels = new Color[g_canvasPixelsSize];
Color *g_tmpCanvasPixels = new Color[g_canvasPixelsSize];
vector<vector<Color>> g_undoCanvasColorPixels;
vector<vector<Color>> g_redoCanvasColorPixels;

PixelDraw g_pixelDraw(g_canvasWidth, g_canvasHeight, g_pixelBlockSize, g_tmpCanvasPixels, g_mainCanvasPixels);


void CenterCanvasPos(){
  g_canvasPos.x = g_screenWidth/2-(g_canvasWidth*g_canvasScale)/2;
  g_canvasPos.y = g_screenHeight/2-(g_canvasHeight*g_canvasScale)/2;
}

void SetTransparentTexture(){
  Color* transparentTexturePixels = new Color[g_canvasPixelsSize]; 
  int countX = 0;
  int countY = 0;
  bool isGray = true;
  for(int i = 0; i < g_canvasWidth; i++){
    if(countX >= g_pixelBlockSize){
      isGray = !isGray;
      countX = 0;
    }
    countX++;
    for(int j = 0; j < g_canvasHeight; j++){
      if(countY >= g_pixelBlockSize){
        isGray = !isGray;
        countY = 0;
      }
      transparentTexturePixels[i+j*g_canvasWidth] = isGray ? GRAY : LIGHTGRAY;
      countY++;
    }
  }
  UpdateTexture(g_transparentTexture,transparentTexturePixels);

}

void CreateCanvas(int pixelSize){
  g_pixelBlockSize = pixelSize;
  g_pixelDraw = PixelDraw(g_canvasWidth, g_canvasHeight, g_pixelBlockSize, g_tmpCanvasPixels, g_mainCanvasPixels);
  CenterCanvasPos();
  g_pixelDraw.ClearPixels();
  SetTransparentTexture();
  g_undoCanvasColorPixels.clear();
  g_redoCanvasColorPixels.clear();
}


void SavePixelArt(char fileName[10]){
  stbi_write_png(fileName, g_canvasWidth,  g_canvasHeight, 4,  g_mainCanvasPixels, g_canvasWidth*4);
}


void AddCanvasToUndo(){
    vector<Color> prevArr(g_mainCanvasPixels, g_mainCanvasPixels+g_canvasPixelsSize);

    // Copying C-array content to std::array 
    /*copy(g_mainCanvasPixels, g_mainCanvasPixels+g_pixelsSize, prevArr.begin());*/
    /*memcpy(prevArr, &g_mainCanvasPixels, g_pixelsSize * sizeof(Color));*/

    // delete the limited step undo
    if(g_undoCanvasColorPixels.size() >= 30){
      g_undoCanvasColorPixels.erase(g_undoCanvasColorPixels.begin());
    }

    g_undoCanvasColorPixels.push_back(prevArr);

}




/*void InterpolateBrush(){*/
/*  float timeDelta = GetFrameTime();*/
/*  float time = 0.5f;*/
/*  while(timeDelta < time){*/
/*    Vector2 intermidiate = Vector2Lerp(g_prevLastMousePos, g_lastMousePos, timeDelta/time);*/
/*    g_pixelDraw.DrawWithBrush(intermidiate.x, intermidiate.y, g_pixelDraw.curDrawingColor);*/
/*    timeDelta += GetFrameTime();*/
/*  }*/
/*}*/

void Draw() {
  switch (g_pixelDraw.curTool) {  

    case Line:
      g_pixelDraw.DrawWithLine(g_LMBHoldingFirstPos.x, g_LMBHoldingFirstPos.y, g_lastMousePosOnCanvas.x, g_lastMousePosOnCanvas.y);
    break;

    case Brush:
      g_pixelDraw.DrawWithBrush(g_secondLastMousePosOnCanvas.x, g_secondLastMousePosOnCanvas.y, g_lastMousePosOnCanvas.x, g_lastMousePosOnCanvas.y);
    break;

    case Rect:
      g_pixelDraw.DrawRectangle(g_LMBHoldingFirstPos.x, g_LMBHoldingFirstPos.y, g_lastMousePosOnCanvas.x, g_lastMousePosOnCanvas.y);
    break;

    case Eraser:
      g_pixelDraw.Erase(g_lastMousePosOnCanvas.x, g_lastMousePosOnCanvas.y);
    break;

    case Circle:

      // The first mouse position is the center, if user is holding left alt - don't delete intermidiate stepsinclude "IconsFontAwesome5.h"
      if(IsKeyDown(KEY_LEFT_CONTROL)){
        g_pixelDraw.DrawCenteredCircle(g_LMBHoldingFirstPos.x, g_LMBHoldingFirstPos.y,
            g_lastMousePosOnCanvas.x, g_lastMousePosOnCanvas.y, IsKeyDown(KEY_LEFT_ALT));
      }
      // Center is calculated as the midpoint of two mouse vectors, if user is holding left alt - don't delete intermidiate steps
      else{
        g_pixelDraw.DrawAndStretchCircle(g_LMBHoldingFirstPos.x, g_LMBHoldingFirstPos.y,
            g_lastMousePosOnCanvas.x, g_lastMousePosOnCanvas.y, IsKeyDown(KEY_LEFT_ALT));
      }
    break;

    case ColorPicker:
    // Changes tool raylib color
    g_pixelDraw.SetColorFromPos(g_lastMousePosOnCanvas.x, g_lastMousePosOnCanvas.y);

    // To change imgui color
    g_imGuiColorFloat[0] = {g_pixelDraw.curDrawingColor.r/255.0f};
    g_imGuiColorFloat[1] = {g_pixelDraw.curDrawingColor.g/255.0f};
    g_imGuiColorFloat[2] = {g_pixelDraw.curDrawingColor.b/255.0f};
    g_imGuiColorFloat[3] = {g_pixelDraw.curDrawingColor.a/255.0f};

    break;

    case Fill:
      g_pixelDraw.FillWithColor(g_lastMousePosOnCanvas.x, g_lastMousePosOnCanvas.y, g_pixelDraw.curDrawingColor);
    break;
  }
  UpdateTexture(g_tmpCanvasTexture, g_tmpCanvasPixels);
}

void AddCanvasToRedo(){
    vector<Color> curArr(g_mainCanvasPixels, g_mainCanvasPixels+g_canvasPixelsSize);

    // Copying C-array content to std::array 
    /*copy(g_mainCanvasPixels, g_mainCanvasPixels+g_pixelsSize, prevArr.begin());*/
    /*memcpy(prevArr, &g_mainCanvasPixels, g_pixelsSize * sizeof(Color));*/

    // delete the limited step undo
    if(g_redoCanvasColorPixels.size() >= 30){
      g_redoCanvasColorPixels.erase(g_undoCanvasColorPixels.begin());
    }

    g_redoCanvasColorPixels.push_back(curArr);
}

void StepsControl(){
  if(IsKeyDown(KEY_LEFT_CONTROL) && IsKeyPressed(KEY_Z)){

    // If Left Shift is down - will not go to undo
    if(IsKeyDown(KEY_LEFT_SHIFT)){
      if(g_redoCanvasColorPixels.size()){

        // Adding current canvas to undo
        AddCanvasToUndo(); 

        auto step = g_redoCanvasColorPixels.back();
        g_redoCanvasColorPixels.pop_back();

        memcpy(g_mainCanvasPixels, step.data(), g_canvasPixelsSize * sizeof(Color));
        /*memcpy(g_mainCanvasPixels, prev, g_pixelsSize * sizeof(Color));*/

        memcpy(g_tmpCanvasPixels, g_mainCanvasPixels, g_canvasPixelsSize * sizeof(Color));

        UpdateTexture(g_mainCanvasTexture, g_mainCanvasPixels);
        UpdateTexture(g_tmpCanvasTexture, g_tmpCanvasPixels);
        
      }

      return;
    }

    if(g_undoCanvasColorPixels.size()){
      // Adding current canvas to redo
      AddCanvasToRedo(); 

      auto prevStep = g_undoCanvasColorPixels.back();
      g_undoCanvasColorPixels.pop_back();

      memcpy(g_mainCanvasPixels, prevStep.data(), g_canvasPixelsSize * sizeof(Color));
      /*memcpy(g_mainCanvasPixels, prev, g_pixelsSize * sizeof(Color));*/

      memcpy(g_tmpCanvasPixels, g_mainCanvasPixels, g_canvasPixelsSize * sizeof(Color));

      UpdateTexture(g_mainCanvasTexture, g_mainCanvasPixels);
      UpdateTexture(g_tmpCanvasTexture, g_tmpCanvasPixels);
    }
  }
}

// Doesn't work yet
/*void ControlCursor(){*/

  /*DrawTextCodepoint(g_rlFontIcons, 0xf2c0, g_lastMousePos, 20, BLACK);*/
  /*DrawTextCodepoint(Font font, int codepoint, Vector2 position, float fontSize, Color tint)*/

  /*std::cout<<" : " ICON_FA_BRUSH<<'\n';*/

  /*DrawTextCodepoint(g_rlFontIcons, 0xf6ac, g_lastMousePos, 20, BLACK);*/
/*}*/



void SetMousePositions(){

  g_secondLastMousePos = g_lastMousePos;
  g_secondLastMousePosOnCanvas = g_lastMousePosOnCanvas;

  g_lastMousePos = GetMousePosition();

  if(g_isMouseDraggingCanvas){
    g_lastMousePosOnCanvas = (g_canvasPosBeforeDrag - g_lastMousePos) / g_canvasScale;
    return;
  }
  g_lastMousePosOnCanvas = (g_canvasPos - g_lastMousePos) / g_canvasScale;
}

void ControlGUIHotKeys(){
  if(IsKeyDown(KEY_LEFT_CONTROL)){
    if(IsKeyPressed(KEY_S)){
      g_isSaveImageWindowOpen = true;
    }
  }
}

void UpdateCenterLines(){
  g_verticalCenterLineStart = Vector2{g_canvasPos.x+(g_canvasWidth*g_canvasScale/2), g_canvasPos.y};
  g_verticalCenterLineEnd = Vector2{g_canvasPos.x+(g_canvasWidth*g_canvasScale/2), (float)g_canvasPos.y+g_canvasHeight*g_canvasScale};
  g_horizontalCenterLineStart = Vector2{g_canvasPos.x, g_canvasPos.y+(g_canvasHeight*g_canvasScale/2)};
  g_horizontalCenterLineEnd = Vector2{(float)g_canvasWidth*g_canvasScale+g_canvasPos.x, g_canvasPos.y+(g_canvasHeight*g_canvasScale/2)};
}


void ControlCanvasTransform(){

  Vector2 mouseWheel = GetMouseWheelMoveV();
  /*std::cout<<mouseWheel.x<<" X "<<'\n';*/
  /*std::cout<<mouseWheel.y<<" Y "<<'\n';*/

  if(IsKeyDown(KEY_LEFT_CONTROL)){

    if(IsKeyPressed(KEY_EQUAL) || mouseWheel.y > 0.1f){
      if(g_canvasScale+0.2f > g_canvasScaleMax){
        return;
      }
      g_canvasScale += 0.2f;
      CenterCanvasPos();

    }
    else if(IsKeyPressed(KEY_MINUS) || mouseWheel.y < -0.1f){

      if(g_canvasScale-0.2f < g_canvasScaleMin){
        return;
      }

      g_canvasScale -= 0.2f;
      CenterCanvasPos();

    }
  }
  
  // TODO when clicking at the same spot, canvas can jump to two positions for whatever reason
  if(g_isHoldingLMB && IsKeyDown(KEY_LEFT_SHIFT)){
    if(!g_isMouseDraggingCanvas){
      g_canvasPosBeforeDrag = g_canvasPos;
    }
    g_isMouseDraggingCanvas = true;
    Vector2 differenceVector = g_LMBHoldingFirstPos - g_lastMousePosOnCanvas; 
    g_canvasPos = g_canvasPosBeforeDrag-differenceVector;
  }
  else if(!g_isHoldingLMB && g_isMouseDraggingCanvas){
    g_isMouseDraggingCanvas = false;
  }

  // have to update center highlighting lines after canvas transformation
  UpdateCenterLines();
}

void ControlPopUpWindows(){
  if(g_isSaveImageWindowOpen){
    ImGui::Begin("File",&g_isSaveImageWindowOpen);

    ImGui::InputText("Enter the file name",  g_outputFileName, MAX_OUTPUT_FILE_SIZE);

    if(ImGui::Button("Save to PNG")){
      SavePixelArt(g_outputFileName);
      // Closing window after confirming
      g_isSaveImageWindowOpen = false;
    }

    ImGui::End();
  }

  if(g_isNewCanvasWindowOpen){
    ImGui::Begin("Canvas",&g_isNewCanvasWindowOpen);

    /*ImGui::InputInt("Width", &g_canvasWidth, 0, 0);*/
    /*ImGui::InputInt("Height", &g_canvasHeight, 0,0);*/

    const char* items[] = {"8x8", "16x16", "32x32", "64x64", "128x128", "256x256"};
    static int selectedItemIndex = 0;

    if(ImGui::BeginListBox("Pixel size")){
      for (int n = 0; n < IM_ARRAYSIZE(items); n++)
      {
        const bool is_selected = (selectedItemIndex == n);
        if (ImGui::Selectable(items[n], is_selected))
            selectedItemIndex = n;

        // Set the initial focus when opening the combo (scrolling + keyboard navigation focus)
        if (is_selected)
            ImGui::SetItemDefaultFocus();
      }
      ImGui::EndListBox();
    }
    auto v = items[selectedItemIndex];
    if(ImGui::Button("Create Canvas")){
      switch (selectedItemIndex){
        case 0:
          CreateCanvas(g_canvasWidth/8);
        break;
        case 1:
          CreateCanvas(g_canvasWidth/16);
        break;
        case 2: 
          CreateCanvas(g_canvasWidth/32);
        break;
        case 3: 
          CreateCanvas(g_canvasWidth/64);
        break;
        case 4: 
          CreateCanvas(g_canvasWidth/128);
        break;
        case 5: 
          CreateCanvas(g_canvasWidth/256);
        break;
      }

      // Closing window after confirming
      g_isNewCanvasWindowOpen = false; 
    }

    ImGui::End();
  }
}

void DrawAndControlGUI() {

  ImGui_ImplRaylib_ProcessEvents();
  ImGui_ImplRaylib_NewFrame();
  ImGui::NewFrame();
  
  ImGui::PushFont(g_textFont);
  bool isFunctionsPanelOpened = true;
  ImGui::Begin("Functions panel", &isFunctionsPanelOpened, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize);
  ImGui::SetWindowPos(ImVec2(0.0f, 0.0f));
  ImGui::SetWindowSize(ImVec2(g_screenWidth, 30));

  if(ImGui::BeginMenuBar()){

    if(ImGui::BeginMenu("File")){

      if(ImGui::MenuItem("New Canvas", "Ctrl+N")){
        g_isNewCanvasWindowOpen = true;      
      }

      if(ImGui::MenuItem("Save image", "Ctrl+S")){
        g_isSaveImageWindowOpen = true;      
      }

      ImGui::EndMenu();

    } 

    if(ImGui::BeginMenu("Donate")){
      if(ImGui::MenuItem("Patreon")){
        OpenURL("https://www.patreon.com/SoFarDevelopment");
      }
      ImGui::EndMenu();
    }
    ImGui::EndMenuBar();
  }
  ImGui::End(); // Functions panel

  ControlPopUpWindows();

  ImGui::Begin("Tool panel");

  ImGui::SetWindowPos(ImVec2(0.0f, 30.0f));


  if (ImGui::Button("Clear Canvas")) {
    g_pixelDraw.ClearPixels();
    UpdateTexture(g_mainCanvasTexture, g_mainCanvasPixels);
    UpdateTexture(g_tmpCanvasTexture, g_mainCanvasPixels);
  }

  ImGui::SliderInt("Tool Size", &g_pixelDraw.curToolSize, 1, 10);

  ImGui::PopFont();

 /*---- ICONS SECTION -----*/
 
  // Color choosing for drawing
  // Changing color only on release for optimization reasons
  if (ImGui::ColorPicker4(ICON_FA_PALETTE " Brush Color", g_imGuiColorFloat)) {
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

  if (ImGui::Button(ICON_FA_PENCIL_RULER " Line")) {
    g_pixelDraw.curTool = Tool::Line;
  }
  if (ImGui::Button(ICON_FA_PAINT_BRUSH " Brush")) {
    g_pixelDraw.curTool = Tool::Brush;
  }
  if(ImGui::CollapsingHeader(ICON_FA_SHAPES " Brush Shapes")){
    if (ImGui::Button("Brush: Circle")) {
      g_pixelDraw.curBrushShape = BrushShape::CircleBrush;
    }
    if (ImGui::Button("Brush: Square")) {
      g_pixelDraw.curBrushShape = BrushShape::SquareBrush;
    }
  }
  if (ImGui::Button(ICON_FA_ERASER " Eraser")) {
    g_pixelDraw.curTool = Tool::Eraser;
  }
  if (ImGui::Button(ICON_FA_EYE_DROPPER "Color Picker")) {
    g_pixelDraw.curTool = Tool::ColorPicker;
  }
  if (ImGui::Button(ICON_FA_CIRCLE " Circle")) {
    g_pixelDraw.curTool = Tool::Circle;
  }
  if (ImGui::Button(ICON_FA_FILL " Fill")) {
    g_pixelDraw.curTool = Tool::Fill;
  }
  if (ImGui::Button( " Rectangle")){
    g_pixelDraw.curTool = Tool::Rect;
  }
  ImGui::Checkbox("Mirror X Axis", &g_pixelDraw.xAxisMirror);
  ImGui::Checkbox("Mirror Y Axis", &g_pixelDraw.yAxisMirror);

  ImGui::End();


  bool isScrollbarsOpen = true;

  /*-----HORIZONTAL VIEW SCROLLBAR--------*/
  ImGui::Begin("Horizontal bar",&isScrollbarsOpen, ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoTitleBar);
  ImGui::PushItemWidth(g_screenWidth-17);
  ImGui::SetWindowPos(ImVec2(0, g_screenHeight-40));
  ImGui::SetWindowSize(ImVec2(g_screenWidth, 40));
  ImGui::SliderFloat("##Horizontal view", &g_canvasPos.x, 0-g_canvasWidth, g_screenWidth, "%.0f");
  ImGui::End();

  /*-----VERTICAL VIEW SCROLLBAR--------*/
  /*ImGui::Begin("Vertical bar",&isScrollbarsOpen, ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoTitleBar);*/
  /*ImGui::PushItemWidth(g_screenWidth-17);*/
  /*ImGui::SetWindowPos(ImVec2(400, 0));*/
  /*ImGui::SliderFloat("##Vertical view", &g_canvasPos.y, 0-g_canvasHeight, g_screenHeight, "%.0f");*/
  /*ImGui::End();*/

  ImGui::Render();
  ImGui_ImplRaylib_RenderDrawData(ImGui::GetDrawData());

}


void SetupStyles(ImGuiIO io){
  ImGuiStyle* style = &ImGui::GetStyle();
  style->Colors[ImGuiCol_Border] = ImVec4(0.5f, 0.5f, 0.5f, 1.0f);
  style->Colors[ImGuiCol_Button] = ImVec4(0.0f,0.0f,0.0f,1.0f);
  style->Colors[ImGuiCol_Header] = ImVec4(0.0f,0.0f,0.0f,1.0f);
  style->Colors[ImGuiCol_FrameBg] = ImVec4(0.0f,0.0f,0.0f,1.0f);
  /*style->Colors[ImGuiCol_TitleBg] = ImVec4(0.0f,0.0f,0.0f,1.0f);*/
  /*style->FrameBorderSize = 1;*/
  style->FrameRounding = 3;
  style->ChildRounding = 3;

  io.Fonts->AddFontDefault();

  float baseFontSize = 25.0f;
  float iconFontSize = baseFontSize * 2.0f / 3.0f;
  /*static const ImWchar rangesFixed[] = {*/
  /*  0x0020, 0x00FF, // Basic Latin + Latin Supplement*/
  /*  0x2026, 0x2026, // ellipsis*/
  /*  0*/
  /*};*/
  static const ImWchar icons_ranges[] = {ICON_MIN_FA, ICON_MAX_16_FA, 0};

  ImFontConfig icons_config; 
  icons_config.MergeMode = true; 
  /*io.Fonts->TexDesiredWidth = 2;*/

  g_iconFont = io.Fonts->AddFontFromFileTTF( "fonts/" FONT_ICON_FILE_NAME_FAS, iconFontSize, &icons_config, icons_ranges );
  g_textFont = io.Fonts->AddFontFromFileTTF( "fonts/louis-george-cafe.ttf", 20);
  int codepoints[2] = {0xf2b9, 0xf6ad};
  g_rlFontIcons = LoadFontEx("fonts/" FONT_ICON_FILE_NAME_FAS, iconFontSize, codepoints, 2);
  io.Fonts->Build();

  io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;

  /*ImGui::StyleColorsDark();*/

  /*Font rlFontIcons = LoadFontEx("fonts/" FONT_ICON_FILE_NAME_FAS, iconFontSize, ICON_MIN_FA, ICON_MAX_16_FA);*/

  ImGui_ImplRaylib_Init();
  Imgui_ImplRaylib_BuildFontAtlas();

}

void ShouldHighlightCenter(){
  if(IsKeyDown(KEY_LEFT_SHIFT)){
    DrawLineEx(g_verticalCenterLineStart, g_verticalCenterLineEnd, 3.0f,BLACK);
    DrawLineEx(g_horizontalCenterLineStart, g_horizontalCenterLineEnd, 3.0f,BLACK);
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

  /*SetConfigFlags(FLAG_WINDOW_RESIZABLE | FLAG_WINDOW_HIGHDPI);*/
  InitWindow(g_screenWidth, g_screenHeight, "Pixelater");

  CenterCanvasPos();

  ImGui::CreateContext();

  ImGuiIO& io = ImGui::GetIO(); (void)io;

  SetupStyles(io);

  g_pixelDraw.ClearPixels();

  Image g_image = {g_mainCanvasPixels, g_canvasWidth, g_canvasHeight, 1,
                   PIXELFORMAT_UNCOMPRESSED_R8G8B8A8};

  g_mainCanvasTexture = LoadTextureFromImage(g_image);
  g_tmpCanvasTexture = LoadTextureFromImage(g_image);

  g_transparentTexture = LoadTextureFromImage(g_image);
  SetTransparentTexture();

  /*ToggleFullscreen();*/
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

    DrawTextureEx(g_transparentTexture, Vector2{g_canvasPos.x,g_canvasPos.y}, 0.0f, g_canvasScale,WHITE);

    BeginDrawing();

    DrawAndControlGUI();


    ControlGUIHotKeys();

    /*ControlCursor();*/

    SetMousePositions();
    ControlCanvasTransform();

    StepsControl();
    
    ShouldHighlightCenter();

    /*DrawSizeCursor();*/

    

    if (IsMouseButtonDown(MOUSE_BUTTON_LEFT) && !io.WantCaptureMouse) {

      if (!g_isHoldingLMB) {
        g_isHoldingLMB = true;
        g_LMBHoldingFirstPos = g_lastMousePosOnCanvas;
      }

      DrawTextureEx(g_tmpCanvasTexture, Vector2{g_canvasPos.x,g_canvasPos.y}, 0.0f, g_canvasScale,WHITE);

      Draw();

    } else {

      if (g_isHoldingLMB) {
        g_isHoldingLMB = false;
      }

      DrawTextureEx(g_mainCanvasTexture, Vector2{g_canvasPos.x,g_canvasPos.y}, 0.0f, g_canvasScale,WHITE);

    }
    if(IsMouseButtonReleased(MOUSE_BUTTON_LEFT)){

      AddCanvasToUndo();

      memcpy(g_mainCanvasPixels, g_tmpCanvasPixels, g_canvasPixelsSize * sizeof(Color));

      UpdateTexture(g_mainCanvasTexture, g_mainCanvasPixels);

      // Means that we can delete redo after undo's
      if(g_redoCanvasColorPixels.size()){
        g_redoCanvasColorPixels.clear();
      }
      
    }

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
