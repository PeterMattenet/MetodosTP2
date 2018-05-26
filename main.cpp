#include "ImagePredictorMain.h"
#include <fstream>
#include <vector>
#include <random>
#include <string>

using namespace std;

void levantarArchivos(string filesPath, vector<string>& paths, vector<int>& labels){
	ifstream inFile;
	inFile.open(filesPath);
	string filePath;
	string previousFilePath = "";
	inFile >> filePath;
	int tag = 0;
	inFile >> tag;
	
	
	while (filePath != ","){
		paths.push_back(filePath.substr(0, filePath.size()-1));

		labels.push_back(tag);
		previousFilePath = filePath;
		inFile >> filePath;
		inFile >> filePath;
		inFile >> tag;

	}
	cout << "levante todo" << endl;
	inFile.close();

}



int main(int argc, char *args[]){
	int methodId = atoi(args[2]);
	cout << methodId << endl;
	string entrenamientoPath = args[4];
	cout << entrenamientoPath << endl;
	string testPath = args[6];
	cout << testPath << endl;
	string outputPath = args[8];
	cout << outputPath << endl;

	vector<string> entrenamiento;
	vector<int> etiquetasEntrenamiento;
	vector<string> test;
	vector<int> etiquetasTest;
	fstream sal(outputPath, ios::out);

	if (methodId == 0){
		ImagePredictor predictor = ImagePredictor();
		levantarArchivos(entrenamientoPath, entrenamiento, etiquetasEntrenamiento);
		levantarArchivos(testPath, test, etiquetasTest);
		predictor.loadImagesFromFileDataSet(entrenamiento, etiquetasEntrenamiento);

		
		for (int i = 0; i < test.size(); ++i)
		{
			int resultado = predictor.clasificarImagen(test[i], 1);
			sal << test[i] << ","<< resultado << endl;
		}
		

	}

	if (methodId == 1){
		ImagePredictor predictor = ImagePredictor();
		levantarArchivos(entrenamientoPath, entrenamiento, etiquetasEntrenamiento);
		levantarArchivos(testPath, test, etiquetasTest);


		predictor.loadImagesFromFileDataSet(entrenamiento, etiquetasEntrenamiento);
		
		predictor.generateBasisChangeMatrixWithSVD(40, 1000);

		for (int i = 0; i < test.size(); ++i)
		{
			int resultado = predictor.clasificarImagen(test[i], 1);
			sal << test[i] << ","<< resultado << endl;
		}
		

	}

	if (methodId == 2){
		ImagePredictor predictor = ImagePredictor();
		levantarArchivos(entrenamientoPath, entrenamiento, etiquetasEntrenamiento);
		levantarArchivos(testPath, test, etiquetasTest);
		predictor.loadImagesFromFileDataSet(entrenamiento, etiquetasEntrenamiento);
		predictor.generateBasisChangeMatrixWithSVD(40, 1000);

		for (int i = 0; i < test.size(); ++i)
		{
			int resultado = predictor.clasificarImagenPuntos(test[i], 1);
			sal << test[i] << ","<< resultado << endl;
		}
		

	}

	sal.close();
	
}















