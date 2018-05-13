#include <iostream>
#include <fstream>
#include <vector>
#include "ImagePredictor.h"


// tuve que ponerlo acá porque el otro es privado
vector<double> restaBetweenVectores(vector<double> v1, vector<double> v2 ){
	vector<double> res ;
	for(int i = 0 ; i < v1.size(); i ++){
		res.push_back(v1[i] - v2[i]);
	}
	return res;
}

// tuve que ponerlo acá porque el otro es privado
double distanciaVector(vector<double> v1, vector<double> v2 ){
	std::vector<double> v3 = restaBetweenVectores(v1,v2);
	sort(v3.begin(), v3.end());
	
	double ac = 0 ;
	for(int i = 0 ; i < v3.size(); i ++){
		ac += v3[i]*v3[i];
	}
	return ac;
}

// quiero hacer ejemplos de juguete. Quiero crear imágenes como vectores de vectores
// así que voy a meter una modificación del Knn para testing.
// En vez de pasarle el vector<Image*>, voy a pasarle un vector<vector<double>> para representar
// a cada imagen de la base, y aparte (cuando cree mis "bases de ejemplo"), crearé un
// vector<string> que contenga las labels de cada imagen y se lo paso por parámetro
// donde la posición 0 de ese vector corresponde a la label de la imagen 0 por ejemplo
// con estas modificaciones puedo testear sin crear imágenes con nuestra clase imagen
string resolverKnnTest(vector<vector<double> >& MatrizDatos, vector<vector<double> >& imagenes, vector<string> labels, vector<double> z, int k){
	
	// calculo distancias de z a cada una de la base y las inserto en un multimap
	// el significado será el número de la imagen (es necesario pues se ordenan todas en el multimap)
	multimap<double,int> distancias;
	for(int i = 0; i < MatrizDatos.size(); i ++){
  		distancias.insert(pair<double,int>(distanciaVector(z,MatrizDatos[i]), i));
	}

	// NO VA ESTO, DEVUELVE TODO AL REVÉS. TENGO QUE USAR UN ITERADOR COMÚN. El multimap está ordenado de menor a mayor, a mí me interesan los k más cercanos, no los k más lejanos.
	// multimap<double, int>::reverse_iterator it = distancias.rbegin();

	multimap<double, int>::iterator itComun = distancias.begin();

	// while(itComun != distancias.end()){
	// 	cout << itComun->first;
	// 	cout << ", ";
	// 	cout << itComun->second << endl;
	// 	itComun++;
	// }

	// while(it != distancias.rend()){
	// 	cout << it->first;
	// 	cout << ", ";
	// 	cout << it->second << endl;
	// 	it++;
	// }

	// armo vector de votaciones sólo considerando los k más cercanos
	// votaciones[i] es la cantidad de votos que tuvo la imagen i
	// en realidad votaciones[i] es 0 y 1, ya que o está entre las k más cercanas o no lo está.
	int vecinos = 0;
 	vector<int> votaciones (MatrizDatos.size(),0);
 	while(itComun != distancias.end() && vecinos < k ){
  		votaciones[itComun->second] += 1;
		vecinos++;
  		itComun++;
 	}

 	// mostrarVectorInt(votaciones);

 	map<string,int> votacionDeLabel;

 	//hago diccionario key nombre de la imagen, value cantidad de votos
 	for(int i = 0 ; i < votaciones.size(); i ++){
 		if(votaciones[i] != 0 ){
 			// si entramos a este caso, significa que la imagen i está entre las k más cercanas
 			if(votacionDeLabel.find(labels[i]) == votacionDeLabel.end()){
 				votacionDeLabel.insert(pair<string,int> (labels[i], 1));	
 			}
 			else{
 				map<string,int>::iterator currentIt = votacionDeLabel.find(labels[i]);
 				currentIt -> second += 1;
 			}	
 		}
 	}

 	// recorro votacionDeLabel para quedarme con el de mayor repeticiones dentro de los k + cercanos
 	// en otras palabras, me quedo con la moda del conjunto
 	map<string,int>::iterator itRes = votacionDeLabel.begin();
 	for(map<string,int>::iterator it = votacionDeLabel.begin(); it != votacionDeLabel.end() ; it ++){
 		if(it -> second > itRes -> second){
 			itRes = it;
 		}
 	}

 	return itRes->first;
}


