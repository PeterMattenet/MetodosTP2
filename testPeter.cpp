#include <iostream>
#include <fstream>
#include <vector>
#include "ImagePredictor.h"
#include <random>
#include <string>
#include <sstream>
#include <chrono> 

using namespace std;

std::chrono::time_point<std::chrono::system_clock> startTime, endTime;

vector<string> levantarArchivosDesdeTestNipo(string testFilesPath){
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


//SOLO FUNCIONA CON ETIQUETAS CON 10 MUESTRAS
void RealizarFoldI(int foldI, int kFoldValue, vector<vector<string> >& imagenesSeparadas, vector<string>& pathsEntrenamiento, vector<string>& pathsTest){
	if(kFoldValue == 2){
		for(int j = 0; j < imagenesSeparadas.size(); j++){
			// recorro todos los vectores de cada categoría
			for(int l = 0; l < 5; l++){
				// en la primera iteración mandás las primeras 5 a testeo y las últimas 5 a entrenamiento
				// en la segunda iteración mandás las primeras 5 a entrenamiento y las segundas 5 a testeo
				// todo esto con respecto a cada categoría
				if(foldI == 0){
					pathsTest.push_back(imagenesSeparadas[j][l]);
					pathsEntrenamiento.push_back(imagenesSeparadas[j][l+5]);						
				}else{
					pathsTest.push_back(imagenesSeparadas[j][l+5]);
					pathsEntrenamiento.push_back(imagenesSeparadas[j][l]);							
				}
			}
		}
	}
	if(kFoldValue == 5){
		for(int j = 0; j < imagenesSeparadas.size(); j++){
			// recorro todos los vectores de cada categoría
			// en la primera iteración pongo la 0 y la 1 en testeo y el resto a entrenamiento, en la segunda iteración la 2 y la 3 y el resto a entrenamiento, ...
			// (todo esto con respecto a cada categoría)
			pathsTest.push_back(imagenesSeparadas[j][foldI*2]);
			pathsTest.push_back(imagenesSeparadas[j][foldI*2+1]);
			for(int l = 0; l < foldI*2; l++){
				pathsEntrenamiento.push_back(imagenesSeparadas[j][l]);
			}
			for(int l = foldI*2+2; l < 10; l++){
				pathsEntrenamiento.push_back(imagenesSeparadas[j][l]);
			}
		}
	}
	if(kFoldValue == 10){
		for(int j = 0; j < imagenesSeparadas.size(); j++){
			// recorro todos los vectores de cada categoría
			// en la primera iteración pongo la primera en testing y resto a entrenamiento, en la segunda iteración la segunda a testing y resto a entrenamiento, ...
			// (todo esto con respecto a cada categoría)
			pathsTest.push_back(imagenesSeparadas[j][foldI]);
			for(int l = 0; l < 10; l++){
				if(l != foldI){
					pathsEntrenamiento.push_back(imagenesSeparadas[j][l]);	
				}
			}
		}
	}
}

void DesordenarImagenes(vector<string> filePaths, vector<vector<string>>& imagenesSeparadas){
	int cant = 0;
	for(int i = 0; i < 41; i++){
		for(int j = 0; j < 10; j++){
			imagenesSeparadas[i].push_back(filePaths[cant]);
			cant++;
		}
		// me armé el vector i con sus 10 imágenes, ahora las shuffleo
		unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();
		shuffle(imagenesSeparadas[i].begin(), imagenesSeparadas[i].end(), std::default_random_engine(seed));
	}
}

int obtainIntTag(string pathTag){
	int n = pathTag.size() - 1 ;
	int indiceUltimBarra = n;
	for( ; indiceUltimBarra >= 0 ; indiceUltimBarra --){
		if(pathTag[indiceUltimBarra] == 's'){
			break;
		}
	}
	return std::stoi(pathTag.substr(indiceUltimBarra+1,n - indiceUltimBarra));
}

void LlenarVectorDeConfusion(vector<pair<int,int>>& paresEsperadosObtenidos, vector<string>& pathsEntrenamiento, vector<string>& pathsTest, int alfa, int knn){
	ImagePredictor predictor = ImagePredictor();
	predictor.loadImagesFromFileDataSet(pathsEntrenamiento);
	predictor.generateBasisChangeMatrixWithSVD(alfa, 200);

	// hago las predicciones de todas las imagenes de testeo y guardo resultados con el valor de k
	// y labels verdaderas en dos vectores para luego comparar
	

	for (int i = 0; i < pathsTest.size(); ++i){
		string imageObtained = predictor.clasificarImagen(pathsTest[i], knn);
		paresEsperadosObtenidos.push_back(pair<int,int>(obtainIntTag(obtainPathUntilLastFolder(pathsTest[i])),obtainIntTag(imageObtained)));
		cout << paresEsperadosObtenidos[i].first << " " << paresEsperadosObtenidos[i].second << endl;
	}
	
}

void CalcularMatrizConfusion(int kFoldValue, int alfa, int knn){
	vector<string> filePaths = levantarArchivosDesdeTestNipo("tests/testFullBig.in");
	string nombreFile;
	stringstream ss;
	ss << "alfa" << alfa << "knn" << knn;
	string fileSuffix = ss.str();
	if(kFoldValue == 2){
		nombreFile = "testPeterDir/matrizConfusionBrutosK2" + fileSuffix;
	}
	if(kFoldValue == 5){
		nombreFile = "testPeterDir/matrizConfusionBrutosK5" + fileSuffix;	
	}
	if(kFoldValue == 10){
		nombreFile = "testPeterDir/matrizConfusionBrutosK10"+ fileSuffix;
	}
	cout << nombreFile << endl;
	fstream sal(nombreFile, ios::out);

	sal << "etiquetaObtenida,etiquetaEsperada,cantidad" << endl;

	cout << "Comienza con K = ";
	cout << kFoldValue << endl;
	cout << endl; 

	vector<vector<string> > imagenesSeparadas(41, vector<string>(0, ""));
	
	DesordenarImagenes(filePaths, imagenesSeparadas);

	vector<pair<int,int>> vectorDeConfusion;
	for (int i = 0; i < kFoldValue; ++i)
	{	
		cout << "Fold numero: " << i << endl;
		vector<string> pathsEntrenamiento;
		vector<string> pathsTest;
		RealizarFoldI(i, kFoldValue, imagenesSeparadas, pathsEntrenamiento, pathsTest);
		LlenarVectorDeConfusion(vectorDeConfusion,pathsEntrenamiento, pathsTest, alfa, knn);
	}


	vector<vector<int>> matrizDeConfusion(41, vector<int>(41, 0));
	for (int i = 0; i < vectorDeConfusion.size(); ++i)
	{
		matrizDeConfusion[vectorDeConfusion[i].first-1][vectorDeConfusion[i].second -1]++;
	}

	for (int i = 0; i < matrizDeConfusion.size(); ++i)
	{
		for (int j = 0; j < matrizDeConfusion.size(); ++j)
		{
			sal << i+1 << "," << j+1 << "," << matrizDeConfusion[i][j] << endl;
		}
	}
	sal.close();


}

double testF1ScorePromediado(vector<string> entrenamientoPath, vector<string> testeoPath, vector<string> etiquetas){
	ImagePredictor predictor = ImagePredictor();
	predictor.loadImagesFromFileDataSet(entrenamientoPath);

	vector<string> results;
	predictor.generateBasisChangeMatrixWithSVD(15, 100);
	vector<string> labelsTesteo;
	
	for (int i = 0; i < testeoPath.size(); ++i)
	{
		labelsTesteo.push_back(obtainPathUntilLastFolder(testeoPath[i]));
		string imageObtained = predictor.clasificarImagen(testeoPath[i], 20);
		results.push_back(imageObtained);
	}
	
	double precisionPromediada = 0.0;
	double recallPromediada = 0.0;
	for (int i = 0; i < etiquetas.size(); ++i)
	{
		int tpositivos = 0;
		int tnegativos = 0;
		int fpositivos = 0;
		int fnegativos = 0;
		for (int j = 0; j < results.size(); ++j)
		{
			if (etiquetas[i] == results[j] && labelsTesteo[j] == etiquetas[i]){
				tpositivos++;
			}
			else if (etiquetas[i] != results[j] && labelsTesteo[j] != etiquetas[i]){
				tnegativos++;
			}
			else if(etiquetas[i] != results[j] && labelsTesteo[j] == etiquetas[i]){
				fnegativos++;
			}
			else if (etiquetas[i] == results[j] && labelsTesteo[j] != etiquetas[i]){
				fpositivos++;
			}
		}

		precisionPromediada += ((double) tpositivos / (tpositivos + fpositivos));
		recallPromediada += ((double) tpositivos / (tpositivos + fnegativos));
	}

	precisionPromediada = precisionPromediada / results.size();
	recallPromediada = recallPromediada / results.size();

	cout << "F1 score del algoritmo: " << endl;
	double f1Score =  2 * precisionPromediada * recallPromediada / (precisionPromediada + recallPromediada);
	cout << f1Score  << endl;
	return f1Score;
}

void testF1ScoreSegunAlfaYk(int kFoldValue){
	vector<string> filePaths = levantarArchivosDesdeTestNipo("tests/testFullBig.in");
	string nombreFile;
	// stringstream ss;
	// ss << "alfa" << alfa << "knn" << knn;
	// string fileSuffix = ss.str();
	if(kFoldValue == 2){
		nombreFile = "testPeterDir/f1ScoreAlfakK2";
	}
	if(kFoldValue == 5){
		nombreFile = "testPeterDir/f1ScoreAlfakK5";	
	}
	if(kFoldValue == 10){
		nombreFile = "testPeterDir/f1ScoreAlfakK10";
	}
	cout << nombreFile << endl;
	fstream sal(nombreFile, ios::out);

	sal << "etiquetaObtenida,etiquetaEsperada,cantidad" << endl;

	cout << "Comienza con K = ";
	cout << kFoldValue << endl;
	cout << endl; 

	vector<vector<string> > imagenesSeparadas(41, vector<string>(0, ""));
	
	DesordenarImagenes(filePaths, imagenesSeparadas);

	vector<pair<int,int>> vectorDeConfusion;
	for (int i = 0; i < kFoldValue; ++i)
	{	
		cout << "Fold numero: " << i << endl;
		vector<string> pathsEntrenamiento;
		vector<string> pathsTest;
		RealizarFoldI(i, kFoldValue, imagenesSeparadas, pathsEntrenamiento, pathsTest);
		LlenarVectorDeConfusion(vectorDeConfusion,pathsEntrenamiento, pathsTest, alfa, knn);
	}


	vector<vector<int>> matrizDeConfusion(41, vector<int>(41, 0));
	for (int i = 0; i < vectorDeConfusion.size(); ++i)
	{
		matrizDeConfusion[vectorDeConfusion[i].first-1][vectorDeConfusion[i].second -1]++;
	}

	for (int i = 0; i < matrizDeConfusion.size(); ++i)
	{
		for (int j = 0; j < matrizDeConfusion.size(); ++j)
		{
			sal << i+1 << "," << j+1 << "," << matrizDeConfusion[i][j] << endl;
		}
	}
	sal.close();


}



int main(){
	// testKnnSinPcaReduced(2);
	// testKnnSinPcaReduced(5);
	// testKnnSinPcaReduced(10);
	// testKnnSinPca(5);
	//CalcularMatrizConfusion(10, 65, 5);
	//CalcularMatrizConfusion(10, 70, 10);
	//CalcularMatrizConfusion(10, 80, 20);
	testF1ScoreSegunAlfaYk(10);
	return 0;
}