#include <iostream>
#include <fstream>
#include <vector>
#include "ImagePredictor.h"
#include <random>
#include <chrono> 

using namespace std;

std::chrono::time_point<std::chrono::system_clock> startTime, endTime;
std::chrono::duration<double, std::milli> elapsed_sum;

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

// tiene que tener como entrada un vector<string> donde cada string es un path (donde un path es ImagenesCaras/s1/9.pgm por ejemplo)
// dado un conjunto de imágenes de testing y otras de entrenamiento, corre el clasificador
// para cada una de las de testing y devuelve accuracy promediada
// recibe como parámetro también el k de knn
double accuracyPromediadaSinPca(vector<string>& entrenamientoFilesPath, vector<string>& testeoFilesPath, int k){
	
	// creo predictor y cargo imágenes de entrenamiento
	ImagePredictor predictor = ImagePredictor();
	predictor.loadImagesFromFileDataSet(entrenamientoFilesPath);

	// hago las predicciones de todas las imagenes de testeo y guardo resultados con el valor de k
	// y labels verdaderas en dos vectores para luego comparar
	vector<string> results;
	vector<string> labelsTesteo;
	for (int i = 0; i < testeoFilesPath.size(); ++i){
		labelsTesteo.push_back(obtainPathUntilLastFolder(testeoFilesPath[i]));
		startTime = std::chrono::system_clock::now();
		string imageObtained = predictor.clasificarImagenSinPca(testeoFilesPath[i], k);
		endTime = std::chrono::system_clock::now();
		elapsed_sum += (endTime - startTime);
		results.push_back(imageObtained);
	}
	
	// calculo la accuracy y la promedio
	double accurracyPromediada = 0.0;
	for (int i = 0; i < results.size(); ++i)
	{
		if (labelsTesteo[i] == results[i]){
			accurracyPromediada++;
		}
	}

	accurracyPromediada = accurracyPromediada / results.size();

	return accurracyPromediada;
}

// corre testFullBig.in y fijo k de kFold en 5 y 10
void testKnnSinPcaReduced(int kFoldValue){
	vector<string> filePaths = levantarArchivosDesdeTestNipo("tests/testFullRed.in");
	string nombreFile;
	if(kFoldValue == 2){
		nombreFile = "testNipo/asf1.csv";
	}
	if(kFoldValue == 5){
		nombreFile = "testNipo/asf2.csv";	
	}
	if(kFoldValue == 10){
		nombreFile = "testNipo/asf3.csv";
	}
	
	fstream sal(nombreFile, ios::out);

	sal << "k,accuracyProm,Tiempo" << endl;

	cout << "Comienza con K = ";
	cout << kFoldValue << endl;
	cout << endl; 

	// pongo a las imágenes en 41 vectores distintos según su categoría (en realidad es un vector<vector>>)
	// cada uno de los vectores tendrá sus 10 imágenes shuffleadas
	vector<vector<string> > imagenesSeparadas(41, vector<string>(0, ""));
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

	int sizeTesting = 0;

	for(int kValue = 1; kValue < 100; kValue++){
		double accur = 0.0;
		int iterNumber = 0;
		cout << "voy por k = " << kValue << endl;
		for(int i = 0; i < kFoldValue; i++){
			vector<string> entrenamiento;
			vector<string> testeo;
			if(kFoldValue == 2){
				for(int j = 0; j < imagenesSeparadas.size(); j++){
					// recorro todos los vectores de cada categoría
					for(int l = 0; l < 5; l++){
						// en la primera iteración mandás las primeras 5 a testeo y las últimas 5 a entrenamiento
						// en la segunda iteración mandás las primeras 5 a entrenamiento y las segundas 5 a testeo
						// todo esto con respecto a cada categoría
						if(i == 0){
							testeo.push_back(imagenesSeparadas[j][l]);
							entrenamiento.push_back(imagenesSeparadas[j][l+5]);						
						}else{
							testeo.push_back(imagenesSeparadas[j][l+5]);
							entrenamiento.push_back(imagenesSeparadas[j][l]);							
						}
					}
				}
			}
			if(kFoldValue == 5){
				for(int j = 0; j < imagenesSeparadas.size(); j++){
					// recorro todos los vectores de cada categoría
					// en la primera iteración pongo la 0 y la 1 en testeo y el resto a entrenamiento, en la segunda iteración la 2 y la 3 y el resto a entrenamiento, ...
					// (todo esto con respecto a cada categoría)
					testeo.push_back(imagenesSeparadas[j][iterNumber]);
					testeo.push_back(imagenesSeparadas[j][iterNumber+1]);
					for(int l = 0; l < iterNumber; l++){
						entrenamiento.push_back(imagenesSeparadas[j][l]);
					}
					for(int l = iterNumber+2; l < 10; l++){
						entrenamiento.push_back(imagenesSeparadas[j][l]);
					}
				}
				iterNumber = iterNumber + 2;
			}
			if(kFoldValue == 10){
				for(int j = 0; j < imagenesSeparadas.size(); j++){
					// recorro todos los vectores de cada categoría
					// en la primera iteración pongo la primera en testing y resto a entrenamiento, en la segunda iteración la segunda a testing y resto a entrenamiento, ...
					// (todo esto con respecto a cada categoría)
					testeo.push_back(imagenesSeparadas[j][i]);
					for(int l = 0; l < 10; l++){
						if(l != i){
							entrenamiento.push_back(imagenesSeparadas[j][l]);	
						}
					}
				}
			}
			sizeTesting = testeo.size();
			accur = accur + accuracyPromediadaSinPca(entrenamiento, testeo, kValue);
		}
		accur = accur / kFoldValue;
		// en este punto ya tengo la accuracy promediada de kNN sin PCA con el kValue actual
		sal << kValue;
		sal << ",";
		sal << accur;
		sal << ",";

		std::chrono::duration<double, std::milli> elapsed_seconds = (elapsed_sum / (sizeTesting * kFoldValue));
		sal << elapsed_seconds.count() << endl;
		elapsed_sum = std::chrono::duration<double, std::milli>::zero();
	}
	sal.close();
}

