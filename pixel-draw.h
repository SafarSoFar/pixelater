#pragma once

#include<raylib.h>
#include <cmath>
#include <cstring>
#include <vector>
#include <queue>

  
enum Tool {
  Brush,
  Line,
  Rect,
  Eraser,
  Circle,
  Fill
};

enum BrushShape{
  SquareBrush,
  CircleBrush
};


bool operator==(Color lhs, Color rhs);

class PixelDraw{
  public: 
    Tool curTool;
    BrushShape curBrushShape;
    int curToolSize = 1;
    Color curDrawingColor = BLACK;

    PixelDraw(int screenWidth, int screenHeight, Color tmpCanvasPixels[], Color mainCanvasPixels[]);
    void ClearPixels(Color pixels[], int dataSize);
    void FillWithColor(int originX, int originY, Color fillColor);
    void DrawAndStretchCircle(int x0, int y0, int x1, int y1, Color color, bool spawnMultipleInstances);
    void DrawCenteredCircle(int centerX, int centerY, int radiusX, int radiusY, Color color, bool spawnMultipleInstances);
    void DrawCircle(int originX, int originY, int radius, Color color);
    void DrawFilledCircle(int originX, int originY, int radius, Color color);
    void DrawFilledSquare(int originX, int originY, 
    int size, Color color);
    void DrawWithBrush(int originX, int originY, Color colorToDraw);
    void DrawWithLine(float x0, float y0, float x1, float y1);
    void DrawWithRectangle();
    bool IsOutsideOfScreen(int x, int y);
    void Draw();

  private:
    bool m_isFillingCanvas = false;
    int m_screenWidth;
    int m_screenHeight;
    int m_pixelsSize;
    Color *m_tmpCanvasPixels;
    Color *m_mainCanvasPixels;

    void ResetTMPBuffer();

};

