#include <vector>
#include <algorithm>
#include "Image.cpp"
#include "matrix.h"

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

		//transformo a nuestra estructura la imagen a identificar
		Image img = Image(fileName);
		unsigned int imageLength = img.width * img.height;
		uchar* aplanatedImageArray = new uchar[imageLength];
		img.aplanateImageArray(aplanatedImageArray); // esto es necesario pues no puedo acceder a la parte privada de la imagen

		// obtengo la norma2 de la nueva imagen
		int newImageNorm2 = obtainVectorNorm(aplanatedImageArray, imageLength );

		// creo un vector donde guardaré la distancia euclídea entre los puntos que representan a las imágenes de la base y el punto que representa a la imagen a identificar
		vector<unsigned int> squaredNorm2MinusNewPicture;
		for (int i = 0; i < imageDataSet.size(); i++){
			squaredNorm2MinusNewPicture.push_back(abs(newImageNorm2 - obtainVectorNorm(imageMatrix[i], imageLength)));
		}

		// ordeno el vector para luego tomar los k más cercanos
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

	// llena el vector imageDataSet con todas las imágenes del archivo de entrada (cuyo nombre es el primer elemento de imageFileDataSet, el parámetro de entrada)
	void loadImagesFromFileDataSet(vector<string> imageFileDataSet){
		imageDataSet= vector<Image*>( );
  		
  		for (int i = 0; i < imageFileDataSet.size(); ++i)
  		{
  		  	Image img = Image(imageFileDataSet[i]);
    		imageDataSet.push_back(&img);
  		}
	}

	// llena el vector imageMatrix con todas las imágenes del imageDataSet
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