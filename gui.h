#include "imgui/imgui.h"
#include "rlImGui/imgui_impl_raylib.h"
#include "pixel-draw.h"
#include "raylib.h"
#include <string>
#include <vector>
#include <cstring>
#include <cmath>
#include "IconsFontAwesome5.h"
#include <raylib.h>
#include <raymath.h>

class GUIControl{
  public:
    GUIControl(PixelDraw *pixelDraw, int screenWidth, int screenHeight, Vector2 *canvasPos, int *canvasWidth, int *canvasHeight);
    void DrawAndControlGUI(); 

    ImGuiIO io;

  private:
    void SetupStyles(ImGuiIO io); 
    PixelDraw* m_pixelDraw;
    int m_screenWidth;
    int m_screenHeight;

    // Dynamic values so pointers required
    Vector2 *m_canvasPos;
    int *m_canvasWidth;
    int *m_canvasHeight;

    bool m_isNewCanvasWindowOpened = false;
    bool m_isSaveImageWindowOpened = false;
    bool m_isColorPickerPressed = false;
    float m_imGuiColorFloat[4] = {};

    ImFont* m_textFont;
    ImFont* m_iconFont;
    Font m_rlFontIcons;


};
