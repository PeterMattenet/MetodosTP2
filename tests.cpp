#include <iostream>
#include <fstream>
//#include <chrono>
#include <vector>
#include "ImagePredictor.h"
//#include "matrix.h"

void testMultiplicarMatricesDouble(){


	vector<vector<double> > A(2, vector<double>(2, 0.0)); 
	vector<vector<double> > B(2, vector<double>(1, 0.0)); 
	vector<vector<double> > C(2, vector<double>(1, 0.0)); 


	A[0][0] = 2.0;
	A[0][1] = 5.0;
	A[1][0] = 1.0;
	A[1][1] = 3.0;

	B[0][0] = 1.0;
	B[1][0] = 2.0;

	multiplicarMatricesDouble(A, B, C);
	mostrarMatriz(C);
}

void multiplicarMatrizVectorDouble(){

	vector<vector<double> > A(2, vector<double>(2, 0.0)); //A = n*m
	
	vector<double>  B(2, 0.0);
	vector<double>   C(2, 0.0);
	
	A[0][0] = 2.0;
	A[0][1] = 5.0;
	A[1][0] = 1.0;
	A[1][1] = 3.0;

	B[0] = 1;
	B[1] = 2;

	multiplicarMatrizVectorDouble(A,B,C);
	for (int i = 0; i < C.size(); ++i)
	{
		cout << C[i] << endl;
	}
}

void testTrasponerMatriz(){
	vector<vector<double> > A(2, vector<double>(2, 0.0));
	vector<vector<double> > B(2, vector<double>(2, 0.0));  
	A[0][0] = 2.0;
	A[0][1] = 5.0;
	A[1][0] = 1.0;
	A[1][1] = 3.0;

	mostrarMatriz(A);
	cout << "traspuesta!"<< endl;
	trasponerMatriz(A, B);
	mostrarMatriz(B);

}



void testMostrarMatriz(){
	vector<vector<double> > A(2, vector<double>(2, 0.0)); //A = n*m
	vector<vector<double> > B(2, vector<double>(1, 0.0)); //B = m*n

	A[0][0] = 2;
	A[0][1] = 5;
	A[1][0] = 1;
	A[1][1] = 3;

	B[0][0] = 1;
	B[1][0] = 2;
	
	//cout << A.size() << endl;
	//cout << A[0].size() << endl;
	//cout << A[0][1] << endl;
	mostrarMatriz(B);

}







int main(){
	testTrasponerMatriz();
	//multiplicarMatrizVectorDouble();
	//testGenerarMatrizDeVectores();
	//testMultiplicarMatricesDouble();
	//testMostrarMatriz();
	return 0;
}