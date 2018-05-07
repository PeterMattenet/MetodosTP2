//#include <vector>

using namespace std;

//multiplica matrices de dimensiones bien definidasy guarda el resultado en C
int MultiplicarMatricesDouble(vector<vector<double> >& matrizA, vector<vector<double> >& matrizB, vector<vector<double> >& matrizC ){
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
	}

	return 1;

}

void RestarMatrices(vector<vector<double>>& matrizA, vector<vector<double>>& matrizB){
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

void GenerarMatrizDeVectores(vector<double>& vec1, vector<double>& vec2, vector<vector<double>>& matriz){
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


int MultiplicarMatrizVectorDouble(vector<vector<double> >& matriz, vector<double>& vec, vector<double>& vectorRes){
	if (matriz[0].size() != vec.size() || matriz[0].size() != vectorRes.size())
		return -1;

	for(int i = 0; i < vectorRes.size(); i++){
		for (int j = 0; j < matriz[i].size(); j++)
		{
			vectorRes[i] += matriz[i][j] * vec[j];
		}
	}

	return 1;
}

void MultiplicarEscalarPorMatriz(double escalar, vector<vector<double> >& matriz){
	for (int i = 0; i < matriz.size(); ++i)
	{
		for (int j = 0; j < matriz[i].size(); ++j)
		{
			matriz[i][j] = matriz[i][j] * escalar;
		}
	}
}

int TrasponerMatriz(vector<vector<double>>& matriz, vector<vector<double>>& matrizT){
	if (matriz.size() != matrizT.size() || matriz[0].size() != matrizT[0].size())
		return -1;

	for (int i = 0; i < matriz.size(); ++i)
	{
		for (int j = 0; i < matriz[i].size(); ++j)
		{
			matrizT[j][i] = matriz[i][j];
		}
	}

	return 1;
}

