#include <iostream>
#include <fstream>
//#include <chrono>
#include <vector>
#include "ImagePredictor.h"
#include <random>       // std::default_random_engine
#include <chrono> 

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
	
	predictor.generateBasisChangeMatrixWithSVD(15, 100);
	string imageObtained = predictor.clasificarImagen(predictFile, 20);
	cout << "Imagen mas parecida" << endl;
	cout << imageObtained << endl;
}



double testAccurracyBig(vector<string>& entrenamientoFilesPath, vector<string>& testeoFilesPath){
	cout << "Comenzando testeo" << endl;
	//vector<string> filePathsEntrenamiento = levantarArchivosDesdeTest(entrenamientoFilesPath);
	//vector<string> filePathsTesteo = levantarArchivosDesdeTest(testeoFilesPath);
	ImagePredictor predictor = ImagePredictor();
	cout << entrenamientoFilesPath.size() << " " << endl;
	cout << entrenamientoFilesPath[0] << " " << endl;
	predictor.loadImagesFromFileDataSet(entrenamientoFilesPath);
	cout << "Cargando imagenes al predictor" << endl;
	vector<string> results;
	//Por alguna razon alfa=45 garpa, probe con 10 y tambien llega a los 90% pero 45 tiene unos 5% extra
	predictor.generateBasisChangeMatrixWithSVD(8, 300);
	vector<string> labelsTesteo;
	cout << "Matriz cambio de base creada" << endl;
	for (int i = 0; i < testeoFilesPath.size(); ++i)
	{
		cout << "Prediciendo la imagen " << i << endl;
		labelsTesteo.push_back(obtainPathUntilLastFolder(testeoFilesPath[i]));
		//k optimo por el momento fue 2
		string imageObtained = predictor.clasificarImagen(testeoFilesPath[i], 2);
		results.push_back(imageObtained);
	}
	
	cout << "Calculando la accurracy" << endl;
	double accurracyPromediada = 0.0;
	for (int i = 0; i < results.size(); ++i)
	{

		if (labelsTesteo[i] == results[i]){
			accurracyPromediada++;
		}


	}

	accurracyPromediada = accurracyPromediada / results.size();

	cout << "Accurracy obtenida del algoritmo " << endl;
	cout << accurracyPromediada * 100 << "% " << endl;

	return accurracyPromediada;

}

void testAccurracyPromediadaBig(string entrenamientoFilesPath, string testeoFilesPath){
	vector<string> filePathsEntrenamiento = levantarArchivosDesdeTest(entrenamientoFilesPath);
	vector<string> filePathsTesteo = levantarArchivosDesdeTest(testeoFilesPath);
	ImagePredictor predictor = ImagePredictor();
	predictor.loadImagesFromFileDataSet(filePathsEntrenamiento);

	vector<string> results;
	predictor.generateBasisChangeMatrixWithSVD(15, 100);
	vector<string> labelsTesteo;
	
	for (int i = 0; i < filePathsTesteo.size(); ++i)
	{
		labelsTesteo.push_back(obtainPathUntilLastFolder(filePathsTesteo[i]));
		string imageObtained = predictor.clasificarImagen(filePathsTesteo[i], 20);
		results.push_back(imageObtained);
	}
	
	double accurracyPromediada = 0.0;
	for (int i = 0; i < results.size(); ++i)
	{
		int positivos = 0;
		int negativos = 0;
		for (int j = 0; j < results.size(); ++j)
		{
			if (labelsTesteo[i] == results[j] && labelsTesteo[j] == results[j]){
				positivos++;
			}
			else if (labelsTesteo[i] != results[j] && labelsTesteo[j] != labelsTesteo[i]){
				negativos++;
			}
		}

		double accurracyI = ((double)(positivos + negativos))/ results.size();
		accurracyPromediada += accurracyI;
	}

	accurracyPromediada = accurracyPromediada / results.size();

	cout << "Accurracy obtenida del algoritmo " << endl;
	cout << accurracyPromediada * 100 << "% " << endl;

}


void kFoldTestAccurracy(int k, string entrenamientoFilesPath){
	cout << "Comenzando kfold" << endl;
	vector<string> filePaths = levantarArchivosDesdeTest(entrenamientoFilesPath);

	unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();

  	shuffle (filePaths.begin(), filePaths.end(), std::default_random_engine(seed));

  	double accurracyFoldPromediada = 0.0;
  	for (int i = 0; i < k; ++i)
  	{	
  		vector<string> entrenamiento;
  		vector<string> testeo;
  		
  		for (int j = 0; j < filePaths.size(); ++j)
  		{
  			if (j % k == i){
  				testeo.push_back(filePaths[j]);
  			}else{
  				entrenamiento.push_back(filePaths[j]);
  			}
  		}
  		accurracyFoldPromediada += testAccurracyBig(entrenamiento, testeo);
  	}

  	cout << "Acurracy de k=" << k << " fold es:" <<endl;
  	cout << accurracyFoldPromediada / k * 100 << "%" << endl;
	
}


int main(){
	
	kFoldTestAccurracy(10, "tests/testFullBig.in");
	//testCargarPredictorConImagenes("tests/testBig.in");
	//testPredecirImagen("tests/testRed.in", "ImagenesCaras/s41/5.pgm");
	//testAccurracyPromediadaBig("tests/entrenamientoFullBig.in","tests/testeoFullBig.in");
	//testAccurracyBig("tests/entrenamientoFullBig.in","tests/testeoFullBig.in");
}