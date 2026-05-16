#ifndef CARGADOR_H
#define CARGADOR_H

#include "estructuras.h"
#include <string>

// Carga masiva de capas desde archivo .cap
// Formato:
//   id {
//   fila,columna,color;
//   ...
//   }
bool cargarCapas(const std::string& archivo, ABBCapas& arbol);

// Carga masiva de imagenes desde archivo .im
// Formato: id{idcapa,...,idcapa}
bool cargarImagenes(const std::string& archivo, ListaCircularImagenes& lista, ABBCapas& arbCapas);

// Carga masiva de usuarios desde archivo .usr
// Formato: nombre:idimg,idimg,...;
bool cargarUsuarios(const std::string& archivo, ABBUsuarios& arbol, ListaCircularImagenes& listImg);

#endif
