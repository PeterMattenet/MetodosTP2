//#include <vector>

using namespace std;

//multiplica matrices de dimensiones bien definidas y guarda el resultado en C
int multiplicarMatricesDouble(vector<vector<double> >& matrizA, vector<vector<double> >& matrizB, vector<vector<double> >& matrizC ){
	if (matrizA[0].size() != matrizB.size() || matrizC.size() != matrizA.size() || matrizB[0].size() != matrizC[0].size())
		return -1;

	for(int i = 0; i < matrizC.size(); i++){
		for (int j = 0; j < matrizC[i].size(); j++)
		{
			for (int k = 0; k < matrizA[0].size(); ++k)
			{
				matrizC[i][j] += matrizA[i][k] * matrizB[k][j]; 
			} 
		}
		cout << "variando i " << i << endl;
	}
	return 1;

}

void multiplicarVectorMatrizDouble(vector<double>& vec, vector<vector<double> >& matriz, vector<double>& vecRes){
	if (vec.size() != matriz.size() || matriz[0].size() != vecRes.size()){
		cout << "Multiplicar vector por matriz con parametros erroneos" << endl;
		return;
	}

	for (int i = 0; i < vecRes.size(); ++i)
	{
		for (int j = 0; j < vec.size(); ++j)
		{
			vecRes[i] += vec[j] * matriz[j][i];
		}
	}

}

//Resta A - B, guardando el resultado en A.
void restarMatrices(vector<vector<double> >& matrizA, vector<vector<double> >& matrizB){
	if (matrizA.size() != matrizB.size() || matrizA[0].size() != matrizB[0].size())
		cout << "Restar Matrices con parametros erroneos" << endl;

	for (int i = 0; i < matrizA.size(); ++i)
	{
		for (int j = 0; j < matrizA[i].size(); ++j)
		{
			matrizA[i][j] = matrizA[i][j] - matrizB[i][j];
		}	
	}
}

void generarMatrizDeVectores(vector<double>& vec1, vector<double>& vec2, vector<vector<double> >& matriz){
	if (matriz.size() != vec1.size() || matriz[0].size() != vec2.size())
		cout << "Generar Matriz de Vectores con parametros erroneos" << endl;

	for (int i = 0; i < vec1.size(); ++i)
	{
		for (int j = 0; j < vec2.size(); ++j)
		{
			matriz[i][j] = vec1[i] * vec2[j];
		}
	}
}

double productoInternoVectores(vector<double>& vec1, vector<double>& vec2){
	if (vec1.size() != vec2.size()){
		cout << "Producto interno de vectores con parametros erroneos" << endl;
		return 0.0;
	}

    double productoInterno = 0.0;
    for(int i=0; i< vec1.size(); i++){
    	productoInterno += (vec1[i] * vec2[i]);
    }
    
    return productoInterno;
}


int multiplicarMatrizVectorDouble(vector<vector<double> >& matriz, vector<double>& vec, vector<double>& vectorRes){
	if (matriz[0].size() != vec.size() || matriz.size() != vectorRes.size())
	{
		cout << "Multiplicar matriz por vector con parametros erroneos"<< endl;
		return -1;	
	}

	for(int i = 0; i < vectorRes.size(); i++){
		for (int j = 0; j < matriz[i].size(); j++)
		{
			vectorRes[i] += matriz[i][j] * vec[j];
		}
	}

	return 1;
}

void multiplicarEscalarPorMatriz(double escalar, vector<vector<double> >& matriz){
	for (int i = 0; i < matriz.size(); ++i)
	{
		for (int j = 0; j < matriz[i].size(); ++j)
		{
			matriz[i][j] = matriz[i][j] * escalar;
		}
	}
}

void trasponerMatriz(vector<vector<double> >& matriz, vector<vector<double> >& matrizT){
	if (matriz.size() != matrizT[0].size() || matriz[0].size() != matrizT.size())
		cout << "Trasponer matriz con parametros erroneos" <<endl;


	for (int i = 0; i < matriz.size(); ++i)
	{
		for (int j = 0; j < matriz[0].size(); ++j)
		{
			matrizT[j][i] = matriz[i][j];
		}
	}

}

void mostrarMatriz(vector<vector<double> >& matriz){
	int n = matriz.size();
	int m = matriz[0].size();

	for (int i = 0; i < n; ++i)
	{
		for (int j = 0; j < m; ++j)
		{
			cout << matriz[i][j] << " " ;
		}
		cout << endl;
	}
}

void mostrarVector(vector<double>& vec){
	for (int i = 0; i < vec.size(); ++i)
	{
		cout << vec[i] << " ";
	}
	cout << endl;
}

void mostrarVectorInt(vector<int>& vec){
	for (int i = 0; i < vec.size(); ++i)
	{
		cout << vec[i] << " ";
	}
	cout << endl;
}
