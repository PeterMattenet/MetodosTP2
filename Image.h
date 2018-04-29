#ifndef __IMAGE_H__
#define __IMAGE_H__

#include <string>

class Image
{
public:
	Image(std::string newFilePath);
	~Image();

	void SaveImage(std::string newFilePath);

	std::string filePath;
	int height;
	int width;

	uchar* imageBuffer;

	/* data */
};

#endif //__IMAGE_H__
	