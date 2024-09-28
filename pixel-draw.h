#pragma once

#include<raylib.h>
#include <cmath>
#include <cstring>
#include <vector>
#include <queue>


enum CanvasPixelType{
  Main,
  Tmp,
};
  
enum Tool {
  Brush,
  Line,
  Rect,
  Eraser,
  ColorPicker,
  Circle,
  Fill
};

enum BrushShape{
  SquareBrush,
  CircleBrush
};


bool operator==(Color lhs, Color rhs);
bool operator==(Color lhsColor, Color rhsColor);

Vector2 operator-(Vector2 lhs, Vector2 rhs);
Vector2 operator*(Vector2 lhs, float rhs);
Vector2 operator/(Vector2 lhs, float rhs);
Vector2 operator/(Vector2 lhsVec, int scalarRhs);



class PixelDraw{
  public: 
    Tool curTool;
    BrushShape curBrushShape;
    int curToolSize = 1;
    Color curDrawingColor = BLACK;
    bool xAxisMirror = false;
    bool yAxisMirror = false;

    PixelDraw(int canvasWidth, int canvasHeight, int pixelBlockSize, Color tmpCanvasPixels[], Color mainCanvasPixels[]);
    void ClearPixels(); 
    void FillWithColor(int originX, int originY, Color fillColor);
    void DrawAndStretchCircle(int x0, int y0, int x1, int y1, Color color, bool spawnMultipleInstances);
    void DrawCenteredCircle(int centerX, int centerY, int radiusX, int radiusY, Color color, bool spawnMultipleInstances);
    void DrawCircle(int originX, int originY, int radius);
    void DrawFilledCircle(int originX, int originY, int radius);
    void DrawFilledSquare(int originX, int originY, 
    int size, Color color);
    void ControlPixelDraw(int drawPosX, int drawPosY, Color color);
    void DrawPixelBlock(int drawPosX, int drawPosY, Color color, bool isMirrored);
    void Erase(int originX, int drawPosY);
    void DrawWithBrush(int prevOriginX, int prevOriginY, int originX, int originY);
    void DrawWithLine(float x0, float y0, float x1, float y1);
    void DrawWithRectangle();
    void SetColorFromPos(int originX, int originY);
    bool IsOutsideOfCanvas(int x, int y);
    void Draw();

  private:
    bool m_isFillingCanvas = false;
    int m_pixelBlockSize;
    int m_canvasPixelAmountX;
    int m_canvasPixelAmountY;
    int m_canvasWidth;
    int m_canvasHeight;
    int m_pixelsSize;
    Color *m_tmpCanvasPixels;
    Color *m_mainCanvasPixels;

    void ResetTMPBuffer();

};