//Idea Test: k=1, devuelve la imagen que mayor reduzca la diferencia.
void testearKnn1(){
	vector<vector<double> > testMatrix(5, vector<double>(5, 0.0));
	testMatrix[0][0] = 1.0;
	testMatrix[0][1] = 2.0;
	testMatrix[0][2] = 3.0;
	testMatrix[0][3] = 4.0;
	testMatrix[0][4] = 5.0;

	testMatrix[1][0] = 2.0;
	testMatrix[1][1] = 3.0;
	testMatrix[1][2] = 4.0;
	testMatrix[1][3] = 5.0;
	testMatrix[1][4] = 6.0;

	testMatrix[2][0] = 3.0;
	testMatrix[2][1] = 4.0;
	testMatrix[2][2] = 5.0;
	testMatrix[2][3] = 6.0;
	testMatrix[2][4] = 7.0;

	testMatrix[3][0] = 4.0;
	testMatrix[3][1] = 5.0;
	testMatrix[3][2] = 6.0;
	testMatrix[3][3] = 7.0;
	testMatrix[3][4] = 8.0;

	testMatrix[4][0] = 5.0;
	testMatrix[4][1] = 6.0;
	testMatrix[4][2] = 7.0;
	testMatrix[4][3] = 8.0;
	testMatrix[4][4] = 9.0;

	vector<string> labels(5, "");
	labels[0] = "pedro";
	labels[1] = "carlos";
	labels[2] = "juan";
	labels[3] = "juan";
	labels[4] = "carlos";

	// resuelvo kNN con k = 1 y z = [6,7,8,9,10]. El resultado debiera ser "carlos", que es el label de la última imagen
	vector<double> imgAReconocer(5, 0.0);
	imgAReconocer[0] = 6.0;
	imgAReconocer[1] = 7.0;
	imgAReconocer[2] = 8.0;
	imgAReconocer[3] = 9.0;
	imgAReconocer[4] = 10.0;

	string res = resolverKnnTest(testMatrix, testMatrix, labels, imgAReconocer, 1);
	cout << res << endl;

	// PASÓ OK LUEGO DE LAS MODIFICACIONES, DEVUELVE LO QUE TIENE QUE DEVOLVER ("carlos")
}

//Idea Test: Probar con 3 vecinos, dos de ellos con la misma etiqueta. Devolver esa.
void testearKnn2(){
	vector<vector<double> > testMatrix(5, vector<double>(5, 0.0));
	testMatrix[0][0] = 1.0;
	testMatrix[0][1] = 2.0;
	testMatrix[0][2] = 3.0;
	testMatrix[0][3] = 4.0;
	testMatrix[0][4] = 5.0;

	testMatrix[1][0] = 2.0;
	testMatrix[1][1] = 3.0;
	testMatrix[1][2] = 4.0;
	testMatrix[1][3] = 5.0;
	testMatrix[1][4] = 6.0;

	testMatrix[2][0] = 3.0;
	testMatrix[2][1] = 4.0;
	testMatrix[2][2] = 5.0;
	testMatrix[2][3] = 6.0;
	testMatrix[2][4] = 7.0;

	testMatrix[3][0] = 4.0;
	testMatrix[3][1] = 5.0;
	testMatrix[3][2] = 6.0;
	testMatrix[3][3] = 7.0;
	testMatrix[3][4] = 8.0;

	testMatrix[4][0] = 5.0;
	testMatrix[4][1] = 6.0;
	testMatrix[4][2] = 7.0;
	testMatrix[4][3] = 8.0;
	testMatrix[4][4] = 9.0;

	vector<string> labels(5, "");
	labels[0] = "pedro";
	labels[1] = "carlos";
	labels[2] = "juan";
	labels[3] = "juan";
	labels[4] = "carlos";

	// resuelvo kNN con k = 3 y z = [6,7,8,9,10]. El resultado debiera ser "juan", pues las 3 más cercanas serán las últimas 3 ("juan", "juan" y "carlos")
	vector<double> imgAReconocer(5, 0.0);
	imgAReconocer[0] = 6.0;
	imgAReconocer[1] = 7.0;
	imgAReconocer[2] = 8.0;
	imgAReconocer[3] = 9.0;
	imgAReconocer[4] = 10.0;

	string res = resolverKnnTest(testMatrix, testMatrix, labels, imgAReconocer, 3);
	cout << res << endl;

	// PASÓ OK LUEGO DE LAS MODIFICACIONES, DEVUELVE LO QUE TIENE QUE DEVOLVER ("juan").
}

