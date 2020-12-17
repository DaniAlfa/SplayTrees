/**
* Programa para testear la eficiencia asintotica de las operaciones Insertar, Borrar y Buscar de un SplayTree 
*
* Para generar casos de prueba voluminosos y probar los tiempos de las funciones se insertan n?meros aleatorios en el ?rbol y, en cada intervalo 
* de magnitud de elementos, se realiza una prueba de tiempos de cada instrucci?n. Esta prueba consiste en un numero de medidas de tiempos de ejecuci?n y en una media de estas.
* 
* Al ejecutar el programa se le pueden pasar las siguientes opciones para alterar su funcionamiento:
* • -f : Nombre de archivo destino (por defecto default Test)
* • -NE : Total de elementos insertados en el ?rbol, como condici?n de terminaci?n del programa. (por defecto 20000000).
* • -MIN y -MAX : N?meros para definir el rango en la generaci?n de los n?meros aleatorios (por defecto [0, 50000000]).
* 
* Los resultados se dividen en un archivo para cada operacion, en los que, en cada linea, se escribe el cardinal del arbol en el momento de la operacion y el tiempo en realizarla.
* Estos archivos se pueden visualizar graficamente con gnuplot.
*
* Copyright (c) 2020 Daniel Alfaro Miranda
*/
#include <iostream>
#include <fstream>
#include <chrono>
#include <string>
#include <cstring>
#include <random>

#include "splayTree.h"

#define RANGO_MINIMO_GENERACION 0 // Rangos maximo y minimo para los numeros a introducir en el arbol
#define RANGO_MAXIMO_GENERACION 50000000
#define ELEMENTOS_TOTALES_ARBOL 20000000 //Limite de elementos a insertar en el arbol
#define INTERVALO_TEST 730000  //Intervalo en el que se realiza un test de los tiempos de las funciones
#define NUMERO_MEDIDAS 1000  //Numero de medidas por cada test de tiempo de una funcion

using namespace std;

void testInsertar(splay<long int> &sp, std::ofstream & out, std::function<long int()> & randGen); //randGen es una funcion que devuelve un numero aleatorio
void testBorrar(splay<long int> &sp, std::ofstream & out, std::function<long int()> & randGen);
void testBuscar(splay<long int> &sp, std::ofstream & out, std::function<long int()> & randGen);
void realizarTestArbol(splay<long int> &sp, std::function<long int()> & randGen);

void parseArgs(int argc, char* argv[]); //Funcion que analiza los argumentos al programa

std::mt19937 seed(chrono::system_clock::now().time_since_epoch().count()); //Semilla para generacion de numeros aleatorios
long int rangoMinGen, rangoMaxGen, totalElementosArbol; //variables para el rango minimo y maximo de los numero aleatorios generados y los elementos a insertar en el arbol
std::string nombreArch; //Nombre de archivo de salida



int main(int argc, char* argv[]) {
	parseArgs(argc, argv);
	uniform_int_distribution<long int>* randomGen = new uniform_int_distribution<long int>(rangoMinGen, rangoMaxGen);
	function<long int()> genRandomInt = [randomGen](){return (*randomGen)(seed);};
	splay<long int>* arbolSplay = new splay<long int>();
	
	realizarTestArbol(*arbolSplay, genRandomInt);

	delete randomGen;
	delete arbolSplay;
	return 0;
}



void realizarTestArbol(splay<long int> &sp, std::function<long int()> & randGen){
	ofstream buscarM(nombreArch + "_buscar.txt", fstream::trunc);
	ofstream insertarM(nombreArch + "_insertar.txt", fstream::trunc);
	ofstream borrarM(nombreArch + "_borrar.txt", fstream::trunc);

	while (sp.cardinal() <= totalElementosArbol) {
		if (sp.cardinal() % INTERVALO_TEST == 0) {
			testBuscar(sp, buscarM, randGen);
			testInsertar(sp, insertarM, randGen);
			testBorrar(sp, borrarM, randGen);
		}
		sp.insertar(randGen());
	}

	buscarM.close();
	insertarM.close();
	borrarM.close();
}


