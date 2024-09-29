#include "pixel-draw.h"
#include "imgui/imgui.h"
#include <iostream>
#include <raymath.h>


bool operator==(Vector2 lhs, Vector2 rhs) {
  return lhs.x == rhs.x && lhs.y == rhs.y;
}

Vector2 operator-(Vector2 lhs, Vector2 rhs){
  return Vector2{rhs.x-lhs.x, rhs.y - lhs.y};
}

Vector2 operator*(Vector2 lhs, float rhs){
  return Vector2{lhs.x*rhs, lhs.y*rhs};
}
Vector2 operator/(Vector2 lhs, float rhs){
  return Vector2{lhs.x/rhs, lhs.y/rhs};
}

bool operator==(Color lhsColor, Color rhsColor){
  return lhsColor.r == rhsColor.r && lhsColor.g == rhsColor.g && lhsColor.b == rhsColor.b && lhsColor.a == rhsColor.a;
}

Vector2 operator/(Vector2 lhsVec, int scalarRhs){
  return Vector2{lhsVec.x / scalarRhs, lhsVec.y / scalarRhs};
}

bool PixelDraw::IsOutsideOfCanvas(int x, int y){
  return x < 0 || y < 0 || x >= m_canvasWidth || y >= m_canvasHeight; 
}

PixelDraw::PixelDraw(int screenWidth, int screenHeight, int pixelBlockSize, Color tmpCanvasPixels[], Color mainCanvasPixels[]){
  this->curDrawingColor = BLACK;
  this->curTool = Tool::Brush;
  this->curToolSize = 1;
  this->m_pixelBlockSize = pixelBlockSize;

  this->m_canvasWidth = screenWidth;
  this->m_canvasHeight = screenHeight;

  this->m_canvasPixelAmountX = m_canvasWidth/m_pixelBlockSize;
  this->m_canvasPixelAmountY = m_canvasHeight/m_pixelBlockSize;

  this->m_pixelsSize = screenWidth * screenHeight;

  this->m_tmpCanvasPixels = tmpCanvasPixels;
  this->m_mainCanvasPixels = mainCanvasPixels;
}

void PixelDraw::SetColorFromPos(int originX, int originY){
  Color pickedColor = m_mainCanvasPixels[originX + originY * m_canvasWidth];
  this->curDrawingColor = pickedColor;

}

void PixelDraw::DrawRectangle(int x0, int y0, int x1, int y1){

  PixelDraw::ResetTMPBuffer();

  float firstPointX;
  float secondPointX;

  float firstPointY;
  float secondPointY;
  
  if(x0 <= x1){
    firstPointX = x0;
    secondPointX = x1;
  }
  else{
    firstPointX = x1;
    secondPointX = x0;
  }

  if(y0 <= y1){
    firstPointY = y0;
    secondPointY = y1;
  }
  else{
    firstPointY = y1;
    secondPointY = y0;
  }

  // Drawing horizontal lines
  for(int i = firstPointX; i <= secondPointX; i++){
    PixelDraw::ControlPixelDraw(i, y0, curDrawingColor);
    PixelDraw::ControlPixelDraw(i, y1, curDrawingColor);
  }
  // Drawing vertical lines
  for(int i = firstPointY; i <= secondPointY; i++){
    PixelDraw::ControlPixelDraw(x0, i, curDrawingColor);
    PixelDraw::ControlPixelDraw(x1, i, curDrawingColor);
  }
  
  
}

void PixelDraw::DrawAndStretchCircle(int x0, int y0, int x1, int y1, bool spawnMultipleInstances){
  int midX = abs(x1+x0)/2;
  int midY = abs(y1+y0)/2;
  if(!spawnMultipleInstances){
    PixelDraw::ResetTMPBuffer();
  }
  // size as distance
  int size = sqrt(pow(x1-x0, 2) + pow(y1-y0,2));
  for(int i = size; i <= size; i++){
    PixelDraw::DrawCircle(midX, midY, i);
  }
}

void PixelDraw::DrawCenteredCircle(int centerX, int centerY, int radiusX, int radiusY, bool spawnMultipleInstances){
  
  int radius = sqrt(pow(radiusX - centerX,2) + pow(radiusY - centerY,2));
  if(!spawnMultipleInstances){
    PixelDraw::ResetTMPBuffer();
  }

  for(int i = radius; i <= radius+curToolSize; i++){
    PixelDraw::DrawCircle(centerX, centerY, i);
  }

}

void PixelDraw::DrawFilledCircle(int originX, int originY, int radius){
  for(int i = 1; i < radius; i++){
    PixelDraw::DrawCircle(originX, originY, i);
  }
}


void PixelDraw::DrawCircle(int originX, int originY, int radius){
  for(double angle = 0; angle<2*PI; angle+=0.001*m_pixelBlockSize){
    int x0 = originX + radius*cos(angle);
    int y0 = originY + radius*sin(angle);


    PixelDraw::ControlPixelDraw(x0, y0, curDrawingColor); 
  }
}

