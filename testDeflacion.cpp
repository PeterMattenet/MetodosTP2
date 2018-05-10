#include <iostream>
#include <fstream>
//#include <chrono>
#include <vector>
#include "ImagePredictor.h"


void testObtenerAutovalor(){
	vector<vector<double>> testMatrix(4, vector<double>(4, 0.0));
	testMatrix[0][0] = 11.0;
	testMatrix[0][1] = 13.0;
	testMatrix[0][2] = 12.0;
	testMatrix[0][3] = 15.0;
	testMatrix[1][0] = 13.0;
	testMatrix[1][1] = 11.0;
	testMatrix[1][2] = 7.0;
	testMatrix[1][3] = 7.0;
	testMatrix[2][0] = 12.0;
	testMatrix[2][1] = 7.0;
	testMatrix[2][2] = 7.0;
	testMatrix[2][3] = 0.0;
	testMatrix[3][0] = 15.0;
	testMatrix[3][1] = 7.0;
	testMatrix[3][2] = 0.0;
	testMatrix[3][3] = 15.0;
	mostrarMatriz(testMatrix);

	ImagePredictor predictor = ImagePredictor();

	vector<double> Vi(4, 0.0);//Autovector 1. Debe ser uno aleatorio para empezar el metodo de la potencia
    for(int i=0;i<4;i++){
        double f = (double)rand() / (double)RAND_MAX;
    	Vi[i] = f*255;
    }


	double lambda1 = predictor.metodoDeLaPotencia(testMatrix, Vi, 20);
	cout << "Lambda esperado 39.851293788948475 " << endl;
	cout << "Lambda obtenido " << lambda1 << endl; 
}

int main(){
	testObtenerAutovalor();
}