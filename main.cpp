#include <cstddef>
#include <iostream>
#include "ppmloader/ppmloader.cpp"
#include "Image.cpp"
//#include "knn.cpp"
#include <vector>

using namespace std;

void testLoadAndSave(){
  
  Image testImage = Image("ppmloader/buda.0.ppm");
  testImage.SaveImage("ppmloader/buda.1.ppm");
}

// string testkNN(vector<string> dataSet, string faceSerached){
//   vector<Image> imageDataSet = vector<Image>( );
//   for (int i = 0; i < dataSet.size(); ++i)
//   {
//     Image img = Image(dataSet[i]);
//     imageDataSet.push_back(img);
//   }

//   knnTable table = kknTable(imageDataSet);

//   table.Find(faceSerached);

//   return faceSerached;
// }


int main() {

  testLoadAndSave();
  
  //vector<string> imageDataSet(1,"ppmloader/buda.0.ppm");
  
  //string mostKSimilar = testkNN(imageDataSet, "ppmloader/buda.0.ppm");
  //cout << mostKSimilar << endl;
 
  return 0;
}