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

// tiene que tener como entrada un vector<string> donde cada string es un path (donde un path es ImagenesCaras/s1/9.pgm por ejemplo)
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

// kFoldValue sólo puede ser 2, 5 o 10
// el archivo de entrada debe tener sí o sí 410 imágenes: 10 de cada categoría
// el algoritmo es para reutilizar, lo debieran retocar para agregarle medidas de accuracy, o precision/recall o lo que sea
void kFoldEspecial(int kFoldValue, string entrenamientoFilesPath){
	vector<string> filePaths = levantarArchivosDesdeTest(entrenamientoFilesPath);

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

	int iterNumber = 0;
	int iterNumber2 = 0;

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
			// ACÁ IRÍA EL ALGORITMO DE MEDIR ACCURACY POR EJEMPLO, O MEDIR LO QUE SEA
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
			// ACÁ IRÍA EL ALGORITMO DE MEDIR ACCURACY POR EJEMPLO, O MEDIR LO QUE SEA
		}
		if(kFoldValue == 10){
			for(int j = 0; j < imagenesSeparadas.size(); j++){
				// recorro todos los vectores de cada categoría
				// en la primera iteración pongo la primera en testing y resto a entrenamiento, en la segunda iteración la segunda a testing y resto a entrenamiento, ...
				// (todo esto con respecto a cada categoría)
				testeo.push_back(imagenesSeparadas[j][iterNumber2]);
				for(int l = 0; l < 10; l++){
					if(l != iterNumber2){
						entrenamiento.push_back(imagenesSeparadas[j][l]);	
					}
				}
			}
			iterNumber2++;
			// ACÁ IRÍA EL ALGORITMO DE MEDIR ACCURACY POR EJEMPLO, O MEDIR LO QUE SEA
		}
	}
}

// kFoldValue sólo puede ser 2, 5 o 10
// el archivo de entrada debe tener sí o sí 410 imágenes: 10 de cada categoría
void kFoldEspecialPorSiQuierenTestear(int kFoldValue, string entrenamientoFilesPath){
	vector<string> filePaths = levantarArchivosDesdeTest(entrenamientoFilesPath);

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

	/*
	for(int i = 0; i < 41; i++){
		mostrarVectorString(imagenesSeparadas[i]);
		cout << endl;
	}
	*/
	// hasta acá todo OK

	int iterNumber = 0;
	int iterNumber2 = 0;


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
			// acá iría el algoritmo de medir accuracy por ejemplo, o medir lo que sea
			/*
			cout << "k es 2" << endl;
			mostrarVectorString(testeo);
			cout << endl;
			cout << endl;
			cout << endl;
			mostrarVectorString(entrenamiento);
			*/
			// kFoldValue == 2 está testeado y anda OK, en ambas iteraciones manda todo perfecto
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
			// acá iría el algoritmo de medir accuracy por ejemplo, o medir lo que sea
			/*
			cout << endl;
			cout << "k es 5" << endl;
			cout << iterNumber << endl;
			cout << endl;
			mostrarVectorString(testeo);
			cout << endl;
			cout << endl;
			cout << endl;
			mostrarVectorString(entrenamiento);
			*/
			// kFoldValue == 5 está testeado y anda OK, en todas las iteraciones divide perfecto
		}
		if(kFoldValue == 10){
			for(int j = 0; j < imagenesSeparadas.size(); j++){
				// recorro todos los vectores de cada categoría
				// en la primera iteración pongo la primera en testing y resto a entrenamiento, en la segunda iteración la segunda a testing y resto a entrenamiento, ...
				// (todo esto con respecto a cada categoría)
				testeo.push_back(imagenesSeparadas[j][iterNumber2]);
				for(int l = 0; l < 10; l++){
					if(l != iterNumber2){
						entrenamiento.push_back(imagenesSeparadas[j][l]);	
					}
				}
			}
			iterNumber2++;
			// acá iría el algoritmo de medir accuracy por ejemplo, o medir lo que sea
			/*
			cout << endl;
			cout << "k es 10" << endl;
			cout << iterNumber2 << endl;
			cout << endl;
			mostrarVectorString(testeo);
			cout << endl;
			cout << endl;
			cout << endl;
			mostrarVectorString(entrenamiento);
			*/
			// kFoldValue == 10 está testeado y anda OK, en todas las iteraciones divide perfecto
		}
	}
}


int main(){
	//kFoldTestAccurracy(5, "tests/testFullBig.in");
	//testCargarPredictorConImagenes("tests/testBig.in");
	//testPredecirImagen("tests/testRed.in", "ImagenesCaras/s41/5.pgm");
	//testAccurracyPromediadaBig("tests/entrenamientoFullBig.in","tests/testeoFullBig.in");
	//testAccurracyBig("tests/entrenamientoFullBig.in","tests/testeoFullBig.in");
	kFoldEspecial(5, "tests/testFullRed.in");
}