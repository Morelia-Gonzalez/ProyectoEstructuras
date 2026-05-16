#include "estructuras.h"
#include <algorithm>

// ==================== MATRIZ DISPERSA ====================
void MatrizDispersa::insertar(int fila, int columna, const std::string& color) {
    if (fila > maxFila) maxFila = fila;
    if (columna > maxColumna) maxColumna = columna;

    // Buscar o crear fila
    NodoFila* prevF = nullptr;
    NodoFila* currF = filas;
    while (currF && currF->fila < fila) { prevF = currF; currF = currF->siguiente; }

    if (!currF || currF->fila != fila) {
        NodoFila* nueva = new NodoFila(fila);
        nueva->siguiente = currF;
        if (prevF) prevF->siguiente = nueva;
        else filas = nueva;
        currF = nueva;
    }

    // Buscar o crear columna en esa fila
    NodoColumna* prevC = nullptr;
    NodoColumna* currC = currF->columnas;
    while (currC && currC->columna < columna) { prevC = currC; currC = currC->siguiente; }

    if (!currC || currC->columna != columna) {
        NodoColumna* nueva = new NodoColumna(columna, color);
        nueva->siguiente = currC;
        if (prevC) prevC->siguiente = nueva;
        else currF->columnas = nueva;
    } else {
        currC->color = color; // actualizar
    }
}

std::string MatrizDispersa::obtener(int fila, int columna) const {
    NodoFila* currF = filas;
    while (currF && currF->fila < fila) currF = currF->siguiente;
    if (!currF || currF->fila != fila) return "";
    NodoColumna* currC = currF->columnas;
    while (currC && currC->columna < columna) currC = currC->siguiente;
    if (!currC || currC->columna != columna) return "";
    return currC->color;
}

void MatrizDispersa::liberar() {
    NodoFila* f = filas;
    while (f) {
        NodoColumna* c = f->columnas;
        while (c) { NodoColumna* tmp = c->siguiente; delete c; c = tmp; }
        NodoFila* tmpF = f->siguiente;
        delete f;
        f = tmpF;
    }
    filas = nullptr; maxFila = 0; maxColumna = 0;
}

// ==================== ABB CAPAS ====================
void ABBCapas::insertar(int id, const std::string& nombre) { insertar(raiz, id, nombre); }

void ABBCapas::insertar(Capa* &nodo, int id, const std::string& nombre) {
    if (!nodo) { nodo = new Capa(id, nombre); return; }
    if (id < nodo->id) insertar(nodo->izquierdo, id, nombre);
    else if (id > nodo->id) insertar(nodo->derecho, id, nombre);
    // si id == id ya existe, ignorar
}

Capa* ABBCapas::buscar(int id) const { return buscar(raiz, id); }

Capa* ABBCapas::buscar(Capa* nodo, int id) const {
    if (!nodo) return nullptr;
    if (id == nodo->id) return nodo;
    if (id < nodo->id) return buscar(nodo->izquierdo, id);
    return buscar(nodo->derecho, id);
}

void ABBCapas::liberar(Capa* nodo) {
    if (!nodo) return;
    liberar(nodo->izquierdo);
    liberar(nodo->derecho);
    nodo->matriz->liberar();
    delete nodo->matriz;
    delete nodo;
}

// ==================== LISTA CAPAS ====================
void ListaCapas::agregar(Capa* c) {
    NodoListaCapa* nuevo = new NodoListaCapa(c);
    if (!cabeza) { cabeza = nuevo; }
    else {
        NodoListaCapa* curr = cabeza;
        while (curr->siguiente) curr = curr->siguiente;
        curr->siguiente = nuevo;
    }
    cantidad++;
}

void ListaCapas::liberar() {
    NodoListaCapa* curr = cabeza;
    while (curr) { NodoListaCapa* tmp = curr->siguiente; delete curr; curr = tmp; }
    cabeza = nullptr; cantidad = 0;
}

// ==================== LISTA CIRCULAR DOBLE IMAGENES ====================
void ListaCircularImagenes::insertar(Imagen* img) {
    NodoImagen* nuevo = new NodoImagen(img);
    if (!cabeza) {
        nuevo->siguiente = nuevo;
        nuevo->anterior = nuevo;
        cabeza = nuevo;
    } else {
        // insertar ordenado por id
        NodoImagen* curr = cabeza;
        do {
            if (img->id < curr->imagen->id) {
                // insertar antes de curr
                NodoImagen* prev = curr->anterior;
                prev->siguiente = nuevo;
                nuevo->anterior = prev;
                nuevo->siguiente = curr;
                curr->anterior = nuevo;
                if (curr == cabeza) cabeza = nuevo;
                cantidad++;
                return;
            }
            curr = curr->siguiente;
        } while (curr != cabeza);
        // insertar al final
        NodoImagen* ultimo = cabeza->anterior;
        ultimo->siguiente = nuevo;
        nuevo->anterior = ultimo;
        nuevo->siguiente = cabeza;
        cabeza->anterior = nuevo;
    }
    cantidad++;
}

