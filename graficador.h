#ifndef GRAFICADOR_H
#define GRAFICADOR_H

#include "estructuras.h"
#include <string>
#include <vector>

// Genera imagen PNG a partir de un archivo .dot usando Graphviz
void generarPNG(const std::string& archivoDot, const std::string& archivoPNG);

// Grafica la matriz dispersa de una capa
void graficarMatrizDispersa(const Capa* capa, const std::string& salida);

// Grafica el ABB de capas
void graficarABBCapas(const ABBCapas& abb, const std::string& salida);

// Grafica la lista circular de imagenes con sus listas de capas
void graficarListaImagenes(const ListaCircularImagenes& lista, const std::string& salida);

// Grafica el ABB de usuarios con sus listas de imagenes
void graficarABBUsuarios(const ABBUsuarios& abb, const std::string& salida);

// Grafica imagen+arbol de capas (con enlaces desde lista hacia ABB)
void graficarImagenYArbol(const NodoImagen* nodoImg, const ABBCapas& abb, const std::string& salida);

// Genera imagen final (canvas de pixeles) combinando capas
void generarImagenPixeles(const std::vector<Capa*>& capas, const std::string& salida);

#endif