//Idea Test: Mismo test1
void testearKnn3(){
	vector<vector<double> > testMatrix(5, vector<double>(5, 0.0));
	testMatrix[0][0] = 1.0;
	testMatrix[0][1] = 2.0;
	testMatrix[0][2] = 3.0;
	testMatrix[0][3] = 4.0;
	testMatrix[0][4] = 5.0;

	testMatrix[1][0] = 2.0;
	testMatrix[1][1] = 3.0;
	testMatrix[1][2] = 4.0;
	testMatrix[1][3] = 5.0;
	testMatrix[1][4] = 6.0;

	testMatrix[2][0] = 3.0;
	testMatrix[2][1] = 4.0;
	testMatrix[2][2] = 5.0;
	testMatrix[2][3] = 6.0;
	testMatrix[2][4] = 7.0;

	testMatrix[3][0] = 4.0;
	testMatrix[3][1] = 5.0;
	testMatrix[3][2] = 6.0;
	testMatrix[3][3] = 7.0;
	testMatrix[3][4] = 8.0;

	testMatrix[4][0] = 5.0;
	testMatrix[4][1] = 6.0;
	testMatrix[4][2] = 7.0;
	testMatrix[4][3] = 8.0;
	testMatrix[4][4] = 9.0;

	vector<string> labels(5, "");
	labels[0] = "pedro";
	labels[1] = "carlos";
	labels[2] = "juan";
	labels[3] = "juan";
	labels[4] = "carlos";

	// resuelvo kNN con k = 1 y z = [0.5,1,2,3,4]. El resultado debiera ser "pedro" que es la imagen más cercana (la primera)
	vector<double> imgAReconocer(5, 0.0);
	imgAReconocer[0] = 0.5;
	imgAReconocer[1] = 1.0;
	imgAReconocer[2] = 2.0;
	imgAReconocer[3] = 3.0;
	imgAReconocer[4] = 4.0;

	string res = resolverKnnTest(testMatrix, testMatrix, labels, imgAReconocer, 1);
	cout << res << endl;

	// PASÓ OK LUEGO DE LAS MODIFICACIONES, DEVUELVE LO QUE TIENE QUE DEVOLVER ("pedro").
}

