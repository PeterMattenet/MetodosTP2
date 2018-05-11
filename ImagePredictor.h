#include <vector>
#include <algorithm>
#include "Image.cpp"
#include "matrix.h"
#include <math.h>
#include <algorithm>
#include <map>

using namespace std;

class ImagePredictor
{
public:

	vector<int> photoIds; // ***************************** qué es esto??? si son las categorías, photoIds[i] corresponde con basicImagePixelMatriz[i]??????
	vector<Image> imageDataSet; //MATRIZ X (inicial)
	
	//(n x m )
	vector<vector<double> > basicImagePixelMatrix; // cada vector<double> es una imagen del archivo de entrada
	//(m x alfa)
	vector<vector<double> > basisChangeMatrix;
	//(n x alfa)
	vector<vector<double> > pcaMatrix;

	int currentAlfa;

	int n; //Cantidad de imagenes
	int m; //pixeles de una imagen en total
	int alfa;
  
  	// crea la basicImagePixelMatrix, que es la matriz X (inicial) en formato de vector<vector<double>>
	ImagePredictor(){
		
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

		createImageMatrix();
	}

	//  
	


	// clasificarImagen basicamente resuelve todo el ejercicio. Es el principal metodo al cual llamar.
	//PRE: Asume que las imagenes ya fueron levantadas con loadImagesFromFileDataSet
	//PARAMS: Imagen. K: vecinos. Alfa: columnas/parametros relevantes. Inter: Cantidad iteraciones para Metodo de la potencia.
	string clasificarImagen(string image, int k, int alfa, int inter){
		
		//Levanto la imagen, y la pongo en un vector de doubles
		//Z = vector de (mx1)
		Image img = Image(image);
		vector<double> imagePixels;
		obtainImageInVectorDoubles(img,imagePixels);
		
		//PCA
		//Genero la matriz de cambio de base
		//V = matriz de autovectores (m*alfa)
		generateBasisChangeMatrix(alfa, inter);
		vector<vector<double> > matrizCambioDeBase(m, vector<double>(alfa, 0.0));
		matrizCambioDeBase = this->basisChangeMatrix;


		//Genero la matriz donde trasponer el cambio de matriz: V^t (alfa*m)
		vector<vector<double> > matrizCambioDeBaseTraspuesta(alfa, vector<double>(m, 0.0));
		trasponerMatriz(matrizCambioDeBase, matrizCambioDeBaseTraspuesta);
		

		//Ahora tengo que cambiar de base tanto Z como mi baseMatrix X.
		
		//z1 = V^t * z  (alfa*1)
		vector<double> z1(alfa, 0.0);
		multiplicarMatrizVectorDouble(matrizCambioDeBaseTraspuesta,imagePixels, z1);

		//X = matriz inicial de imagenes (n*m)
		vector<vector<double> > matrizDeImagenes(this->basicImagePixelMatrix.size(), vector<double>(this->basicImagePixelMatrix[0].size(), 0.0));
		matrizDeImagenes = this->basicImagePixelMatrix;

		//X1' = X * V ===> (n*alfa)
		//YA ESTA GUARDADO EN PCAMATRIX!(Se guardo solo al generar)

		//De todas formas, aca esta otro metodo para hacerlo manualmente:
		//vector<vector<double> > X1(n, vector<double>(alfa, 0.0));
		//multiplicarMatricesDouble(matrizDeImagenes, matrizCambioDeBase, X1);


		
		//Finalmente, llamo a kNN con las z y X ya cambiados de base
		return resolverKnn(z1, k);





		/* Idea de la resolucion:
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


	}

	//CAMBIO DE BASE
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
		trasponerMatriz(matrizRestadoEsperanza, matrizRestadoEsperanzaTraspuesta);


		//Creo M matriz de covarianzas. M = [ (X^t)*(X) ] * (1/n-1)
		vector<vector<double> > matrizCovarianza(m, vector<double>(m, 0.0));
		multiplicarMatricesDouble(matrizRestadoEsperanzaTraspuesta, matrizRestadoEsperanza, matrizCovarianza );

		// M * (1/(n-1))
		multiplicarEscalarPorMatriz(1/(n-1), matrizCovarianza);
		
    
		//Ahora hay que diagonalizar M mediante el MetodoDeLaPotencia.
    	//Luego de obtener el primer autovalor y autovector, continuamos haciendo Deflacion para obtener el resto de autoval y autovec.
    	//Hacer ALFA deflaciones, donde ALFA es la cantidad de columnas relevantes que vamos a dejar en nuestro cambio de base.
		//Finalmente, la matriz de cambio de base no son mas que los ALFA autovectores mas relevantes de la matriz M.
     	
     	vector<vector<double> > matrizCambioBase(m, vector<double>(alfa, 0.0));
    	obtenerAutovectoresDeflacion(matrizCovarianza, matrizCambioBase, alfa, niter);
    	
    	basisChangeMatrix = matrizCambioBase;

    	//pca matrix. (n x alfa)
    	pcaMatrix = vector<vector<double> > (n, std::vector<double>(alfa, 0.0));
    	multiplicarMatricesDouble(basicImagePixelMatrix, matrizCambioBase, pcaMatrix);

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
	void obtenerAutovectoresDeflacion(vector<vector<double> >& matrizOriginal, vector<vector<double> >& matrizResultado, int alfa, int niter){
		this->alfa = alfa;

    	for(int i=0; i< alfa ; i++){
      
      		double Ai; //Autovector de la iteracion i
			vector<double> Vi(matrizOriginal.size(), 0.0);//Autovector 1. Debe ser uno aleatorio para empezar el metodo de la potencia
    		for(int j=0;j<matrizOriginal.size();j++){
        		double f = (double)rand() / (double)RAND_MAX;
          		Vi[j] = f*255;
        	}

      		Ai = metodoDeLaPotencia(matrizOriginal, Vi, niter);
      
    		//Vi ahora tiene el iesimo autovector de M. Esta sera nuestra primera columna de la matriz 
  			for(int j=0; j<m; j++){
      			matrizResultado[j][i] = Vi[i];
    		}
      		vector<vector<double> > matrizProductoAutovectores(m , vector<double>(m,0.0));
      		generarMatrizDeVectores(Vi, Vi, matrizProductoAutovectores); //Vi*Vi^t
      		multiplicarEscalarPorMatriz(Ai, matrizProductoAutovectores); // Ai * (Vi*Vi^t)
      		restarMatrices(matrizOriginal, matrizProductoAutovectores);
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


	string resolverKnn(vector<double> imagePixels, int k){
		

		vector<double> imageAfterBasisChange = vector<double> (alfa, 0.0);
		applyBasisChangeToImagePixelsVector(imagePixels, imageAfterBasisChange);

		map<double,int> distancias;
 		for(int i = 0; i < n; i ++){
	  		distancias.insert(pair<double,int>(distanceBetweenVector(imageAfterBasisChange,pcaMatrix[i]), i));
		}

	 	map<double,int>::reverse_iterator it = distancias.rbegin();
	 	int i = 0 ;

	 	vector<int> votaciones (0,n);
	 	while(it != distancias.rend() && i < k ){
	  		votaciones[it->second] += 1;
  			i++;
	  		it++;
	 	}

	 	map<string,int> votacionDeLabel;

	 	//hago diccionario  key nombre de la imagen, value cantidad de votos
	 	for(int i = 0 ; i < n; i ++){
	 		if(votaciones[i] != 0 ){
	 			if(votacionDeLabel.find(imageDataSet[i].label) == votacionDeLabel.end()){
	 				votacionDeLabel.insert(pair<string,int> (imageDataSet[i].label, 1));	
	 			}
	 			else{
	 				map<string,int>::iterator currentIt = votacionDeLabel.find(imageDataSet[i].label);
	 				currentIt -> second += 1;
	 			}	
	 		}
	 	}

	 	map<string,int>::iterator itRes = votacionDeLabel.begin();
	 	for(map<string,int>::iterator it = votacionDeLabel.begin(); it != votacionDeLabel.end() ; it ++){
	 		if(it -> second > it ->second){
	 			itRes = it;
	 		}
	 	}

	 	return itRes->first;
	}



	

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


	void createImageMatrix(){
    	vector<vector<double> > matrix(imageDataSet.size(), vector<double>(m, 0.0));
		for (int i = 0; i < n; ++i)
		{		
        	for( int j = 0; j < m; j++){
      			matrix[i][j] = imageDataSet[i].obtainPixelValue(j); 
    		}
		}

		basicImagePixelMatrix = matrix;	
	}

	int maximo (vector<int>& v){
		int res = v[0];
		int n = v.size();
		for(int i = 0 ; i < n; i ++){
			res = res > v[i] ? res : v[i];
		}
 		return res;
	}

	void obtainImageInVectorDoubles(Image& image, vector<double>& z){
		for(int i = 0 ; i < m ; i ++){
			double pixel = image.obtainPixelValue(i);
			z.push_back(pixel);
		}
	} 

	void applyBasisChangeToImagePixelsVector(std::vector<double>& imagePixels, vector<double> imageAfterBasisChange){
		multiplicarMatrizVectorDouble(pcaMatrix,imagePixels,imageAfterBasisChange);
	}

	vector<double> restaEntreVectores(vector<double> v1, vector<double> v2 ){
		vector<double> res ;
		int n = v1.size();
		for(int i = 0 ; i < n; i ++){
			res.push_back(v1[i] - v2[i]);
		}
		return res;
	}

	double distanceBetweenVector(vector<double> v1, vector<double> v2 ){
		std::vector<double> v3 = restaEntreVectores(v1,v2);
		sort(v3.begin(), v3.end());
		
		int n = v3.size();
		double ac = 0 ;
		for(int i = 0 ; i < n; i ++){
			ac += v3[i]*v3[i];
		}
		return ac;
	}
};