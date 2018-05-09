#include <vector>
#include <algorithm>
#include "Image.cpp"
#include "matrix.h"
#include <math.h>

using namespace std;

class ImagePredictor
{
public:

	vector<int> photoIds; // ***************************** qué es esto??? si son las categorías, photoIds[i] corresponde con basicImagePixelMatriz[i]??????
	vector<Image> imageDataSet; //MATRIZ X (inicial)
	
	vector<vector<double> > basicImagePixelMatrix; // cada vector<double> es una imagen del archivo de entrada
	vector<vector<double> > basisChangeMatrix;
	vector<vector<double> > pcaMatrix;

	int currentAlfa;

	int n; //Cantidad de imagenes
	int m; //TOTAL DE PIXELS
  
  	// crea la basicImagePixelMatrix, que es la matriz X (inicial) en formato de vector<vector<double>>
	ImagePredictor(){
		createImageMatrix();
		
	}; 

	~ImagePredictor(){};

	// llena el vector imageDataSet con todas las imágenes del archivo de entrada (cuyo nombre es el primer elemento de imageFileDataSet, el parámetro de entrada)
	void loadImagesFromFileDataSet(vector<string>& imageFileDataSet) {
		imageDataSet = vector<Image>();
		n = 0;
		m = 0; 

		n = imageDataSet.size();

		for (int i = 0; i < n; ++i)
		{
			Image img = Image(imageFileDataSet[i]);
			m = img.width * img.height; 
			imageDataSet.push_back(img);
		}
	}

	//  
	/*
		* Z = vector de (mx1)
		* 
		* V = matriz de autovectores (m*alfa)
		* 
		* z` = V^t * z  (alfa*1)
		* 
		*----------------------------------
		*  X matriz inicial de imagenes (n*m)
		* 	
		* 	X' = X * V ===> (n*alfa)
		* 
		* -------------
		* 
		* Luego, kNN(z', X', k) -> devuelve etiqueta. "k" es el parametro de con cuantos vecinos cercanos se vota. 
		* 
		* 
	*/


	// hay que hacer kNN acá


	//PCA Y CAMBIO DE BASE
   	void generateBasisChangeMatrix(int alfa, int niter) {
   		currentAlfa = alfa;
		// genero matriz X con pixeles en doubles
    	
    	// generar vector U de medianas
		vector<double> vectorEsperanza;
 		for (int i = 0; i < m; i++) {
			vectorEsperanza[i] = 0.0;
			for (int j = 0; j < n; j++) {
				vectorEsperanza[i] += basicImagePixelMatrix[j][i];
			}
			vectorEsperanza[i] = vectorEsperanza[i] / n;
			//En U[i] tengo el promedio del pixel i
		}

		//Creo la Matriz X donde X_i = (X_i - u ) / sqrt(n-1)
    	vector<vector<double> > matrizRestadoEsperanza(n, vector<double>(m, 0.0));
    	for (int i = 0 ;  i < m; i++){ //itero columnas
        	for (int j = 0; j < n; j++) { //itero filas

				matrizRestadoEsperanza[j][i] = (basicImagePixelMatrix[j][i] - vectorEsperanza[i]) / sqrt(n-1) ;
		
			}
    	}

    	
    	//(X^t)
    	vector<vector<double> > matrizRestadoEsperanzaTraspuesta(m, vector<double>(n, 0.0));


		//Creo M matriz de covarianzas. M = [ (X^t)*(X) ] * (1/n-1)
		vector<vector<double> > matrizCovarianza(m, vector<double>(m, 0.0));

		trasponerMatriz(matrizRestadoEsperanza, matrizRestadoEsperanzaTraspuesta);

		multiplicarMatricesDouble(matrizRestadoEsperanzaTraspuesta, matrizRestadoEsperanza, matrizCovarianza );

		multiplicarEscalarPorMatriz(1/(n-1), matrizCovarianza);
		
    
		//Ahora hay que diagonalizar M mediante el MetodoDeLaPotencia.
    	//Luego de obtener el primer autovalor y autovector, continuamos haciendo Deflacion para obtener el resto de autoval y autovec.
    	//Hacer ALFA deflaciones, donde ALFA es la cantidad de columnas relevantes que vamos a dejar en nuestro cambio de base.
		//Finalmente, la matriz de cambio de base no son mas que los ALFA autovectores mas relevantes de la matriz M.
     	
     	vector<vector<double> > matrizCambioBase(m, vector<double>(alfa, 0.0));
    
		//Deflacion. Se modifica la matriz alfa cantidad de veces para obtener los alfa primeros autovectores.
		// ********************POR FAVOR QUE ALGUIEN ENCAPSULE EL MÉTODO DE DEFLACIÓN A OTRA FUNCIÓN***************
    	for(int i=0; i< alfa ; i++){
      
      		double Ai; //Autovector de la iteracion i
			vector<double> Vi(m, 0.0);//Autovector 1. Debe ser uno aleatorio para empezar el metodo de la potencia
    		for(i=0;i<m;i++){
        		double f = (double)rand() / (double)RAND_MAX;
          		Vi[i] = f*255;
        	}

      		Ai = metodoDeLaPotencia(matrizCovarianza, Vi, niter);
      
    		//Vi ahora tiene el iesimo autovector de M. Esta sera nuestra primera columna de la matriz 
  			for(int j=0; j<m; j++){
      			matrizCambioBase[j][i] = Vi[i];
    		}
      		vector<vector<double> > matrizProductoAutovectores(m , vector<double>(m,0.0));
      		generarMatrizDeVectores(Vi, Vi, matrizProductoAutovectores); //Vi*Vi^t
      		multiplicarEscalarPorMatriz(Ai, matrizProductoAutovectores); // Ai * (Vi*Vi^t)
      		restarMatrices(matrizCovarianza, matrizProductoAutovectores);
    	}
    
    	basisChangeMatrix = matrizCambioBase;
    
	  

	}
	
	
	
  
  