void testInsertar(splay<long int> &sp, std::ofstream & out, std::function<long int()> & randGen) {
	std::chrono::duration<double, std::milli> duracion;
	int medidas = NUMERO_MEDIDAS;
	double tiempoTranscurrido = 0;
	long int randNum;

	while (medidas-- > 0) {
		randNum = randGen();
		auto inicio = chrono::high_resolution_clock::now();
		sp.insertar(randNum);
		auto fin = chrono::high_resolution_clock::now();
		duracion = fin - inicio;
		tiempoTranscurrido += duracion.count();
	}

	tiempoTranscurrido /= NUMERO_MEDIDAS;

	out << sp.cardinal() << " " << tiempoTranscurrido << endl;
}

void testBorrar(splay<long int> &sp, std::ofstream & out, std::function<long int()> & randGen) {
	std::chrono::duration<double, std::milli> duracion;
	int medidas = NUMERO_MEDIDAS;
	double tiempoTranscurrido = 0;
	long int randNum;

	while (medidas-- > 0) {
		randNum = randGen();
		auto inicio = chrono::high_resolution_clock::now();
		sp.borrar(randNum);
		auto fin = chrono::high_resolution_clock::now();
		duracion = fin - inicio;
		tiempoTranscurrido += duracion.count();
	}

	tiempoTranscurrido /= NUMERO_MEDIDAS;

	out << sp.cardinal() << " " << tiempoTranscurrido << endl;
}


void testBuscar(splay<long int> &sp, std::ofstream & out, std::function<long int()> & randGen) {
	std::chrono::duration<double, std::milli> duracion;
	int medidas = NUMERO_MEDIDAS;
	double tiempoTranscurrido = 0;
	long int randNum;

	while (medidas-- > 0) {
		randNum = randGen();
		auto inicio = chrono::high_resolution_clock::now();
		sp.buscar(randNum);
		auto fin = chrono::high_resolution_clock::now();
		duracion = fin - inicio;
		tiempoTranscurrido += duracion.count();
	}

	tiempoTranscurrido /= NUMERO_MEDIDAS;

	out << sp.cardinal() << " " << tiempoTranscurrido << endl;
}



void parseArgs(int argc, char* argv[]){
	rangoMinGen = RANGO_MINIMO_GENERACION; //Se asignan los parametros por defecto 
	rangoMaxGen = RANGO_MAXIMO_GENERACION; 
	totalElementosArbol = ELEMENTOS_TOTALES_ARBOL;
	nombreArch = "testPorDefecto";

	if(argc != 1){
		if(argc == 2 && strcmp(argv[1], "-h") == 0) {
			cout << "Uso: " << argv[0] << " <<-f nombre_archivo>> <<-NE long int>> (limite de elementos en el arbol) <<-MIN long int>> <<-MAX long int>> (rango maximo y minimo en los numeros aleatorios)" << endl;
			exit(0);
		}
		if(argc % 2 == 0){
			cerr << "Error: Numero de parametros incorrecto" << endl;
			cout << "Uso: " << argv[0] << " <<-f nombre_archivo>> <<-NE long int>> (limite de elementos en el arbol) <<-MIN long int>> <<-MAX long int>> (rango maximo y minimo en los numeros aleatorios)" << endl;
			exit(-1);
		}
		for(int i = 1; i < argc; i = i + 2){
			if(strcmp(argv[i], "-f") == 0) nombreArch = argv[i + 1];
			else if(strcmp(argv[i], "-NE") == 0) totalElementosArbol = atol(argv[i + 1]);
			else if(strcmp(argv[i], "-MIN") == 0) rangoMinGen = atol(argv[i + 1]);
			else if(strcmp(argv[i], "-MAX") == 0) rangoMaxGen = atol(argv[i + 1]);
			else {
				cerr << "Error: Opciones de programa incorrectas" << argv[i] <<endl;
				exit(-1);
			}
		}
	}
	if(rangoMinGen >= rangoMaxGen || totalElementosArbol > rangoMaxGen - rangoMinGen){
		cerr << "Error: Numero de parametros incorrecto" << endl;
		exit(-1);
	}
}
