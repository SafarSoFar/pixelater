#include "pixel-draw.h"


bool operator==(Color lhs, Color rhs){
  return lhs.r == rhs.r && lhs.g == rhs.g && lhs.b == rhs.b && lhs.a == rhs.a;
}

bool PixelDraw::IsOutsideOfScreen(int x, int y){
  return x < 0 || y < 0 || x >= m_screenWidth || y >= m_screenHeight; 
}

PixelDraw::PixelDraw(int screenWidth, int screenHeight, Color tmpCanvasPixels[], Color mainCanvasPixels[]){
  this->m_screenWidth = screenWidth;
  this->m_screenHeight = screenHeight;
  this->m_pixelsSize = screenWidth * screenHeight;
  this->m_tmpCanvasPixels = tmpCanvasPixels;
  this->m_mainCanvasPixels = mainCanvasPixels;
}


void PixelDraw::DrawCircle(int originX, int originY, int size, Color color){
  for(double angle = 0; angle<2*PI; angle+=0.01){
    for(int radius = 0; radius < size; radius++){
      int x0 = originX + radius*cos(angle);
      int y0 = originY + radius*sin(angle);
      m_tmpCanvasPixels[x0 + y0 * m_screenWidth] = color;  
    }
  }
}

void PixelDraw::ClearPixels(Color pixels[], int dataSize) {
  for (int i = 0; i < dataSize; i++) {
    pixels[i] = WHITE;
  }
}

void PixelDraw::FillWithColor(int originX, int originY, Color fillColor){
  if(m_isFillingCanvas)
    return;

  m_isFillingCanvas = true;
  Color colorToFill = m_tmpCanvasPixels[originX + originY * m_screenWidth];

  // Worst case scenario fill memory usage - fill full screen
  std::vector<std::vector<bool>> isVis(m_screenWidth, std::vector<bool>(m_screenHeight, false));
  std::queue<std::pair<int,int>> q;

  q.push({originX, originY});

  while(q.size()){

    std::pair<int,int> coords = q.front();
    q.pop();
    if(!IsOutsideOfScreen(coords.first, coords.second) && !isVis[coords.first][coords.second] 
        && m_tmpCanvasPixels[coords.first + coords.second * m_screenWidth] == colorToFill){
      m_tmpCanvasPixels[coords.first + coords.second * m_screenWidth] = fillColor;
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
        if (!IsOutsideOfScreen(i, j)){
          m_tmpCanvasPixels[i + j * m_screenWidth] = color;
        }
      }
    }
}


void PixelDraw::DrawWithBrush(int originX, int originY, Color colorToDraw) {

  if(curBrushShape == BrushShape::SquareBrush){
    DrawFilledSquare(originX, originY, curBrushSize, colorToDraw);
  }
  else{
    DrawCircle(originX, originY, curBrushSize, colorToDraw);
  }

}

void PixelDraw::DrawWithLine(float x0, float y0, float x1, float y1) {


  // checking screen boundaries to prevent segfault
  if(IsOutsideOfScreen(x0, y0) || IsOutsideOfScreen(x1, y1)){
    return;
  }

  // resetting the buffer to draw only one line at the time
  memcpy(m_tmpCanvasPixels, m_mainCanvasPixels, m_pixelsSize * sizeof(Color));

  float x = x1 - x0;
  float y = y1 - y0;

  const float max = std::max(std::fabs(x), std::fabs(y));

  x /= max;
  y /= max;

  for (int i = 0; i < max; i++) {

    for(int width = x0 - curBrushSize; width <= x0 + curBrushSize; width++){
      for(int height = y0 - curBrushSize; height <= y0 + curBrushSize; height++){
        if(!IsOutsideOfScreen(width, height)){
          m_tmpCanvasPixels[(int)width + (int)height * m_screenWidth] = curDrawingColor;
        }
      }
    }

    x0 += x;
    y0 += y;

  }
}


void PixelDraw::DrawWithRectangle() {}












