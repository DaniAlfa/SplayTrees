/**
* Implementacion de SplayTree/Arbol Biselado
*
* Copyright (c) 2020 Daniel Alfaro Miranda
*/
#ifndef SPLAY_TREE
#define SPLAY_TREE
#include <functional>
#include <utility>
#include <tuple>

template<class T, class Comp = std::less<T>>
class splay {
protected:

	using splay_t = splay<T, Comp>;

	struct arbol;
	using pArbol = arbol *;
	struct arbol {
		arbol(T const& e) : raiz(e), izq(nullptr), der(nullptr)  {};
		arbol(pArbol l, T const& e, pArbol r) : raiz(e), izq(l), der(r)  {};
		T raiz;
		pArbol izq, der;
	};

	pArbol pArbolPrincipal;

	int nElementos; //Numero de elementos

	Comp menor; //Comparador, por defecto el menor

	//Estructura que contiene lo necesario para identificar, la posicion de la raiz origen de la flotacion y el tipo de flotacion a aplicar
	struct posicionOrigenFlotacion { 
		posicionOrigenFlotacion(int diff = 0, int dir1Izq = false, int dir2Izq = false) : diff(diff), dir1Izq(dir1Izq), dir2Izq(dir2Izq) {}
		int diff;  //Diferencia de altura relativa entre el nodo actual y el origen de la flotacion.
		bool dir1Izq; //Indica la direccion mas proxima hacia el hijo con el nodo origen. True si el origen esta en el arbol izquierdo, false en el derecho.
		bool dir2Izq; //Indica la siguiente direccion mas proxima hacia el hijo con el nodo origen.
		/*
		 Solo son necesarias dos direcciones para identificar al nodo, puesto que, la rotacion se hara como maximo por el abuelo del nodo origen
		 -Si la diferecia de altura es 1 y el nodo actual del recorrido es la raiz, se utilizara la dir1Izq para identificar si el nodo a flotar esta 
		  a la izquierda o la derecha
		 -Si la diferencia es 2, el nodo actual del recorrido es el abuelo del nodo a flotar, y se utilizaran los dos booleanos para identificar al nodo
		*/
	};

public:

	splay(Comp c = Comp()) : pArbolPrincipal(nullptr), nElementos(0), menor(c) {}

	splay(splay_t const& otro) { copia(otro); }

	splay_t & operator=(splay_t const& otro) {
		if (*this != otro) {
			liberaMem(pArbolPrincipal);
			copia(otro);
		}
		return *this;
	}

	~splay() { liberaMem(pArbolPrincipal); }

	bool es_vacio() const { return pArbolPrincipal == nullptr; }

	int cardinal() const { return nElementos; }

	bool buscar(T const& elem){
		return buscar(elem, pArbolPrincipal).first;
	}

	bool insertar(T const& elem) {
		return insertar(elem, pArbolPrincipal).first;
	}

	bool borrar(T const& elem) { 
		return borrar(elem, pArbolPrincipal).first;
	}


protected:


	void copia(splay_t const& otro) {
		pArbolPrincipal = copia(otro.pArbolPrincipal);
		nElementos = otro.nElementos;
		menor = otro.menor;
	}

	static pArbol copia(pArbol t) {
		return ((t == nullptr) ? nullptr : new arbol(copia(t->izq), t->elem, copia(t->der)));
	}

	static void liberaMem(pArbol t) {
		if (t != nullptr) {
			liberaMem(t->izq);
			liberaMem(t->der);
			delete t;
		}
	}

	std::pair<bool,posicionOrigenFlotacion> buscar(T const& elem, pArbol & t){
		bool encontrado;
		posicionOrigenFlotacion pos;
		if (t == nullptr) return { false, {0} }; //Si el arbol es vacio el nodo a flotar es el anterior (al retornar, diferencia relativa 0)
		else if (menor(elem, t->raiz)) {        // elem < raiz
			std::tie(encontrado, pos) = buscar(elem, t->izq);
			pos.dir1Izq = true;
		}
		else if (menor(t->raiz, elem)) {        //elem > raiz
			std::tie(encontrado, pos) = buscar(elem, t->der);
			pos.dir1Izq = false;
		}
		else return { true, {1} }; //elem == raiz Se encuentra el nodo, el nodo a flotar es el actual (al retornar, diferencia relativa 1)

		flotarNodo(t, pos); //Se actualiza la posicion del nodo origen y si es el caso se hace la flotacion
		return { encontrado, pos };
	}