//Idea test: la imagen z pertenece al conjunto de testeo. Deberia devolver su etiqueta. (ver que esto NO sirve para experimentar en el informe)
void testearKnn4(){
	vector<vector<double> > testMatrix(5, vector<double>(5, 0.0));
	testMatrix[0][0] = 1.0;
	testMatrix[0][1] = 2.0;
	testMatrix[0][2] = 3.0;
	testMatrix[0][3] = 4.0;
	testMatrix[0][4] = 5.0;

	testMatrix[1][0] = 2.0;
	testMatrix[1][1] = 3.0;
	testMatrix[1][2] = 4.0;
	testMatrix[1][3] = 5.0;
	testMatrix[1][4] = 6.0;

	testMatrix[2][0] = 3.0;
	testMatrix[2][1] = 4.0;
	testMatrix[2][2] = 5.0;
	testMatrix[2][3] = 6.0;
	testMatrix[2][4] = 7.0;

	testMatrix[3][0] = 4.0;
	testMatrix[3][1] = 5.0;
	testMatrix[3][2] = 6.0;
	testMatrix[3][3] = 7.0;
	testMatrix[3][4] = 8.0;

	testMatrix[4][0] = 5.0;
	testMatrix[4][1] = 6.0;
	testMatrix[4][2] = 7.0;
	testMatrix[4][3] = 8.0;
	testMatrix[4][4] = 9.0;

	vector<string> labels(5, "");
	labels[0] = "pedro";
	labels[1] = "carlos";
	labels[2] = "juan";
	labels[3] = "juan";
	labels[4] = "carlos";

	// resuelvo kNN con k = 1 y z = [3,4,5,6,7]. El resultado debiera ser "juan" pues la imagen a reconocer es idéntica a testMatrix[2]
	vector<double> imgAReconocer(5, 0.0);
	imgAReconocer[0] = 3.0;
	imgAReconocer[1] = 4.0;
	imgAReconocer[2] = 5.0;
	imgAReconocer[3] = 6.0;
	imgAReconocer[4] = 7.0;

	string res = resolverKnnTest(testMatrix, testMatrix, labels, imgAReconocer, 1);
	cout << res << endl;

	// PASÓ OK LUEGO DE LAS MODIFICACIONES, DEVUELVE LO QUE TIENE QUE DEVOLVER ("juan").
}

//Idea Test: Tomo dos vecinos, con etiquetas distintas. Deberia devolver alguna de las dos.
//Corolario: No importa el orden de las mismas, siempre devolvera la misma etiqueta en estos casos.
void testearKnn5(){
	vector<vector<double> > testMatrix(5, vector<double>(5, 0.0));
	testMatrix[0][0] = 1.0;
	testMatrix[0][1] = 2.0;
	testMatrix[0][2] = 3.0;
	testMatrix[0][3] = 4.0;
	testMatrix[0][4] = 5.0;

	testMatrix[1][0] = 2.0;
	testMatrix[1][1] = 3.0;
	testMatrix[1][2] = 4.0;
	testMatrix[1][3] = 5.0;
	testMatrix[1][4] = 6.0;

	testMatrix[2][0] = 13.0;
	testMatrix[2][1] = 14.0;
	testMatrix[2][2] = 15.0;
	testMatrix[2][3] = 16.0;
	testMatrix[2][4] = 17.0;

	testMatrix[3][0] = 14.0;
	testMatrix[3][1] = 15.0;
	testMatrix[3][2] = 16.0;
	testMatrix[3][3] = 17.0;
	testMatrix[3][4] = 18.0;

	testMatrix[4][0] = 15.0;
	testMatrix[4][1] = 16.0;
	testMatrix[4][2] = 17.0;
	testMatrix[4][3] = 18.0;
	testMatrix[4][4] = 19.0;

	vector<string> labels(5, "");
	labels[0] = "pedro";
	labels[1] = "carlos";
	labels[2] = "juan";
	labels[3] = "juan";
	labels[4] = "carlos";

	// resuelvo kNN con k = 2 y z = [6,7,8,9,10]. El resultado debiera ser "pedro" o "carlos", pues estas son las dos mas cercanas.

	vector<double> imgAReconocer(5, 0.0);
	imgAReconocer[0] = 6.0;
	imgAReconocer[1] = 7.0;
	imgAReconocer[2] = 8.0;
	imgAReconocer[3] = 9.0;
	imgAReconocer[4] = 10.0;

	string res = resolverKnnTest(testMatrix, testMatrix, labels, imgAReconocer, 2);
	cout << res << endl;

	// PASÓ OK LUEGO DE LAS MODIFICACIONES, DEVUELVE LO QUE TIENE QUE DEVOLVER ("carlos").
	
}


int main(){
	//testearKnn1();
	//testearKnn2();
	//testearKnn3();
	//testearKnn4();
	testearKnn5();
	return 0;
}