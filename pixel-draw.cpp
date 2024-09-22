#include "pixel-draw.h"
#include "imgui/imgui.h"


bool operator==(Color lhs, Color rhs){
  return lhs.r == rhs.r && lhs.g == rhs.g && lhs.b == rhs.b && lhs.a == rhs.a;
}

bool PixelDraw::IsOutsideOfScreen(int x, int y){
  return x < 0 || y < 0 || x >= m_canvasWidth || y >= m_canvasHeight; 
}

PixelDraw::PixelDraw(int screenWidth, int screenHeight, Color tmpCanvasPixels[], Color mainCanvasPixels[]){
  this->m_canvasWidth = screenWidth;
  this->m_canvasHeight = screenHeight;
  this->m_pixelsSize = screenWidth * screenHeight;
  this->m_tmpCanvasPixels = tmpCanvasPixels;
  this->m_mainCanvasPixels = mainCanvasPixels;
}

void PixelDraw::DrawAndStretchCircle(int x0, int y0, int x1, int y1, Color color, bool spawnMultipleInstances){
  int midX = abs(x1+x0)/2;
  int midY = abs(y1+y0)/2;
  if(!spawnMultipleInstances){
    PixelDraw::ResetTMPBuffer();
  }
  // size as distance
  int size = sqrt(pow(x1-x0, 2) + pow(y1-y0,2));
  for(int i = size; i <= size+curToolSize; i++){
    PixelDraw::DrawCircle(midX, midY, i, color);
  }
}

void PixelDraw::DrawCenteredCircle(int centerX, int centerY, int radiusX, int radiusY, Color color, bool spawnMultipleInstances){
  
  int radius = sqrt(pow(radiusX - centerX,2) + pow(radiusY - centerY,2));
  if(!spawnMultipleInstances){
    PixelDraw::ResetTMPBuffer();
  }

  for(int i = radius; i <= radius+curToolSize; i++){
    PixelDraw::DrawCircle(centerX, centerY, i, color);
  }

}

void PixelDraw::DrawFilledCircle(int originX, int originY, int radius, Color color){
  for(int i = 1; i < radius; i++){
    PixelDraw::DrawCircle(originX, originY, i, color);
  }
}


void PixelDraw::DrawCircle(int originX, int originY, int radius, Color color){
  for(double angle = 0; angle<2*PI; angle+=0.001){
    int x0 = originX + radius*cos(angle);
    int y0 = originY + radius*sin(angle);

    if(IsOutsideOfScreen(x0, y0)){
      continue;
    }

    m_tmpCanvasPixels[x0 + y0 * m_canvasWidth] = color;  
  }
}

void PixelDraw::ClearPixels() {
  for (int i = 0; i < m_canvasWidth; i++) {
    for(int j = 0; j < m_canvasHeight; j++){
        m_mainCanvasPixels[i+j*m_canvasWidth] = WHITE;
        m_tmpCanvasPixels[i+j*m_canvasWidth] = WHITE;
    }
  }
  for(int i = 0; i < m_canvasWidth; i+=2){
    for(int j = 0; j < m_canvasHeight; j+=2){
      m_mainCanvasPixels[i+j*m_canvasWidth] = GRAY;
      m_tmpCanvasPixels[i+j*m_canvasWidth] = GRAY;
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
    if(!IsOutsideOfScreen(coords.first, coords.second) && !isVis[coords.first][coords.second] 
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
        if (!IsOutsideOfScreen(i, j)){
          m_tmpCanvasPixels[i + j * m_canvasWidth] = color;
        }
      }
    }
}


void PixelDraw::DrawWithBrush(int originX, int originY, Color colorToDraw) {

  if(curBrushShape == BrushShape::SquareBrush){
    DrawFilledSquare(originX, originY, curToolSize, colorToDraw);
  }
  else{
    DrawCircle(originX, originY, curToolSize, colorToDraw);
  }

}

void PixelDraw::DrawWithLine(float x0, float y0, float x1, float y1) {


  // checking screen boundaries to prevent segfault
  if(IsOutsideOfScreen(x0, y0) || IsOutsideOfScreen(x1, y1)){
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
        if(!IsOutsideOfScreen(width, height)){
          m_tmpCanvasPixels[(int)width + (int)height * m_canvasWidth] = curDrawingColor;
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


void PixelDraw::DrawWithRectangle() {}












