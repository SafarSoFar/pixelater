
#include <raylib.h>
struct Layer{
  char name[32];
  bool isSelected = false;
  bool isActive = true;

  // () ensures that each index has default zero value 
  Color *mainLayerPixels = new Color[g_canvasPixelsSize]();
  Color *tmpLayerPixels = new Color[g_canvasPixelsSize]();
  vector<vector<Color>> undoLayerColorPixels;
  vector<vector<Color>> redoLayerColorPixels;
};