void testKnnSinPca(int kFoldValue){
	vector<string> filePaths = levantarArchivosDesdeTestNipo("tests/testFullBig.in");
	string nombreFile;
	if(kFoldValue == 2){
		nombreFile = "testNipo/asd1.csv";
	}
	if(kFoldValue == 5){
		nombreFile = "testNipo/asd2.csv";	
	}
	if(kFoldValue == 10){
		nombreFile = "testNipo/asd3.csv";
	}
	
	fstream sal(nombreFile, ios::out);

	sal << "k,accuracyProm,Tiempo" << endl;

	cout << "Comienza con K = ";
	cout << kFoldValue << endl;
	cout << endl; 

	// pongo a las imágenes en 41 vectores distintos según su categoría (en realidad es un vector<vector>>)
	// cada uno de los vectores tendrá sus 10 imágenes shuffleadas
	vector<vector<string> > imagenesSeparadas(41, vector<string>(0, ""));
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

	int sizeTesting = 0;

	for(int kValue = 1; kValue < 100; kValue++){
		double accur = 0.0;
		int iterNumber = 0;
		cout << "voy por k = " << kValue << endl;
		for(int i = 0; i < kFoldValue; i++){
			vector<string> entrenamiento;
			vector<string> testeo;
			if(kFoldValue == 2){
				for(int j = 0; j < imagenesSeparadas.size(); j++){
					// recorro todos los vectores de cada categoría
					for(int l = 0; l < 5; l++){
						// en la primera iteración mandás las primeras 5 a testeo y las últimas 5 a entrenamiento
						// en la segunda iteración mandás las primeras 5 a entrenamiento y las segundas 5 a testeo
						// todo esto con respecto a cada categoría
						if(i == 0){
							testeo.push_back(imagenesSeparadas[j][l]);
							entrenamiento.push_back(imagenesSeparadas[j][l+5]);						
						}else{
							testeo.push_back(imagenesSeparadas[j][l+5]);
							entrenamiento.push_back(imagenesSeparadas[j][l]);							
						}
					}
				}
			}
			if(kFoldValue == 5){
				for(int j = 0; j < imagenesSeparadas.size(); j++){
					// recorro todos los vectores de cada categoría
					// en la primera iteración pongo la 0 y la 1 en testeo y el resto a entrenamiento, en la segunda iteración la 2 y la 3 y el resto a entrenamiento, ...
					// (todo esto con respecto a cada categoría)
					testeo.push_back(imagenesSeparadas[j][iterNumber]);
					testeo.push_back(imagenesSeparadas[j][iterNumber+1]);
					for(int l = 0; l < iterNumber; l++){
						entrenamiento.push_back(imagenesSeparadas[j][l]);
					}
					for(int l = iterNumber+2; l < 10; l++){
						entrenamiento.push_back(imagenesSeparadas[j][l]);
					}
				}
				iterNumber = iterNumber + 2;
			}
			if(kFoldValue == 10){
				for(int j = 0; j < imagenesSeparadas.size(); j++){
					// recorro todos los vectores de cada categoría
					// en la primera iteración pongo la primera en testing y resto a entrenamiento, en la segunda iteración la segunda a testing y resto a entrenamiento, ...
					// (todo esto con respecto a cada categoría)
					testeo.push_back(imagenesSeparadas[j][i]);
					for(int l = 0; l < 10; l++){
						if(l != i){
							entrenamiento.push_back(imagenesSeparadas[j][l]);	
						}
					}
				}
			}
			sizeTesting = testeo.size();
			accur = accur + accuracyPromediadaSinPca(entrenamiento, testeo, kValue);
		}
		accur = accur / kFoldValue;
		// en este punto ya tengo la accuracy promediada de kNN sin PCA con el kValue actual
		sal << kValue;
		sal << ",";
		sal << accur;
		sal << ",";

		std::chrono::duration<double, std::milli> elapsed_seconds = (elapsed_sum / (sizeTesting * kFoldValue));
		sal << elapsed_seconds.count() << endl;
		elapsed_sum = std::chrono::duration<double, std::milli>::zero();

	}
	sal.close();
}





int main(){
	// testKnnSinPcaReduced(2);
	// testKnnSinPcaReduced(5);
	// testKnnSinPcaReduced(10);
	testKnnSinPca(5);
	return 0;
}