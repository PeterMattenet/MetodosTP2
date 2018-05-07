
#include <iostream>
#include <cstddef>

#include "Image.h"

Image::Image(std::string newFilePath){

	width = 0, height = 0;
  	PPM_LOADER_PIXEL_TYPE pt = PPM_LOADER_PIXEL_TYPE_INVALID;
	//  std::string filename = "buda.0.ppm";
  	filePath = newFilePath;
  	bool ret = LoadPPMFile(&imageBuffer, &width, &height, &pt, filePath.c_str());
	
};
Image::~Image(){
	
	delete [] imageBuffer;

};

void Image::aplanateImageArray(uchar* array){
	for (int i = 0; i < height; ++i)
	{
		for (int j = 0; j < width; ++j)
		{
			array[i * width + j] = obtainPixelValue(i * width + j);
		}
	}
}

uchar Image::obtainPixelValue(unsigned int pixelNumber){
	return imageBuffer[pixelNumber];
}

void Image::SaveImage(std::string newFilePath){

 	char comments[100];

  		
  	bool ret = SavePPMFile(newFilePath.c_str(),imageBuffer,width,height,PPM_LOADER_PIXEL_TYPE_RGB_8B, comments);
}