void PixelDraw::ControlPixelDraw(int drawPosX, int drawPosY, Color color){

  drawPosX -= drawPosX % m_pixelBlockSize;
  drawPosY -= drawPosY % m_pixelBlockSize;

  for(int x = drawPosX; x < drawPosX+(curToolSize*m_pixelBlockSize); x+=m_pixelBlockSize){
    for(int y = drawPosY; y < drawPosY+(curToolSize*m_pixelBlockSize); y+=m_pixelBlockSize){
      DrawPixelBlock(x, y,  color);

      if(xAxisMirror){
        DrawPixelBlock(m_canvasWidth-m_pixelBlockSize-x, y, color);
      }

      if(yAxisMirror){
        DrawPixelBlock(x, m_canvasHeight-m_pixelBlockSize-y, color);
        
        // For both axis mirror
        if(xAxisMirror){
          DrawPixelBlock(m_canvasWidth-m_pixelBlockSize-x, m_canvasHeight-m_pixelBlockSize-y, color);
        }
      }

    }
  }
  
  /*DrawPixelBlock(drawPosX, drawPosY,  color);*/
  /**/
  /*if(xAxisMirror){*/
  /*  DrawPixelBlock(m_canvasWidth-m_pixelBlockSize-drawPosX, drawPosY, color);*/
  /*}*/
  /**/
  /*if(yAxisMirror){*/
  /*  DrawPixelBlock(drawPosX, m_canvasHeight-m_pixelBlockSize-drawPosY, color);*/
  /*  if(xAxisMirror){*/
  /*    DrawPixelBlock(m_canvasWidth-m_pixelBlockSize-drawPosX, m_canvasHeight-m_pixelBlockSize-drawPosY, color);*/
  /*  }*/
  /*}*/
}

void PixelDraw::DrawPixelBlock(int drawPosX, int drawPosY, Color color){

  if(IsOutsideOfCanvas(drawPosX, drawPosY)){
    return;
  }
  
  for(int i = drawPosX; i < m_canvasWidth && i < drawPosX+m_pixelBlockSize; i++){
    for(int j = drawPosY; j < m_canvasHeight && j < drawPosY+m_pixelBlockSize; j++){
      m_tmpCanvasPixels[i + j * m_canvasWidth] = color;
    }
  }
  
}

void PixelDraw::ClearPixels() {
  Color transparentColor = Color{0,0,0,0};
  for (int i = 0; i < m_canvasWidth; i++) {
    for(int j = 0; j < m_canvasHeight; j++){
        m_mainCanvasPixels[i+j*m_canvasWidth] = transparentColor;
        m_tmpCanvasPixels[i+j*m_canvasWidth] = transparentColor;
    }
  }
}

void PixelDraw::FillWithColor(int originX, int originY, Color fillColor){
  if(m_isFillingCanvas)
    return;

  m_isFillingCanvas = true;
  Color colorToFill = m_tmpCanvasPixels[originX + originY * m_canvasWidth];

  // Worst case scenario fill memory usage - fill full screen
  std::vector<std::vector<bool>> isVis(m_canvasWidth, std::vector<bool>(m_canvasHeight, false));
  std::queue<std::pair<int,int>> q;

  q.push({originX, originY});

  while(q.size()){

    std::pair<int,int> coords = q.front();
    q.pop();
    if(!IsOutsideOfCanvas(coords.first, coords.second) && !isVis[coords.first][coords.second] 
        && m_tmpCanvasPixels[coords.first + coords.second * m_canvasWidth] == colorToFill){
      m_tmpCanvasPixels[coords.first + coords.second * m_canvasWidth] = fillColor;
      isVis[coords.first][coords.second] = true;
      q.push({coords.first+1, coords.second});
      q.push({coords.first-1, coords.second});
      q.push({coords.first, coords.second+1});
      q.push({coords.first, coords.second-1});
    }



  }
  m_isFillingCanvas = false;
  /*thread::slle*/
}

void PixelDraw::DrawFilledSquare(int originX, int originY, int size, Color color){
    for (int i = originX - size;
        i <= originX + size; i++) {
      for (int j = originY - size;
          j <= originY + size; j++) {
        if (!IsOutsideOfCanvas(i, j)){
          m_tmpCanvasPixels[i + j * m_canvasWidth] = color;
        }
      }
    }
}

void PixelDraw::Erase(int drawPosX, int drawPosY){
  PixelDraw::ControlPixelDraw(drawPosX, drawPosY, BLANK);
}

void PixelDraw::DrawWithBrush(int prevOriginX, int prevOriginY, int originX, int originY) {

  float timeDelta = GetFrameTime();
  float time = 0.5f;
  while(timeDelta < time){
    int intermediateX = Lerp(prevOriginX, originX, timeDelta/time);
    int intermediateY = Lerp(prevOriginY, originY, timeDelta/time);

    /*if(curBrushShape == BrushShape::SquareBrush){*/
    /*  DrawFilledSquare(intermediateX, intermediateY, curToolSize, colorToDraw);*/
    /*}*/
    /*else{*/
    /*  DrawCircle(intermediateX, intermediateY, curToolSize, colorToDraw);*/
    /*}*/

    ControlPixelDraw(intermediateX, intermediateY, curDrawingColor);

    timeDelta += GetFrameTime();
  }

}

void PixelDraw::DrawWithLine(float x0, float y0, float x1, float y1) {


  // checking screen boundaries to prevent segfault
  if(IsOutsideOfCanvas(x0, y0) || IsOutsideOfCanvas(x1, y1)){
    return;
  }

  // resetting the buffer to draw only one line at the time
  PixelDraw::ResetTMPBuffer();

  float x = x1 - x0;
  float y = y1 - y0;

  const float max = std::max(std::fabs(x), std::fabs(y));

  x /= max;
  y /= max;

  for (int i = 0; i < max; i++) {

    for(int width = x0 - curToolSize; width <= x0 + curToolSize; width++){
      for(int height = y0 - curToolSize; height <= y0 + curToolSize; height++){
        if(!IsOutsideOfCanvas(width, height)){
          PixelDraw::ControlPixelDraw(width, height, curDrawingColor);
        }
      }
    }

    x0 += x;
    y0 += y;

  }
}

void PixelDraw::ResetTMPBuffer(){
  // resetting the buffer to draw only one instance at the time
  memcpy(m_tmpCanvasPixels, m_mainCanvasPixels, m_pixelsSize * sizeof(Color));
}













