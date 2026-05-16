#ifndef ESTRUCTURAS_H
#define ESTRUCTURAS_H

#include <string>
#include <iostream>

// ==================== MATRIZ DISPERSA (Capa) ====================
// Nodo de columna en la matriz dispersa
struct NodoColumna {
    int columna;
    std::string color; // Hex color
    NodoColumna* siguiente;
    NodoColumna(int c, const std::string& col) : columna(c), color(col), siguiente(nullptr) {}
};

// Nodo de fila en la matriz dispersa
struct NodoFila {
    int fila;
    NodoColumna* columnas; // lista simplemente enlazada de columnas
    NodoFila* siguiente;
    NodoFila(int f) : fila(f), columnas(nullptr), siguiente(nullptr) {}
};

// Matriz dispersa simplemente enlazada
struct MatrizDispersa {
    NodoFila* filas;
    int maxFila;
    int maxColumna;
    MatrizDispersa() : filas(nullptr), maxFila(0), maxColumna(0) {}

    void insertar(int fila, int columna, const std::string& color);
    std::string obtener(int fila, int columna) const;
    void liberar();
};

// ==================== ARBOL BINARIO DE BUSQUEDA DE CAPAS ====================
struct Capa {
    int id;
    std::string nombre;
    MatrizDispersa* matriz;
    Capa* izquierdo;
    Capa* derecho;
    Capa(int id, const std::string& nombre) 
        : id(id), nombre(nombre), matriz(new MatrizDispersa()), izquierdo(nullptr), derecho(nullptr) {}
};

struct ABBCapas {
    Capa* raiz;
    ABBCapas() : raiz(nullptr) {}
    void insertar(int id, const std::string& nombre);
    Capa* buscar(int id) const;
    void insertar(Capa* &nodo, int id, const std::string& nombre);
    Capa* buscar(Capa* nodo, int id) const;
    void liberar(Capa* nodo);
    ~ABBCapas() { liberar(raiz); }
};

// ==================== LISTA DE CAPAS POR IMAGEN ====================
struct NodoListaCapa {
    Capa* capa; // apuntador al nodo en el ABB (no copia)
    NodoListaCapa* siguiente;
    NodoListaCapa(Capa* c) : capa(c), siguiente(nullptr) {}
};

struct ListaCapas {
    NodoListaCapa* cabeza;
    int cantidad;
    ListaCapas() : cabeza(nullptr), cantidad(0) {}
    void agregar(Capa* c);
    void liberar();
};

// ==================== IMAGEN ====================
struct Imagen {
    int id;
    ListaCapas capas;
};

// ==================== LISTA CIRCULAR DOBLEMENTE ENLAZADA DE IMAGENES ====================
struct NodoImagen {
    Imagen* imagen;
    NodoImagen* siguiente;
    NodoImagen* anterior;
    NodoImagen(Imagen* img) : imagen(img), siguiente(nullptr), anterior(nullptr) {}
};

struct ListaCircularImagenes {
    NodoImagen* cabeza;
    int cantidad;
    ListaCircularImagenes() : cabeza(nullptr), cantidad(0) {}
    void insertar(Imagen* img); // ordenada por id
    NodoImagen* buscar(int id) const;
    bool eliminar(int id);
    void liberar();
};

// ==================== LISTA DE IMAGENES DE USUARIO ====================
struct NodoImagenUsuario {
    Imagen* imagen; // apuntador a imagen en lista circular
    NodoImagenUsuario* siguiente;
    NodoImagenUsuario(Imagen* img) : imagen(img), siguiente(nullptr) {}
};

struct ListaImagenesUsuario {
    NodoImagenUsuario* cabeza;
    ListaImagenesUsuario() : cabeza(nullptr) {}
    void agregar(Imagen* img);
    bool eliminar(int idImagen);
    void liberar();
};

// ==================== USUARIO ====================
struct Usuario {
    std::string nombre;
    ListaImagenesUsuario imagenes;
    Usuario* izquierdo;
    Usuario* derecho;
    Usuario(const std::string& n) : nombre(n), izquierdo(nullptr), derecho(nullptr) {}
};

// ==================== ABB USUARIOS ====================
struct ABBUsuarios {
    Usuario* raiz;
    ABBUsuarios() : raiz(nullptr) {}
    void insertar(const std::string& nombre);
    Usuario* buscar(const std::string& nombre) const;
    bool eliminar(const std::string& nombre);
    void insertar(Usuario* &nodo, const std::string& nombre);
    Usuario* buscar(Usuario* nodo, const std::string& nombre) const;
    Usuario* eliminar(Usuario* nodo, const std::string& nombre);
    Usuario* minimo(Usuario* nodo) const;
    void liberar(Usuario* nodo);
    ~ABBUsuarios() { liberar(raiz); }
};

#endif