NodoImagen* ListaCircularImagenes::buscar(int id) const {
    if (!cabeza) return nullptr;
    NodoImagen* curr = cabeza;
    do {
        if (curr->imagen->id == id) return curr;
        curr = curr->siguiente;
    } while (curr != cabeza);
    return nullptr;
}

bool ListaCircularImagenes::eliminar(int id) {
    if (!cabeza) return false;
    NodoImagen* curr = cabeza;
    do {
        if (curr->imagen->id == id) {
            if (cantidad == 1) {
                delete curr;
                cabeza = nullptr;
            } else {
                curr->anterior->siguiente = curr->siguiente;
                curr->siguiente->anterior = curr->anterior;
                if (curr == cabeza) cabeza = curr->siguiente;
                delete curr;
            }
            cantidad--;
            return true;
        }
        curr = curr->siguiente;
    } while (curr != cabeza);
    return false;
}

void ListaCircularImagenes::liberar() {
    if (!cabeza) return;
    NodoImagen* curr = cabeza;
    cabeza->anterior->siguiente = nullptr; // romper ciclo
    while (curr) {
        NodoImagen* tmp = curr->siguiente;
        curr->imagen->capas.liberar();
        delete curr->imagen;
        delete curr;
        curr = tmp;
    }
    cabeza = nullptr; cantidad = 0;
}

// ==================== LISTA IMAGENES USUARIO ====================
void ListaImagenesUsuario::agregar(Imagen* img) {
    NodoImagenUsuario* nuevo = new NodoImagenUsuario(img);
    if (!cabeza) { cabeza = nuevo; return; }
    NodoImagenUsuario* curr = cabeza;
    while (curr->siguiente) curr = curr->siguiente;
    curr->siguiente = nuevo;
}

bool ListaImagenesUsuario::eliminar(int idImagen) {
    NodoImagenUsuario* prev = nullptr, *curr = cabeza;
    while (curr) {
        if (curr->imagen->id == idImagen) {
            if (prev) prev->siguiente = curr->siguiente;
            else cabeza = curr->siguiente;
            delete curr;
            return true;
        }
        prev = curr; curr = curr->siguiente;
    }
    return false;
}

void ListaImagenesUsuario::liberar() {
    NodoImagenUsuario* curr = cabeza;
    while (curr) { NodoImagenUsuario* tmp = curr->siguiente; delete curr; curr = tmp; }
    cabeza = nullptr;
}

// ==================== ABB USUARIOS ====================
void ABBUsuarios::insertar(const std::string& nombre) { insertar(raiz, nombre); }

void ABBUsuarios::insertar(Usuario* &nodo, const std::string& nombre) {
    if (!nodo) { nodo = new Usuario(nombre); return; }
    if (nombre < nodo->nombre) insertar(nodo->izquierdo, nombre);
    else if (nombre > nodo->nombre) insertar(nodo->derecho, nombre);
}

Usuario* ABBUsuarios::buscar(const std::string& nombre) const { return buscar(raiz, nombre); }

Usuario* ABBUsuarios::buscar(Usuario* nodo, const std::string& nombre) const {
    if (!nodo) return nullptr;
    if (nombre == nodo->nombre) return nodo;
    if (nombre < nodo->nombre) return buscar(nodo->izquierdo, nombre);
    return buscar(nodo->derecho, nombre);
}

Usuario* ABBUsuarios::minimo(Usuario* nodo) const {
    while (nodo->izquierdo) nodo = nodo->izquierdo;
    return nodo;
}

Usuario* ABBUsuarios::eliminar(Usuario* nodo, const std::string& nombre) {
    if (!nodo) return nullptr;
    if (nombre < nodo->nombre) nodo->izquierdo = eliminar(nodo->izquierdo, nombre);
    else if (nombre > nodo->nombre) nodo->derecho = eliminar(nodo->derecho, nombre);
    else {
        if (!nodo->izquierdo) {
            Usuario* tmp = nodo->derecho;
            nodo->imagenes.liberar();
            delete nodo;
            return tmp;
        } else if (!nodo->derecho) {
            Usuario* tmp = nodo->izquierdo;
            nodo->imagenes.liberar();
            delete nodo;
            return tmp;
        }
        Usuario* sucesor = minimo(nodo->derecho);
        nodo->nombre = sucesor->nombre;
        // mover lista de imágenes del sucesor
        nodo->imagenes.liberar();
        nodo->imagenes.cabeza = sucesor->imagenes.cabeza;
        sucesor->imagenes.cabeza = nullptr;
        nodo->derecho = eliminar(nodo->derecho, sucesor->nombre);
    }
    return nodo;
}

bool ABBUsuarios::eliminar(const std::string& nombre) {
    if (!buscar(nombre)) return false;
    raiz = eliminar(raiz, nombre);
    return true;
}

void ABBUsuarios::liberar(Usuario* nodo) {
    if (!nodo) return;
    liberar(nodo->izquierdo);
    liberar(nodo->derecho);
    nodo->imagenes.liberar();
    delete nodo;
}
