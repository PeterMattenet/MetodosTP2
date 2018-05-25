#include <iostream>
#include <fstream>
#include <utility>  
#include <vector>
#include <algorithm> 
#include "ImagePredictor.h"
#include <random>       // std::default_random_engine
#include <chrono> 
#include <string>

using namespace std;
using namespace std::chrono;


#define niter 700

string directoryOfSnap = "snapImagePredictor";

vector<ImagePredictor> imagePredictors;
string baseCatedra = "tests/testFullBig.in";
string nuevaBase2 = "nuevaBase/nuevaBase.in";


void createImagePredictorWithAlpha(ImagePredictor& res, int k, int alpha){

	res.n = imagePredictors[k].n;

	res.m = imagePredictors[k].m;

	res.alfa = alpha;
	
	res.labels = imagePredictors[k].labels;

	for(int i = 0 ; i < res.labels.size(); i ++){
		Image* img = new Image();
		img->label  = res.labels[i];
		res.imageDataSet.push_back(img);
	}

	res.basicImagePixelMatrix = imagePredictors[k].basicImagePixelMatrix;

	for(int i = 0 ; i < res.m ; i ++ ){
		vector<double > v ;
		for(int j = 0; j < alpha; j++){
			v.push_back(imagePredictors[k].basisChangeMatrix[i][j]);
		}
		res.basisChangeMatrix.push_back(v);
	}

	for(int i = 0 ; i < res.n ; i ++ ){
		vector<double > v ;
		for(int j = 0; j < alpha; j++){
			v.push_back(imagePredictors[k].pcaMatrix[i][j]);
		}
		res.pcaMatrix.push_back(v);
	}
}


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







