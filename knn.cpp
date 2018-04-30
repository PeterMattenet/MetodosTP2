#include <vector>
#include <algorithm>
#include "Image.cpp"

using namespace std;

class knnTable
{
public:
	knnTable(vector<string> imageFileDataSet, vector<int>& ids ){

		loadImagesFromFileDataSet(imageFileDataSet);

		photoIds = ids;
		createImageMatrix();
	};
	~knnTable(){
		for (int i = 0; i < imageDataSet.size(); ++i)
		{
			delete imageMatrix[i];
		}
	};

	string obtainKSimilarPicture(string fileName){
		Image img = Image(fileName);
		
		unsigned int imageLength = img.width * img.height;

		uchar* aplanatedImageArray = new uchar[imageLength];
		img.aplanateImageArray(aplanatedImageArray);

		int newImageNorm2 = obtainVectorNorm(aplanatedImageArray, imageLength );

		vector<unsigned int> squaredNorm2MinusNewPicture;
		for (int i = 0; i < imageDataSet.size(); i++){
			squaredNorm2MinusNewPicture.push_back(abs(newImageNorm2 - obtainVectorNorm(imageMatrix[i], imageLength)));
		}

		std::sort(squaredNorm2MinusNewPicture.begin(), squaredNorm2MinusNewPicture.end());

		//buscar los k primeros y definir cual es la id moda, retornar el path de la imagen con dicho id

		delete aplanatedImageArray;

	}

	vector<int> photoIds;
	vector<Image*> imageDataSet; 
	vector<uchar*> imageMatrix;

private:
	int obtainVectorNorm(uchar* aplanatedImageArray, unsigned int imageLength){
		//implementar
		return 1;
	}
	void loadImagesFromFileDataSet(vector<string> imageFileDataSet){
		imageDataSet= vector<Image*>( );
  		
  		for (int i = 0; i < imageFileDataSet.size(); ++i)
  		{
  		  	Image img = Image(imageFileDataSet[i]);
    		imageDataSet.push_back(&img);
  		}
	}

	void createImageMatrix(){
		for (int i = 0; i < imageDataSet.size(); ++i)
		{
			unsigned int imageWidth = imageDataSet[i]->width;
			unsigned int imageHeight = imageDataSet[i]->height;

			uchar* aplanatedImageArray = new uchar[imageWidth * imageHeight];

		
			imageDataSet[i]->aplanateImageArray(aplanatedImageArray);
			
			imageMatrix.push_back(aplanatedImageArray);
			
		}
	}

	/* data */
};