  //deprecado
	// string obtainKSimilarPicture(string fileName){

	// 	//transformo a nuestra estructura la imagen a identificar
	// 	Image img = Image(fileName);
	// 	unsigned int imageLength = img.width * img.height;
	// 	uchar* aplanatedImageArray = new uchar[imageLength];
	// 	img.aplanateImageArray(aplanatedImageArray); // esto es necesario pues no puedo acceder a la parte privada de la imagen

	// 	// obtengo la norma2 de la nueva imagen
	// 	int newImageNorm2 = obtainVectorNorm(aplanatedImageArray, imageLength );

	// 	// creo un vector donde guardaré la distancia euclídea entre los puntos que representan a las imágenes de la base y el punto que representa a la imagen a identificar
	// 	vector<unsigned int> squaredNorm2MinusNewPicture;
	// 	for (int i = 0; i < imageDataSet.size(); i++){
	// 		squaredNorm2MinusNewPicture.push_back(abs(newImageNorm2 - obtainVectorNorm(imageMatrix[i], imageLength)));
	// 	}

	// 	// ordeno el vector para luego tomar los k más cercanos
	// 	std::sort(squaredNorm2MinusNewPicture.begin(), squaredNorm2MinusNewPicture.end());

	// 	//buscar los k primeros y definir cual es la id moda, retornar el path de la imagen con dicho id

	// 	delete aplanatedImageArray;

	// }

	

private:
	double obtainVectorNorm(vector<double>& vec, unsigned int imageLength){
		double norma2 = 0.0;

		for(int i = 0; i < vec.size(); i++){
			norma2 += vec[i]*vec[i];
		}

		norma2 = sqrt(norma2);

		return norma2;
	}

	void normalizar(vector<double>& vec){
		double sumaDouble = 0.0;

		for (int i = 0; i < vec.size(); ++i)
		{
			sumaDouble += vec[i]*vec[i];
		}

		sumaDouble = sqrt(sumaDouble);

		for (int i = 0; i < vec.size(); ++i)
		{
			vec[i] = vec[i]/sumaDouble; 
		}
	}

	double metodoDeLaPotencia(vector<vector<double> >& matriz, vector<double> vec, int niter){
  		
  		if (matriz.size() == 0 || matriz[0].size() != vec.size())
  		{
  			cout << "Parametros para metodo de la potencia erroneos" << endl;
			return 0;
		}

    	double lambda1; 
    	
    	for(int i = 0; i< niter; i++){
      		vector<double> tempVector(vec.size(), 0.0);

      		multiplicarMatrizVectorDouble(matriz, vec, tempVector);
      		vec = tempVector;
      		normalizar(vec);
 	  		//En cada iteracion, perfecciono el autovector que devuelvo
    	}

		vector<double> matrizXVec(vec.size(), 0.0);

    	multiplicarMatrizVectorDouble(matriz, vec, matrizXVec);

    	double vtBvv = productoInternoVectores(vec, matrizXVec);
    	double vtv = productoInternoVectores(vec, vec);
    	lambda1 = vtBvv / vtv;
    
    	return lambda1;
	}  


	void createImageMatrix(){
    	vector<vector<double> > matrix(imageDataSet.size(), vector<double>(n * m, 0.0));
		for (int i = 0; i < imageDataSet.size(); ++i)
		{		
        	for( int j = 0; j < n * m; j++){
      			matrix[i][j] = (double) imageDataSet[i].obtainPixelValue(j); 
    		}
		}

		basicImagePixelMatrix = matrix;	
	}
};