double acuracyDesImagePredictorConKnn (ImagePredictor& predictor, vector<string>& testeoFilesPath, int alpha, int knn, duration<double>& time_span){
	vector<string> results;
	vector<string> labelsTesteo;
	high_resolution_clock::time_point t1 = high_resolution_clock::now();
	high_resolution_clock::time_point t2 = high_resolution_clock::now();
	
	duration<double> ac_time = duration_cast<duration<double>>(t2 - t1); ;

	for (int i = 0; i < testeoFilesPath.size(); ++i){
		labelsTesteo.push_back(obtainPathUntilLastFolder(testeoFilesPath[i]));

		t1 = high_resolution_clock::now();
		string imageObtained = predictor.clasificarImagen(testeoFilesPath[i], knn);
		t2 = high_resolution_clock::now();
		ac_time += duration_cast<duration<double>>(t2 - t1); 				
		
		
		results.push_back(imageObtained);
	}
	time_span = ac_time / testeoFilesPath.size();

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


double acuracyDesImagePredictorConKPuntos (ImagePredictor& predictor, vector<string>& testeoFilesPath, int alpha, int knn, duration<double>& time_span){
	vector<string> results;
	vector<string> labelsTesteo;
	high_resolution_clock::time_point t1 = high_resolution_clock::now();
	high_resolution_clock::time_point t2 = high_resolution_clock::now();
	
	duration<double> ac_time = duration_cast<duration<double>>(t2 - t1); ;

	for (int i = 0; i < testeoFilesPath.size(); ++i){
		labelsTesteo.push_back(obtainPathUntilLastFolder(testeoFilesPath[i]));

		t1 = high_resolution_clock::now();
		string imageObtained = predictor.clasificarImagenPuntos(testeoFilesPath[i], knn);
		t2 = high_resolution_clock::now();
		ac_time += duration_cast<duration<double>>(t2 - t1); 				
		
		
		results.push_back(imageObtained);
	}
	time_span = ac_time / testeoFilesPath.size();

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

void generateTwoTraningAndTestingSets(string entrenamientoFilesPath , vector<vector<string> >& entrenamientos, std::vector<vector<string> >& testeos, int alpha){
	int pliegos = 2;
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

	

	for(int i = 0; i < 2; i++){
		vector<string> entrenamiento;
		vector<string> testeo;
	
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
		entrenamientos.push_back(entrenamiento);
		testeos.push_back(testeo);
		
		imagePredictors.push_back(ImagePredictor());
		imagePredictors[i].loadImagesFromFileDataSet(entrenamiento);
		imagePredictors[i].generateBasisChangeMatrixWithSVD(alpha, niter);

	}
}

void generateFiveTraningAndTestingSets(string entrenamientoFilesPath , vector<vector<string> >& entrenamientos, std::vector<vector<string> >& testeos, int alpha){
	int pliegos = 5;
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

	

	for(int i = 0; i < pliegos; i++){
		vector<string> entrenamiento;
		vector<string> testeo;

		//genero los entrenamientos y test
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

		//modifico las variables de input
		entrenamientos.push_back(entrenamiento);
		testeos.push_back(testeo);

		//creo el image predictor y lo guardo
		imagePredictors.push_back(ImagePredictor());
		imagePredictors[i].loadImagesFromFileDataSet(entrenamiento);
		imagePredictors[i].generateBasisChangeMatrixWithSVD(alpha, niter);
	}
}



void combinacionAlphaKBaseCatedra2PliegosConKnn(){
	
	vector<string> filePaths = levantarArchivosDesdeTest(baseCatedra);

	high_resolution_clock::time_point t1, t2;
	duration<double> time_span ;

 	int minAlpha = 5;
 	int maxAlpha = 100;

 	// K de Knn
 	int minK = 5;
 	int maxK = 100;

 	int pliegos = 2;

 	std::vector<vector<string> > entrenamientos;
 	std::vector<vector<string> > testeos;

 	if(pliegos == 2){
	 	generateTwoTraningAndTestingSets(baseCatedra, entrenamientos, testeos, maxAlpha);
	}else if(pliegos == 5){
		generateFiveTraningAndTestingSets(baseCatedra, entrenamientos, testeos, maxAlpha);
	}
 	cout << "k" << "," << "alpha" << "," << "accuracy" << "," << "time" <<endl;

 	for(int k  = 1; k <= 1; k += 5){
 		for(int alpha = minAlpha; alpha <= maxAlpha; alpha += 5){
 			double accurracyPromediada = 0.0;
 			vector<double> acuracyPliegos ; 
 			for(int i = 0 ; i < pliegos; i ++){
 				ImagePredictor imagePredictor;
 				createImagePredictorWithAlpha(imagePredictor, i, alpha);
 				
 				double accuracy;

 				accuracy = acuracyDesImagePredictorConKnn(imagePredictor,testeos[i],alpha, k, time_span);

 				
 				acuracyPliegos.push_back(accuracy);
 				accurracyPromediada += acuracyPliegos[i];
 			}

 			
 			
 			accurracyPromediada = accurracyPromediada / pliegos;
 			cout << k << "," << alpha << "," << accurracyPromediada * 100 << "," <<time_span.count() << endl;
 		}
 	}


 	for(int k  = minK; k <= maxK; k += 5){
 		for(int alpha = minAlpha; alpha <= maxAlpha; alpha += 5){
 			double accurracyPromediada = 0.0;
 			vector<double> acuracyPliegos ; 
 			for(int i = 0 ; i < pliegos; i ++){
 				ImagePredictor imagePredictor;
 				createImagePredictorWithAlpha(imagePredictor, i, alpha);
 				
 				double accuracy;

 				accuracy = acuracyDesImagePredictorConKnn(imagePredictor,testeos[i],alpha, k, time_span);
 				
 				acuracyPliegos.push_back(accuracy);
 				accurracyPromediada += acuracyPliegos[i];
 			}
 			
 			accurracyPromediada = accurracyPromediada / pliegos;
 			cout << k << "," << alpha << "," << accurracyPromediada * 100 << "," <<time_span.count() << endl;
 		}
 	}
}



void combinacionAlphaKBaseCatedra2PliegosConKPuntos(){
	
	vector<string> filePaths = levantarArchivosDesdeTest(baseCatedra);

	high_resolution_clock::time_point t1, t2;
	duration<double> time_span ;

 	int minAlpha = 5;
 	int maxAlpha = 100;

 	// K de Knn
 	int minK = 5;
 	int maxK = 100;

 	int pliegos = 2;

 	std::vector<vector<string> > entrenamientos;
 	std::vector<vector<string> > testeos;

 	if(pliegos == 2){
	 	generateTwoTraningAndTestingSets(baseCatedra, entrenamientos, testeos, maxAlpha);
	}else if(pliegos == 5){
		generateFiveTraningAndTestingSets(baseCatedra, entrenamientos, testeos, maxAlpha);
	}
 	cout << "k" << "," << "alpha" << "," << "accuracy" << "," << "time" <<endl;

 	for(int k  = 1; k <= 1; k += 5){
 		for(int alpha = minAlpha; alpha <= maxAlpha; alpha += 5){
 			double accurracyPromediada = 0.0;
 			vector<double> acuracyPliegos ; 
 			for(int i = 0 ; i < pliegos; i ++){
 				ImagePredictor imagePredictor;
 				createImagePredictorWithAlpha(imagePredictor, i, alpha);
 				
 				double accuracy;

 				accuracy = acuracyDesImagePredictorConKPuntos(imagePredictor,testeos[i],alpha, k, time_span);

 				
 				acuracyPliegos.push_back(accuracy);
 				accurracyPromediada += acuracyPliegos[i];
 			}

 			
 			
 			accurracyPromediada = accurracyPromediada / pliegos;
 			cout << k << "," << alpha << "," << accurracyPromediada * 100 << "," <<time_span.count() << endl;
 		}
 	}


 	for(int k  = minK; k <= maxK; k += 5){
 		for(int alpha = minAlpha; alpha <= maxAlpha; alpha += 5){
 			double accurracyPromediada = 0.0;
 			vector<double> acuracyPliegos ; 
 			for(int i = 0 ; i < pliegos; i ++){
 				ImagePredictor imagePredictor;
 				createImagePredictorWithAlpha(imagePredictor, i, alpha);
 				
 				double accuracy;

 				accuracy = acuracyDesImagePredictorConKPuntos(imagePredictor,testeos[i],alpha, k, time_span);
 				
 				acuracyPliegos.push_back(accuracy);
 				accurracyPromediada += acuracyPliegos[i];
 			}
 			
 			accurracyPromediada = accurracyPromediada / pliegos;
 			cout << k << "," << alpha << "," << accurracyPromediada * 100 << "," <<time_span.count() << endl;
 		}
 	}
}


void combinacionAlphaKBaseCatedra5PliegosConKnn(){
	
	vector<string> filePaths = levantarArchivosDesdeTest(baseCatedra);

	high_resolution_clock::time_point t1, t2;
	duration<double> time_span ;

 	int minAlpha = 5;
 	int maxAlpha = 100;

 	// K de Knn
 	int minK = 5;
 	int maxK = 100;

 	int pliegos = 5;

 	std::vector<vector<string> > entrenamientos;
 	std::vector<vector<string> > testeos;

 	if(pliegos == 2){
	 	generateTwoTraningAndTestingSets(baseCatedra, entrenamientos, testeos, maxAlpha);
	}else if(pliegos == 5){
		generateFiveTraningAndTestingSets(baseCatedra, entrenamientos, testeos, maxAlpha);
	}
 	cout << "k" << "," << "alpha" << "," << "accuracy" << "," << "time" <<endl;

 	for(int k  = 1; k <= 1; k += 5){
 		for(int alpha = minAlpha; alpha <= maxAlpha; alpha += 5){
 			double accurracyPromediada = 0.0;
 			vector<double> acuracyPliegos ; 
 			for(int i = 0 ; i < pliegos; i ++){
 				ImagePredictor imagePredictor;
 				createImagePredictorWithAlpha(imagePredictor, i, alpha);
 				
 				double accuracy;

 				accuracy = acuracyDesImagePredictorConKnn(imagePredictor,testeos[i],alpha, k, time_span);

 				
 				acuracyPliegos.push_back(accuracy);
 				accurracyPromediada += acuracyPliegos[i];
 			}

 			
 			
 			accurracyPromediada = accurracyPromediada / pliegos;
 			cout << k << "," << alpha << "," << accurracyPromediada * 100 << "," <<time_span.count() << endl;
 		}
 	}


 	for(int k  = minK; k <= maxK; k += 5){
 		for(int alpha = minAlpha; alpha <= maxAlpha; alpha += 5){
 			double accurracyPromediada = 0.0;
 			vector<double> acuracyPliegos ; 
 			for(int i = 0 ; i < pliegos; i ++){
 				ImagePredictor imagePredictor;
 				createImagePredictorWithAlpha(imagePredictor, i, alpha);
 				
 				double accuracy;

 				accuracy = acuracyDesImagePredictorConKnn(imagePredictor,testeos[i],alpha, k, time_span);
 				
 				acuracyPliegos.push_back(accuracy);
 				accurracyPromediada += acuracyPliegos[i];
 			}
 			
 			accurracyPromediada = accurracyPromediada / pliegos;
 			cout << k << "," << alpha << "," << accurracyPromediada * 100 << "," <<time_span.count() << endl;
 		}
 	}
}

void combinacionAlphaKBaseCatedra5PliegosConKPuntos(){
	
	vector<string> filePaths = levantarArchivosDesdeTest(baseCatedra);

	high_resolution_clock::time_point t1, t2;
	duration<double> time_span ;

 	int minAlpha = 5;
 	int maxAlpha = 100;

 	// K de Knn
 	int minK = 5;
 	int maxK = 100;

 	int pliegos = 5;

 	std::vector<vector<string> > entrenamientos;
 	std::vector<vector<string> > testeos;

 	if(pliegos == 2){
	 	generateTwoTraningAndTestingSets(baseCatedra, entrenamientos, testeos, maxAlpha);
	}else if(pliegos == 5){
		generateFiveTraningAndTestingSets(baseCatedra, entrenamientos, testeos, maxAlpha);
	}
 	cout << "k" << "," << "alpha" << "," << "accuracy" << "," << "time" <<endl;

 	for(int k  = 1; k <= 1; k += 5){
 		for(int alpha = minAlpha; alpha <= maxAlpha; alpha += 5){
 			double accurracyPromediada = 0.0;
 			vector<double> acuracyPliegos ; 
 			for(int i = 0 ; i < pliegos; i ++){
 				ImagePredictor imagePredictor;
 				createImagePredictorWithAlpha(imagePredictor, i, alpha);
 				
 				double accuracy;

 				accuracy = acuracyDesImagePredictorConKPuntos(imagePredictor,testeos[i],alpha, k, time_span);

 				
 				acuracyPliegos.push_back(accuracy);
 				accurracyPromediada += acuracyPliegos[i];
 			}

 			
 			
 			accurracyPromediada = accurracyPromediada / pliegos;
 			cout << k << "," << alpha << "," << accurracyPromediada * 100 << "," <<time_span.count() << endl;
 		}
 	}


 	for(int k  = minK; k <= maxK; k += 5){
 		for(int alpha = minAlpha; alpha <= maxAlpha; alpha += 5){
 			double accurracyPromediada = 0.0;
 			vector<double> acuracyPliegos ; 
 			for(int i = 0 ; i < pliegos; i ++){
 				ImagePredictor imagePredictor;
 				createImagePredictorWithAlpha(imagePredictor, i, alpha);
 				
 				double accuracy;

 				accuracy = acuracyDesImagePredictorConKPuntos(imagePredictor,testeos[i],alpha, k, time_span);
 				
 				acuracyPliegos.push_back(accuracy);
 				accurracyPromediada += acuracyPliegos[i];
 			}
 			
 			accurracyPromediada = accurracyPromediada / pliegos;
 			cout << k << "," << alpha << "," << accurracyPromediada * 100 << "," <<time_span.count() << endl;
 		}
 	}
}



void combinacionAlphaKBaseNueva2PliegosConKnn(){
	
	vector<string> filePaths = levantarArchivosDesdeTest(nuevaBase2);

	high_resolution_clock::time_point t1, t2;
	duration<double> time_span ;

 	int minAlpha = 5;
 	int maxAlpha = 100;

 	// K de Knn
 	int minK = 5;
 	int maxK = 100;

 	int pliegos = 2;

 	std::vector<vector<string> > entrenamientos;
 	std::vector<vector<string> > testeos;

 	if(pliegos == 2){
	 	generateTwoTraningAndTestingSets(baseCatedra, entrenamientos, testeos, maxAlpha);
	}else if(pliegos == 5){
		generateFiveTraningAndTestingSets(baseCatedra, entrenamientos, testeos, maxAlpha);
	}
 	cout << "k" << "," << "alpha" << "," << "accuracy" << "," << "time" <<endl;

 	for(int k  = 1; k <= 1; k += 5){
 		for(int alpha = minAlpha; alpha <= maxAlpha; alpha += 5){
 			double accurracyPromediada = 0.0;
 			vector<double> acuracyPliegos ; 
 			for(int i = 0 ; i < pliegos; i ++){
 				ImagePredictor imagePredictor;
 				createImagePredictorWithAlpha(imagePredictor, i, alpha);
 				
 				double accuracy;

 				accuracy = acuracyDesImagePredictorConKnn(imagePredictor,testeos[i],alpha, k, time_span);

 				
 				acuracyPliegos.push_back(accuracy);
 				accurracyPromediada += acuracyPliegos[i];
 			}

 			
 			
 			accurracyPromediada = accurracyPromediada / pliegos;
 			cout << k << "," << alpha << "," << accurracyPromediada * 100 << "," <<time_span.count() << endl;
 		}
 	}


 	for(int k  = minK; k <= maxK; k += 5){
 		for(int alpha = minAlpha; alpha <= maxAlpha; alpha += 5){
 			double accurracyPromediada = 0.0;
 			vector<double> acuracyPliegos ; 
 			for(int i = 0 ; i < pliegos; i ++){
 				ImagePredictor imagePredictor;
 				createImagePredictorWithAlpha(imagePredictor, i, alpha);
 				
 				double accuracy;

 				accuracy = acuracyDesImagePredictorConKnn(imagePredictor,testeos[i],alpha, k, time_span);
 				
 				acuracyPliegos.push_back(accuracy);
 				accurracyPromediada += acuracyPliegos[i];
 			}
 			
 			accurracyPromediada = accurracyPromediada / pliegos;
 			cout << k << "," << alpha << "," << accurracyPromediada * 100 << "," <<time_span.count() << endl;
 		}
 	}
}

void combinacionAlphaKBaseNueva2PliegosConKpuntos(){
	
	vector<string> filePaths = levantarArchivosDesdeTest(nuevaBase2);

	high_resolution_clock::time_point t1, t2;
	duration<double> time_span ;

 	int minAlpha = 5;
 	int maxAlpha = 100;

 	// K de Knn
 	int minK = 5;
 	int maxK = 100;

 	int pliegos = 2;

 	std::vector<vector<string> > entrenamientos;
 	std::vector<vector<string> > testeos;

 	if(pliegos == 2){
	 	generateTwoTraningAndTestingSets(baseCatedra, entrenamientos, testeos, maxAlpha);
	}else if(pliegos == 5){
		generateFiveTraningAndTestingSets(baseCatedra, entrenamientos, testeos, maxAlpha);
	}
 	cout << "k" << "," << "alpha" << "," << "accuracy" << "," << "time" <<endl;

 	for(int k  = 1; k <= 1; k += 5){
 		for(int alpha = minAlpha; alpha <= maxAlpha; alpha += 5){
 			double accurracyPromediada = 0.0;
 			vector<double> acuracyPliegos ; 
 			for(int i = 0 ; i < pliegos; i ++){
 				ImagePredictor imagePredictor;
 				createImagePredictorWithAlpha(imagePredictor, i, alpha);
 				
 				double accuracy;

 				accuracy = acuracyDesImagePredictorConKPuntos(imagePredictor,testeos[i],alpha, k, time_span);

 				
 				acuracyPliegos.push_back(accuracy);
 				accurracyPromediada += acuracyPliegos[i];
 			}

 			
 			
 			accurracyPromediada = accurracyPromediada / pliegos;
 			cout << k << "," << alpha << "," << accurracyPromediada * 100 << "," <<time_span.count() << endl;
 		}
 	}


 	for(int k  = minK; k <= maxK; k += 5){
 		for(int alpha = minAlpha; alpha <= maxAlpha; alpha += 5){
 			double accurracyPromediada = 0.0;
 			vector<double> acuracyPliegos ; 
 			for(int i = 0 ; i < pliegos; i ++){
 				ImagePredictor imagePredictor;
 				createImagePredictorWithAlpha(imagePredictor, i, alpha);
 				
 				double accuracy;

 				accuracy = acuracyDesImagePredictorConKPuntos(imagePredictor,testeos[i],alpha, k, time_span);
 				
 				acuracyPliegos.push_back(accuracy);
 				accurracyPromediada += acuracyPliegos[i];
 			}
 			
 			accurracyPromediada = accurracyPromediada / pliegos;
 			cout << k << "," << alpha << "," << accurracyPromediada * 100 << "," <<time_span.count() << endl;
 		}
 	}
}



void combinacionAlphaKBaseNueva5PliegosConKnn(){
	
	vector<string> filePaths = levantarArchivosDesdeTest(nuevaBase2);

	high_resolution_clock::time_point t1, t2;
	duration<double> time_span ;

 	int minAlpha = 5;
 	int maxAlpha = 100;

 	// K de Knn
 	int minK = 5;
 	int maxK = 100;

 	int pliegos = 5;

 	std::vector<vector<string> > entrenamientos;
 	std::vector<vector<string> > testeos;

 	if(pliegos == 2){
	 	generateTwoTraningAndTestingSets(baseCatedra, entrenamientos, testeos, maxAlpha);
	}else if(pliegos == 5){
		generateFiveTraningAndTestingSets(baseCatedra, entrenamientos, testeos, maxAlpha);
	}
 	cout << "k" << "," << "alpha" << "," << "accuracy" << "," << "time" <<endl;

 	for(int k  = 1; k <= 1; k += 5){
 		for(int alpha = minAlpha; alpha <= maxAlpha; alpha += 5){
 			double accurracyPromediada = 0.0;
 			vector<double> acuracyPliegos ; 
 			for(int i = 0 ; i < pliegos; i ++){
 				ImagePredictor imagePredictor;
 				createImagePredictorWithAlpha(imagePredictor, i, alpha);
 				
 				double accuracy;

 				accuracy = acuracyDesImagePredictorConKnn(imagePredictor,testeos[i],alpha, k, time_span);

 				
 				acuracyPliegos.push_back(accuracy);
 				accurracyPromediada += acuracyPliegos[i];
 			}

 			
 			
 			accurracyPromediada = accurracyPromediada / pliegos;
 			cout << k << "," << alpha << "," << accurracyPromediada * 100 << "," <<time_span.count() << endl;
 		}
 	}


 	for(int k  = minK; k <= maxK; k += 5){
 		for(int alpha = minAlpha; alpha <= maxAlpha; alpha += 5){
 			double accurracyPromediada = 0.0;
 			vector<double> acuracyPliegos ; 
 			for(int i = 0 ; i < pliegos; i ++){
 				ImagePredictor imagePredictor;
 				createImagePredictorWithAlpha(imagePredictor, i, alpha);
 				
 				double accuracy;

 				accuracy = acuracyDesImagePredictorConKnn(imagePredictor,testeos[i],alpha, k, time_span);
 				
 				acuracyPliegos.push_back(accuracy);
 				accurracyPromediada += acuracyPliegos[i];
 			}
 			
 			accurracyPromediada = accurracyPromediada / pliegos;
 			cout << k << "," << alpha << "," << accurracyPromediada * 100 << "," <<time_span.count() << endl;
 		}
 	}
}

void combinacionAlphaKBaseNueva5PliegosConKPuntos(){
	
	vector<string> filePaths = levantarArchivosDesdeTest(nuevaBase2);

	high_resolution_clock::time_point t1, t2;
	duration<double> time_span ;

 	int minAlpha = 5;
 	int maxAlpha = 100;

 	// K de Knn
 	int minK = 5;
 	int maxK = 100;

 	int pliegos = 5;

 	std::vector<vector<string> > entrenamientos;
 	std::vector<vector<string> > testeos;

 	if(pliegos == 2){
	 	generateTwoTraningAndTestingSets(baseCatedra, entrenamientos, testeos, maxAlpha);
	}else if(pliegos == 5){
		generateFiveTraningAndTestingSets(baseCatedra, entrenamientos, testeos, maxAlpha);
	}
 	cout << "k" << "," << "alpha" << "," << "accuracy" << "," << "time" <<endl;

 	for(int k  = 1; k <= 1; k += 5){
 		for(int alpha = minAlpha; alpha <= maxAlpha; alpha += 5){
 			double accurracyPromediada = 0.0;
 			vector<double> acuracyPliegos ; 
 			for(int i = 0 ; i < pliegos; i ++){
 				ImagePredictor imagePredictor;
 				createImagePredictorWithAlpha(imagePredictor, i, alpha);
 				
 				double accuracy;

 				accuracy = acuracyDesImagePredictorConKPuntos(imagePredictor,testeos[i],alpha, k, time_span);

 				
 				acuracyPliegos.push_back(accuracy);
 				accurracyPromediada += acuracyPliegos[i];
 			}

 			
 			
 			accurracyPromediada = accurracyPromediada / pliegos;
 			cout << k << "," << alpha << "," << accurracyPromediada * 100 << "," <<time_span.count() << endl;
 		}
 	}


 	for(int k  = minK; k <= maxK; k += 5){
 		for(int alpha = minAlpha; alpha <= maxAlpha; alpha += 5){
 			double accurracyPromediada = 0.0;
 			vector<double> acuracyPliegos ; 
 			for(int i = 0 ; i < pliegos; i ++){
 				ImagePredictor imagePredictor;
 				createImagePredictorWithAlpha(imagePredictor, i, alpha);
 				
 				double accuracy;

 				accuracy = acuracyDesImagePredictorConKPuntos(imagePredictor,testeos[i],alpha, k, time_span);
 				
 				acuracyPliegos.push_back(accuracy);
 				accurracyPromediada += acuracyPliegos[i];
 			}
 			
 			accurracyPromediada = accurracyPromediada / pliegos;
 			cout << k << "," << alpha << "," << accurracyPromediada * 100 << "," <<time_span.count() << endl;
 		}
 	}
}









int main(){
	
}