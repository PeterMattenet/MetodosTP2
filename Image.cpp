
#include <iostream>
#include <cstddef>

#include "Image.h"

Image::Image(std::string newFilePath){

  	width = 0, height = 0;
  	PPM_LOADER_PIXEL_TYPE pt = PPM_LOADER_PIXEL_TYPE_INVALID;
	//  std::string filename = "buda.0.ppm";
  	std::string filename = newFilePath;
  	bool ret = LoadPPMFile(&imageBuffer, &width, &height, &pt, filename.c_str());
	
};
Image::~Image(){

	delete imageBuffer;

};

void Image::SaveImage(std::string newFilePath){

 	char comments[100];

  		
  	bool ret = SavePPMFile(newFilePath.c_str(),imageBuffer,width,height,PPM_LOADER_PIXEL_TYPE_RGB_8B, comments);
}

