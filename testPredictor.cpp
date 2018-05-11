#include <iostream>
#include <fstream>
//#include <chrono>
#include <vector>
#include "ImagePredictor.h"

using namespace std;


vector<string> levantarArchivosDesdeTest(string testFilesPath){
	ifstream inFile;
	inFile.open(testFilesPath);
	string filePath;
	string previousFilePath = "";
	inFile >> filePath;
	string tag;
	inFile >> tag;

	vector<string> filePaths;

		
	while (filePath != previousFilePath){
		filePaths.push_back(filePath.substr(0, filePath.size()-1));
		previousFilePath = filePath;
		inFile >> filePath;
		inFile >> tag;

	}

	inFile.close();

	return filePaths;
}
void testCargarPredictorConImagenes(string testFilesPath){

	vector<string> filePaths = levantarArchivosDesdeTest(testFilesPath);
	ImagePredictor predictor = ImagePredictor();
	predictor.loadImagesFromFileDataSet(filePaths);

	cout << "Reviso el tamaño de la matriz de pixeles" <<endl;
	cout << predictor.basicImagePixelMatrix.size() << endl;
	cout << predictor.basicImagePixelMatrix[0].size() << endl;

	
}



void testPredecirImagen(string testFilesPath, string predictFile){
	vector<string> filePaths = levantarArchivosDesdeTest(testFilesPath);
	ImagePredictor predictor = ImagePredictor();
	predictor.loadImagesFromFileDataSet(filePaths);
	
	string imageObtained = predictor.clasificarImagen(predictFile, 10, 10, 30);
	cout << "Imagen mas parecida" << endl;
	cout << imageObtained << endl;
}
int main(){
	testCargarPredictorConImagenes("tests/testBig.in");
	testPredecirImagen("tests/testBig.in", "ImagenesCaras/s41/5.pgm");
}