	std::pair<bool, posicionOrigenFlotacion> insertar(T const& elem, pArbol & t) {
		bool insertado;
		posicionOrigenFlotacion pos;
		if (t == nullptr) {
			t = new arbol(elem);
			++nElementos;
			return { true, {1} }; //El nodo a flotar es el insertado
		}
		else if (menor(elem, t->raiz)) {
			std::tie(insertado, pos) = insertar(elem, t->izq);
			pos.dir1Izq = true;
		}
		else if (menor(t->raiz, elem)) {
			std::tie(insertado, pos) = insertar(elem, t->der);
			pos.dir1Izq = false;
		}
		else return { false, {1} }; //Elemento encontrado, el nodo a flotar el es el encontrado

		flotarNodo(t, pos);
		return { insertado, pos };
	}

	std::pair<bool, posicionOrigenFlotacion> borrar(T const& elem, pArbol & t) {
		bool borrado;
		posicionOrigenFlotacion pos;
		if (t == nullptr) return { false, {0} };  //No se encuentra el nodo a borrar, el nodo a flotar es el anterior
		else if (menor(elem, t->raiz)) {
			std::tie(borrado, pos) = borrar(elem, t->izq);
			pos.dir1Izq = true;
		}
		else if (menor(t->raiz, elem)) {
			std::tie(borrado, pos) = borrar(elem, t->der);
			pos.dir1Izq = false;
		}
		else { //El nodo a borrar se encuentra
			if (t->izq == nullptr || t->der == nullptr) { //Si alguno de sus hijos es vacio se sustituye el nodo por uno de ellos
				pArbol aux = t;
				t = (t->izq == nullptr) ? t->der : t->izq;
				delete aux;
			}
			else borrarMinHijoDerecho(t); //Si no, se borra el menor del hijo derecho y se pone como nueva raiz del arbol t
			--nElementos;
			return { true, {0} }; //El nodo a flotar es el padre del borrado
		}

		flotarNodo(t, pos);
		return { borrado, pos };
	}

	// Actualiza la posicion del nodo origen y si es el caso se hace la flotacion
	// Precondicion: t es un subarbol no vacio y la posicion identifica correctamente a un nodo
	void flotarNodo(pArbol & t, posicionOrigenFlotacion & pos) {
		if (pos.diff == 1 && t == this->pArbolPrincipal) { //Si la diferencia es 1 y el subarbol es el arbol pricipal de la estructura se intercambian los nodos
			if (pos.dir1Izq) rotarDerecha(t); //LL de AVL (rotacion derecha)
			else rotarIzquierda(t); //RR de AVL
			pos.diff = 0;
		}
		else if (pos.diff == 2) { //Si la diferencia es 2 se intercambia el abuelo por el nodo origen de la flotacion (indicado por pos)
			if (pos.dir1Izq && pos.dir2Izq) { //LL de splay
				rotarDerecha(t);
				rotarDerecha(t);
			}
			else if (!pos.dir1Izq && !pos.dir2Izq) { //RR de splay
				rotarIzquierda(t);
				rotarIzquierda(t);
			}
			else if (!pos.dir1Izq && pos.dir2Izq) { //RL de splay
				rotarDerecha(t->der);
				rotarIzquierda(t);
			}
			else { //LR de splay
				rotarIzquierda(t->izq);
				rotarDerecha(t);
			}
			pos.diff = 0;
		}
		pos.dir2Izq = pos.dir1Izq;
		++pos.diff;
	}

	//Rotacion a la derecha sobre el nodo t, la misma aplicada en desequilibrio LL para AVL,s
	static void rotarDerecha(pArbol & t) {
		pArbol aux = t->izq;
		t->izq = t->izq->der;
		aux->der = t;
		t = aux;
	}

	//Rotacion a la izquierda sobre el nodo t, la misma aplicada en desequilibrio RR para AVL,s
	static void rotarIzquierda(pArbol & t) {
		pArbol aux = t->der;
		t->der = t->der->izq;
		aux->izq = t;
		t = aux;
	}

	static void borrarMinHijoDerecho(pArbol & t) { //Borra el minimo del subarbol derecho y lo convierte en la nueva raiz del arbol t
		pArbol min = t->der, padre = nullptr;
		while (min->izq != nullptr) { //Se va bajando por la izquierda hasta encontrar el menor del hijo derecho
			padre = min;
			min = min->izq;
		}
		if (padre != nullptr) { //Si es el siguiente al que vas a eliminar no hace falta subir el menor ni guardar los mayores del menor en el padre
			padre->izq = min->der; //Los mayores al menor se guardan en la izquierda del padre
			min->der = t->der; //Al menor se le ponen como mayores el primer hijo del nodo a borrar
		}
		min->izq = t->izq; //Al menor se le asigna la parte izquierda del nodo a borrar
		delete t;
		t = min;
	}
};
#endif // SPLAY_TREE