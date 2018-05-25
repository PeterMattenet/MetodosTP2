#include <iostream>
#include <fstream>
#include <vector>
#include "ImagePredictor.h"
#include <random>
#include <string>
#include <sstream>
#include <chrono> 

using namespace std;

string directoryOfSnap = "snapImagePredictor";

vector<ImagePredictor*> imagePredictors;
vector<vector<string>> pathsEntrenamientoPorFold;
vector<vector<string>> pathsTestPorFold;



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
		nombreFile = "resultados_f1score_y_matriz_confusion" + fileSuffix;
	}
	if(kFoldValue == 5){
		nombreFile = "resultados_f1score_y_matriz_confusion" + fileSuffix;	
	}
	if(kFoldValue == 10){
		nombreFile = "resultados_f1score_y_matriz_confusion"+ fileSuffix;
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



double testF1ScorePromediado(ImagePredictor& predictor, vector<string> testeoPath, int k, vector<string> etiquetas){
	//ImagePredictor predictor = ImagePredictor();
	//predictor.loadImagesFromFileDataSet(entrenamientoPath);

	vector<string> results;
	//predictor.generateBasisChangeMatrixWithSVD(15, 100);
	vector<string> labelsTesteo;
	
	for (int i = 0; i < testeoPath.size(); ++i)
	{
		labelsTesteo.push_back(obtainPathUntilLastFolder(testeoPath[i]));
		string imageObtained = predictor.clasificarImagen(testeoPath[i], k);
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

		if (tpositivos + fpositivos == 0){
			cout << "tpositivos: " << tpositivos << endl;
			cout << "fpositivos " << fpositivos << endl;	
		}else{
			precisionPromediada += ((double) tpositivos / (tpositivos + fpositivos));
		}
		

		if (tpositivos + fnegativos == 0){
			cout << "tpositivos: " << tpositivos << endl;
			cout << "fnegativos " << fnegativos << endl;	
		}
		else{
			recallPromediada += ((double) tpositivos / (tpositivos + fnegativos));
		}
		
	}

	precisionPromediada = precisionPromediada / results.size();
	recallPromediada = recallPromediada / results.size();
	
	cout << "F1 score del algoritmo: " << endl;
	double f1Score =  2 * precisionPromediada * recallPromediada / (precisionPromediada + recallPromediada);
	cout << f1Score  << endl;
	return f1Score;
}

void createImagePredictorWithAlpha(ImagePredictor& res, int k, int alpha){

	res.alfa = alpha;
	

	// res.labels = imagePredictors[k].labels;

	// for(int i = 0 ; i < pathsEntrenamientoPorFold[k].size(); i ++){
	//   	Image* img = new Image();
	//   	img->label  = obtainPathUntilLastFolder(pathsEntrenamientoPorFold[k][i]);
	//   	res.imageDataSet.push_back(img);
	// }

	//res.basicImagePixelMatrix = imagePredictors[k].basicImagePixelMatrix;
	//res.basicImagePixelMatrix = vector
	for(int i = 0 ; i < res.m ; i ++ ){
		vector<double > v ;
		for(int j = 0; j < alpha; j++){
			v.push_back(imagePredictors[k]->basisChangeMatrix[i][j]);
		}
		res.basisChangeMatrix.push_back(v);
	}

	for(int i = 0 ; i < res.n ; i ++ ){
		vector<double > v ;
		for(int j = 0; j < alpha; j++){
			v.push_back(imagePredictors[k]->pcaMatrix[i][j]);
		}
		res.pcaMatrix.push_back(v);
	}
}


void GuardarSnapDeMatrizPCA(vector<string>& pathsEntrenamiento, int alfa, int niter){
	imagePredictors.push_back(new ImagePredictor());
	cout << "Cargando imagenes" << endl;
	cout << pathsEntrenamiento.size() << endl;
	(imagePredictors.back())->loadImagesFromFileDataSet(pathsEntrenamiento);
	cout << "Calculando cambio de base" << endl;
	(imagePredictors.back())->generateBasisChangeMatrixWithSVD(alfa, niter);
}

void testF1ScoreSegunAlfaYk(int kFoldValue){
	vector<string> filePaths = levantarArchivosDesdeTestNipo("tests/testFullBig.in");
	string nombreFile;

	int minAlfa = 5;
	int maxAlfa = 100;
	int minK = 5;
	int maxK = 100;
	// stringstream ss;
	// ss << "alfa" << alfa << "knn" << knn;
	// string fileSuffix = ss.str();
	if(kFoldValue == 2){
		nombreFile = "resultados_f1score_y_matriz_confusion/f1ScoreAlfakK2";
	}
	if(kFoldValue == 5){
		nombreFile = "resultados_f1score_y_matriz_confusion/f1ScoreAlfakK5";	
	}
	if(kFoldValue == 10){
		nombreFile = "resultados_f1score_y_matriz_confusion/f1ScoreAlfakK10";
	}
	fstream sal(nombreFile, ios::out);

	sal << "k,alfa,f1score" << endl;

	cout << "Comienza con K = ";
	cout << kFoldValue << endl;
	cout << endl; 

	vector<vector<string> > imagenesSeparadas(41, vector<string>());
	
	DesordenarImagenes(filePaths, imagenesSeparadas);

	vector<string> etiquetas;
	for (int i = 0; i < imagenesSeparadas.size(); ++i)
	{
		etiquetas.push_back(obtainPathUntilLastFolder(imagenesSeparadas[i][0]));
	}



	
	for (int i = 0; i < kFoldValue; ++i)
	{	
		vector<string> pathsEntrenamiento;
		vector<string> pathsTest;
		RealizarFoldI(i, kFoldValue, imagenesSeparadas, pathsEntrenamiento, pathsTest);
		pathsTestPorFold.push_back(pathsTest);
		pathsEntrenamientoPorFold.push_back(pathsEntrenamiento);
		GuardarSnapDeMatrizPCA(pathsEntrenamiento, maxAlfa, 300);
	}



 	for(int k  = 1; k <= 1; k += 5){
 		for(int alfa = minAlfa; alfa <= maxAlfa; alfa += 5){
 			double F1ScorePromediado = 0.0;
 			for(int i = 0 ; i < kFoldValue; i ++){
 				ImagePredictor imagePredictor = ImagePredictor();
 				cout << "paths Entrenamiento Por Fold " << i << " size" << pathsEntrenamientoPorFold[i].size() << endl;
 				imagePredictor.loadImagesFromFileDataSet(pathsEntrenamientoPorFold[i]);
 				cout << "Cargue imagenes " << endl;
 				createImagePredictorWithAlpha(imagePredictor, i, alfa);
 				cout << "Tamaño paths test: " << pathsTestPorFold[i].size() << endl;
 				cout << "Tamaño paths entrenamiento: " << pathsEntrenamientoPorFold[i].size() << endl;
 				double f1Score = testF1ScorePromediado(imagePredictor,pathsTestPorFold[i], k, etiquetas);
				
 				F1ScorePromediado += f1Score;
 			}

 			F1ScorePromediado = F1ScorePromediado / kFoldValue;
 			sal << k << "," << alfa << "," << F1ScorePromediado << endl;
 			cout << k << "," << alfa << "," << F1ScorePromediado << endl;
 		}
 	}


 	for(int k  = minK; k <= maxK; k += 5){
 		for(int alfa = minAlfa; alfa <= maxAlfa; alfa += 5){
 			double F1ScorePromediado = 0.0;
 			for(int i = 0 ; i < kFoldValue; i ++){
 				ImagePredictor imagePredictor;
 				createImagePredictorWithAlpha(imagePredictor, i, alfa);
 				imagePredictor.loadImagesFromFileDataSet(pathsEntrenamientoPorFold[i]);
 				
 				double f1Score = testF1ScorePromediado(imagePredictor,pathsTestPorFold[i], k, etiquetas);
				
 				F1ScorePromediado += f1Score;
 			}

 			F1ScorePromediado = F1ScorePromediado / kFoldValue;
 			sal << k << "," << alfa << "," << F1ScorePromediado << endl;
 			cout << k << "," << alfa << "," << F1ScorePromediado << endl;
 		}
 	}

	
	sal.close();


}

double testF1Score(int kFoldValue){
	vector<string> filePaths = levantarArchivosDesdeTestNipo("tests/testFullBig.in");
	string nombreFile;

	int minAlfa = 5;
	int maxAlfa = 100;
	int minK = 5;
	int maxK = 100;
	// stringstream ss;
	// ss << "alfa" << alfa << "knn" << knn;
	// string fileSuffix = ss.str();
	if(kFoldValue == 2){
		nombreFile = "resultados_f1score_y_matriz_confusion/f1ScoreAlfakK2";
	}
	if(kFoldValue == 5){
		nombreFile = "resultados_f1score_y_matriz_confusion/f1ScoreAlfakK5";	
	}
	if(kFoldValue == 10){
		nombreFile = "resultados_f1score_y_matriz_confusion/f1ScoreAlfakK10";
	}
	fstream sal(nombreFile, ios::out);

	sal << "k,alfa,f1score" << endl;

	cout << "Comienza con K = ";
	cout << kFoldValue << endl;
	cout << endl; 

	vector<vector<string> > imagenesSeparadas(41, vector<string>());
	
	DesordenarImagenes(filePaths, imagenesSeparadas);

	vector<string> etiquetas;
	for (int i = 0; i < imagenesSeparadas.size(); ++i)
	{
		etiquetas.push_back(obtainPathUntilLastFolder(imagenesSeparadas[i][0]));
	}

	vector<vector<string>> pathsEntrenamientoPorFold;
	vector<vector<string>> pathsTestPorFold;


	for (int i = 0; i < kFoldValue; ++i)
	{	
		vector<string> pathsEntrenamiento;
		vector<string> pathsTest;
		RealizarFoldI(i, kFoldValue, imagenesSeparadas, pathsEntrenamiento, pathsTest);
		pathsTestPorFold.push_back(pathsTest);
		pathsEntrenamientoPorFold.push_back(pathsEntrenamiento);
	}

	ImagePredictor predictor = ImagePredictor();
	predictor.loadImagesFromFileDataSet(pathsEntrenamientoPorFold[0]);
	predictor.generateBasisChangeMatrixWithSVD(65, 300);

	testF1ScorePromediado(predictor, pathsTestPorFold[0], 5, etiquetas);

	return 0.0;
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
	//testF1Score(10);
	return 0;
}





