#include <iostream>
#include "ppmloader.h"

using namespace std;

typedef unsigned char uchar;


void test_load_gray(const string& filename){

  uchar* data = NULL;
  int width = 0, height = 0;
  PPM_LOADER_PIXEL_TYPE pt = PPM_LOADER_PIXEL_TYPE_INVALID;

  bool ret = LoadPPMFile(&data, &width, &height, &pt, filename.c_str());
  if (!ret || width == 0|| height == 0|| pt!=PPM_LOADER_PIXEL_TYPE_GRAY_8B)
  {
    throw runtime_error("test_load failed");
  }
  cout << width << " " << height << endl;

  delete [] data;
}


void test_orlFaces(const string& path) {

  for(int i=1; i <= 40; ++i)
    for(int j=1; j <= 10; ++j){
      string filename = path + "s"+to_string(i)+"/"+to_string(j)+".pgm";

      cout << filename << endl;

      test_load_gray(filename);
    }
}

int main() {

  string path = "/home/pachi/Descargas/ImagenesCaras/";
//  string path = "/home/pachi/Descargas/orl_faces/";

  test_orlFaces(path);

  return 0;
}
