#include <cstddef>
#include <iostream>

#include "knn.cpp"
#include <vector>

using namespace std;

// void testLoadAndSave(){
  
//   Image testImage = Image("ppmloader/buda.0.ppm");
//   testImage.SaveImage("ppmloader/buda.1.ppm");
// }


int main() {

  //testLoadAndSave();
  
  vector<string> imageDataSet(1,"ppmloader/buda.0.ppm");
  vector<int> ids(1, 5);
  
  knnTable table = knnTable(imageDataSet, ids);

 
  return 0;
}