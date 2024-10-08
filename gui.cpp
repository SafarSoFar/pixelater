#include "gui.h"
#include "pixel-draw.h"


GUIControl::GUIControl(PixelDraw *pixelDraw, int screenWidth, int screenHeight, Vector2 *canvasPos, int *canvasWidth, int *canvasHeight){
  m_pixelDraw = pixelDraw;
  m_screenWidth = screenWidth;
  m_screenHeight = screenHeight;

  m_canvasPos = canvasPos;
  m_canvasWidth = canvasWidth;
  m_canvasHeight = canvasHeight;

  ImGui::CreateContext();

  io = ImGui::GetIO(); (void)io;

  SetupStyles(io);
  
}

void GUIControl::SetupStyles(ImGuiIO io){
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

  m_iconFont = io.Fonts->AddFontFromFileTTF( "fonts/" FONT_ICON_FILE_NAME_FAS, iconFontSize, &icons_config, icons_ranges );
  m_textFont = io.Fonts->AddFontFromFileTTF( "fonts/louis-george-cafe.ttf", 20);
  int codepoints[2] = {0xf2b9, 0xf6ad};
  m_rlFontIcons = LoadFontEx("fonts/" FONT_ICON_FILE_NAME_FAS, iconFontSize, codepoints, 2);
  io.Fonts->Build();

  io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;

  /*ImGui::StyleColorsDark();*/

  /*Font rlFontIcons = LoadFontEx("fonts/" FONT_ICON_FILE_NAME_FAS, iconFontSize, ICON_MIN_FA, ICON_MAX_16_FA);*/

  ImGui_ImplRaylib_Init();
  Imgui_ImplRaylib_BuildFontAtlas();


}


void GUIControl::DrawAndControlGUI(){
  ImGui_ImplRaylib_ProcessEvents();
  ImGui_ImplRaylib_NewFrame();
  ImGui::NewFrame();
  
  ImGui::PushFont(m_textFont);
  bool isFunctionsPanelOpened = true;
  ImGui::Begin("Functions panel", &isFunctionsPanelOpened, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize);
  ImGui::SetWindowPos(ImVec2(0.0f, 0.0f));
  ImGui::SetWindowSize(ImVec2(m_screenWidth, 30));

  if(ImGui::BeginMenuBar()){

    if(ImGui::BeginMenu("File")){

      if(ImGui::MenuItem("New Canvas", "Ctrl+N")){
        m_isNewCanvasWindowOpened = true;      
      }

      if(ImGui::MenuItem("Save image", "Ctrl+S")){
        m_isSaveImageWindowOpened = true;      
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

  /*ControlPopUpWindows();*/

  ImGui::Begin("Tool panel");

  ImGui::SetWindowPos(ImVec2(0.0f, 30.0f));


  if (ImGui::Button("Clear Canvas")) {
    m_pixelDraw->ClearCanvas();
  }

  ImGui::SliderInt("Tool Size", &m_pixelDraw->curToolSize, 1, 10);

  ImGui::PopFont();

 /*---- ICONS SECTION -----*/
 
  // Color choosing for drawing
  // Changing color only on release for optimization reasons
  if (ImGui::ColorPicker4(ICON_FA_PALETTE " Brush Color", m_imGuiColorFloat)) {
    if (!m_isColorPickerPressed) {
      m_isColorPickerPressed = true;
    }
  } else {
    if (m_isColorPickerPressed) {
      unsigned char r = m_imGuiColorFloat[0] * 255;
      unsigned char g = m_imGuiColorFloat[1] * 255;
      unsigned char b = m_imGuiColorFloat[2] * 255;
      unsigned char a = m_imGuiColorFloat[3] * 255;
      m_pixelDraw->curDrawingColor = {r, g, b, a};

      m_isColorPickerPressed = false;
    }
  }

  if (ImGui::Button(ICON_FA_PENCIL_RULER " Line")) {
    m_pixelDraw->curTool = Tool::Line;
  }
  if (ImGui::Button(ICON_FA_PAINT_BRUSH " Brush")) {
    m_pixelDraw->curTool = Tool::Brush;
  }
  if(ImGui::CollapsingHeader(ICON_FA_SHAPES " Brush Shapes")){
    if (ImGui::Button("Brush: Circle")) {
      m_pixelDraw->curBrushShape = BrushShape::CircleBrush;
    }
    if (ImGui::Button("Brush: Square")) {
      m_pixelDraw->curBrushShape = BrushShape::SquareBrush;
    }
  }
  if (ImGui::Button(ICON_FA_ERASER " Eraser")) {
    m_pixelDraw->curTool = Tool::Eraser;
  }
  if (ImGui::Button(ICON_FA_EYE_DROPPER "Color Picker")) {
    m_pixelDraw->curTool = Tool::ColorPicker;
  }
  if (ImGui::Button(ICON_FA_CIRCLE " Circle")) {
    m_pixelDraw->curTool = Tool::Circle;
  }
  if (ImGui::Button(ICON_FA_FILL " Fill")) {
    m_pixelDraw->curTool = Tool::Fill;
  }
  if (ImGui::Button( " Rectangle")){
    m_pixelDraw->curTool = Tool::Rect;
  }
  ImGui::Checkbox("Mirror X Axis", &m_pixelDraw->xAxisMirror);
  ImGui::Checkbox("Mirror Y Axis", &m_pixelDraw->yAxisMirror);

  ImGui::End();


  bool isScrollbarsOpen = true;

  /*-----HORIZONTAL VIEW SCROLLBAR--------*/
  ImGui::Begin("Horizontal bar",&isScrollbarsOpen, ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoTitleBar);
  ImGui::PushItemWidth(m_screenWidth-17);
  ImGui::SetWindowPos(ImVec2(0, m_screenHeight-40));
  ImGui::SetWindowSize(ImVec2(m_screenWidth, 40));
  ImGui::SliderFloat("##Horizontal view", &m_canvasPos->x, 0-*m_canvasWidth, m_screenWidth, "%.0f");
  ImGui::End();

  /*-----VERTICAL VIEW SCROLLBAR--------*/
  /*ImGui::Begin("Vertical bar",&isScrollbarsOpen, ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoTitleBar);*/
  /*ImGui::PushItemWidth(g_screenWidth-17);*/
  /*ImGui::SetWindowPos(ImVec2(400, 0));*/
  /*ImGui::SliderFloat("##Vertical view", &g_canvasPos.y, 0-g_canvasHeight, g_screenHeight, "%.0f");*/
  /*ImGui::End();*/



  /*DrawAndControlLayersGUILogic();*/

  ImGui::Render();
  ImGui_ImplRaylib_RenderDrawData(ImGui::GetDrawData());

}


