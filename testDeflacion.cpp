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
	
	ImagePredictor predictor = ImagePredictor();

	vector<double> Vi(4, 0.0);//Autovector 1. Debe ser uno aleatorio para empezar el metodo de la potencia
    for(int i=0;i<4;i++){
        double f = (double)rand() / (double)RAND_MAX;
    	Vi[i] = f*255;
    }


	double lambda1 = predictor.metodoDeLaPotencia(testMatrix, Vi, 20);
	cout << "Lambda esperado 39.851293788948475 " << endl;
	cout << "Lambda obtenido " << lambda1 << endl; 

	vector<double> vecMatrizPorAutovector(4, 0.0);
	multiplicarMatrizVectorDouble(testMatrix, Vi, vecMatrizPorAutovector);
	Vi[0] = Vi[0] * 39.851293788948475 ;
	Vi[1] = Vi[1] * 39.851293788948475 ;
	Vi[2] = Vi[2] * 39.851293788948475 ;
	Vi[3] = Vi[3] * 39.851293788948475 ;

	cout << "Comparar Ax por lambda x" << endl;
	mostrarVector(vecMatrizPorAutovector);
	
	mostrarVector(Vi);

	
}

void testObtenerMatrizAlfaAutovectores(){
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
	
	ImagePredictor predictor = ImagePredictor();

	//autovalores
	double lambda1 = 39.851293788948475;
	double lambda2 = 11.179458412990325;
	double lambda3 = -8.524285896354167;
	double lambda4 = 1.4935336944153677 ;
	//
	vector<vector<double>> cambioDeBase(4, vector<double>(4, 0.0));
	predictor.obtenerAutovectoresDeflacion(testMatrix, cambioDeBase, 4, 50);

	vector<vector<double>> matrizAutovalores(4, vector<double>(4,0.0));
	matrizAutovalores[0][0] = lambda1;
	matrizAutovalores[1][1] = lambda2;
	matrizAutovalores[2][2] = lambda3;
	matrizAutovalores[3][3] = lambda4;

	vector<vector<double>> cambioDeBasePorAutovalores(4, vector<double>(4,0.0));
	vector<vector<double>> testMatrixPorAutovectores(4, vector<double>(4,0.0));

	multiplicarMatricesDouble(testMatrix, cambioDeBase, testMatrixPorAutovectores);
	multiplicarMatricesDouble(cambioDeBase, matrizAutovalores, cambioDeBasePorAutovalores);

	cout << "Comparar A * autovectores = autovectores * matriz de autovalores" << endl;
	mostrarMatriz(testMatrixPorAutovectores);
	mostrarMatriz(cambioDeBasePorAutovalores);

	// vector<double> v3(4,0.0);
	// v3[0]= cambioDeBase[0][2];;
	// v3[1]= cambioDeBase[1][2] ;
	// v3[2]= cambioDeBase[2][2] ;
	// v3[3]= cambioDeBase[3][2] ;

	// vector<double> v3lambda3(4,0.0);
	
	// v3lambda3[0]= cambioDeBase[0][2] * lambda3;
	// v3lambda3[1]= cambioDeBase[1][2] * lambda3;
	// v3lambda3[2]= cambioDeBase[2][2] * lambda3;
	// v3lambda3[3]= cambioDeBase[3][2] * lambda3;

	// vector<double> testMatrixv3(4,0.0);
	// multiplicarMatrizVectorDouble(testMatrix, v3, testMatrixv3 );
	// mostrarVector(v3lambda3);
	// mostrarVector(testMatrixv3);
	

}


int main(){
	testObtenerAutovalor();
	testObtenerMatrizAlfaAutovectores();
}