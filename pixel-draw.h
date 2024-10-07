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
Vector2 operator+(Vector2 lhs, Vector2 rhs);
Vector2 operator*(Vector2 lhs, float rhs);
Vector2 operator/(Vector2 lhs, float rhs);
Vector2 operator/(Vector2 lhsVec, int scalarRhs);
bool operator!=(Color lhsColor, Color rhsColor);



class PixelDraw{
  public: 
    Tool curTool;
    BrushShape curBrushShape;
    int curToolSize = 1;
    Color curDrawingColor = BLACK;
    bool xAxisMirror = false;
    bool yAxisMirror = false;

    PixelDraw(int canvasWidth, int canvasHeight, int pixelBlockSize, Color canvasPixels[], Color mainLayerPixels[], Color tmpLayerPixels[]);
    void ClearLayerPixels(); 
    void ClearCanvasPixels();
    void ChangeLayer(Color mainLayerPixels[], Color tmpLayerPixels[]);
    void FillWithColor(int originX, int originY, Color fillColor);
    void DrawAndStretchCircle(int x0, int y0, int x1, int y1, bool spawnMultipleInstances);
    void DrawCenteredCircle(int centerX, int centerY, int radiusX, int radiusY, bool spawnMultipleInstances);
    void DrawRectangle(int x0, int y0, int x1, int y1);
    void DrawCircle(int originX, int originY, int radius);
    void DrawFilledCircle(int originX, int originY, int radius);
    void DrawFilledSquare(int originX, int originY, 
    int size, Color color);
    void ControlPixelDraw(int drawPosX, int drawPosY, Color color);
    void DrawPixelBlock(int drawPosX, int drawPosY, Color color);
    void Erase(int originX, int drawPosY);
    void DrawWithBrush(int prevOriginX, int prevOriginY, int originX, int originY);
    void DrawWithLine(float x0, float y0, float x1, float y1);
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
    int m_canvasPixelsSize;

    Color *m_canvasPixels;
    Color *m_mainLayerPixels;
    Color *m_tmpLayerPixels;

    void ResetBufferState();

};

