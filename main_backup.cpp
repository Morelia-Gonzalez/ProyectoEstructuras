#include <iostream>
#include <string>
#include <vector>
#include <limits>
#include <algorithm>
#include <fstream>
#include <sstream>
#include "estructuras.h"
#include "graficador.h"
#include "cargador.h"

// ==================== GLOBALS ====================
ABBCapas arbolCapas;
ListaCircularImagenes listaImagenes;
ABBUsuarios arbolUsuarios;
std::string dirGraficas = "graficas/";

// ==================== UTILIDADES UI ====================
void limpiarBuffer() {
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
}

void pausar() {
    std::cout << "\n  Presione Enter para continuar...";
    std::cin.get();
}

void encabezado(const std::string& titulo) {
    std::cout << "\n===================================================\n";
    std::cout << "  " << titulo << "\n";
    std::cout << "===================================================\n";
}

// ==================== RECORRIDOS ABB PARA GENERACION ====================
static void preorden(Capa* nodo, std::vector<Capa*>& resultado, int limite) {
    if (!nodo || (int)resultado.size() >= limite) return;
    resultado.push_back(nodo);
    preorden(nodo->izquierdo, resultado, limite);
    preorden(nodo->derecho, resultado, limite);
}

static void inorden(Capa* nodo, std::vector<Capa*>& resultado, int limite) {
    if (!nodo || (int)resultado.size() >= limite) return;
    inorden(nodo->izquierdo, resultado, limite);
    if ((int)resultado.size() < limite) resultado.push_back(nodo);
    inorden(nodo->derecho, resultado, limite);
}

static void postorden(Capa* nodo, std::vector<Capa*>& resultado, int limite) {
    if (!nodo || (int)resultado.size() >= limite) return;
    postorden(nodo->izquierdo, resultado, limite);
    postorden(nodo->derecho, resultado, limite);
    if ((int)resultado.size() < limite) resultado.push_back(nodo);
}

// ==================== IMPRIMIR LISTA CAPAS ====================
static void imprimirListaCapas(const ListaCapas& lc) {
    NodoListaCapa* curr = lc.cabeza;
    while (curr) {
        std::cout << "    -> Capa ID=" << curr->capa->id << " (" << curr->capa->nombre << ")\n";
        curr = curr->siguiente;
    }
}

// ==================== IMPRIMIR LISTA CIRCULAR ====================
static void imprimirListaCircular() {
    if (!listaImagenes.cabeza) { std::cout << "  Lista vacia.\n"; return; }
    NodoImagen* curr = listaImagenes.cabeza;
    do {
        std::cout << "  [Imagen ID=" << curr->imagen->id << "] Capas: ";
        NodoListaCapa* nc = curr->imagen->capas.cabeza;
        if (!nc) std::cout << "(ninguna)";
        while (nc) {
            std::cout << nc->capa->id;
            if (nc->siguiente) std::cout << ",";
            nc = nc->siguiente;
        }
        std::cout << "\n";
        curr = curr->siguiente;
    } while (curr != listaImagenes.cabeza);
}

// ==================== MENU CARGA MASIVA ====================
void menuCargaMasiva() {
    encabezado("CARGA MASIVA DE DATOS");
    std::cout << "  [1] Cargar Capas (.cap)\n";
    std::cout << "  [2] Cargar Imagenes (.im)\n";
    std::cout << "  [3] Cargar Usuarios (.usr)\n";
    std::cout << "  [4] Carga automatica (capas + imagenes + usuarios)\n";
    std::cout << "  [0] Volver\n";
    std::cout << "  Opcion: ";
    int op; std::cin >> op; limpiarBuffer();

    switch (op) {
        case 1: {
            std::cout << "  Ruta del archivo .cap: ";
            std::string ruta; std::getline(std::cin, ruta);
            cargarCapas(ruta, arbolCapas);
            break;
        }
        case 2: {
            std::cout << "  Ruta del archivo .im: ";
            std::string ruta; std::getline(std::cin, ruta);
            cargarImagenes(ruta, listaImagenes, arbolCapas);
            break;
        }
        case 3: {
            std::cout << "  Ruta del archivo .usr: ";
            std::string ruta; std::getline(std::cin, ruta);
            cargarUsuarios(ruta, arbolUsuarios, listaImagenes);
            break;
        }
        case 4: {
            std::cout << "  Ruta base (sin extension): ";
            std::string base; std::getline(std::cin, base);
            std::cout << "  Cargando capas...\n";
            cargarCapas(base + ".cap", arbolCapas);
            std::cout << "  Cargando imagenes...\n";
            cargarImagenes(base + ".im", listaImagenes, arbolCapas);
            std::cout << "  Cargando usuarios...\n";
            cargarUsuarios(base + ".usr", arbolUsuarios, listaImagenes);
            break;
        }
        default: return;
    }
    pausar();
}

// ==================== MENU GENERACION DE IMAGENES ====================
void menuGeneracion() {
    encabezado("GENERACION DE IMAGENES");
    std::cout << "  [1] Por recorrido limitado\n";
    std::cout << "  [2] Por lista de imagenes (id)\n";
    std::cout << "  [3] Por capa\n";
    std::cout << "  [4] Por usuario\n";
    std::cout << "  [0] Volver\n";
    std::cout << "  Opcion: ";
    int op; std::cin >> op; limpiarBuffer();

    switch (op) {
        case 1: {
            std::cout << "  Numero de capas a utilizar: ";
            int n; std::cin >> n; limpiarBuffer();
            std::cout << "  Tipo de recorrido (1=inorden, 2=preorden, 3=postorden): ";
            int tipo; std::cin >> tipo; limpiarBuffer();
            std::vector<Capa*> capas;
            if (tipo == 1) inorden(arbolCapas.raiz, capas, n);
            else if (tipo == 2) preorden(arbolCapas.raiz, capas, n);
            else postorden(arbolCapas.raiz, capas, n);
            if (capas.empty()) { std::cout << "  No hay capas en el arbol.\n"; break; }
            std::string sal = dirGraficas + "gen_recorrido";
            generarImagenPixeles(capas, sal);
            std::cout << "  Imagen generada en: " << sal << ".png\n";
            break;
        }
        case 2: {
            std::cout << "  ID de la imagen: ";
            int id; std::cin >> id; limpiarBuffer();
            NodoImagen* ni = listaImagenes.buscar(id);
            if (!ni) { std::cout << "  Imagen no encontrada.\n"; break; }
            std::vector<Capa*> capas;
            NodoListaCapa* nc = ni->imagen->capas.cabeza;
            while (nc) { capas.push_back(nc->capa); nc = nc->siguiente; }
            std::string sal = dirGraficas + "gen_img_" + std::to_string(id);
            generarImagenPixeles(capas, sal);
            std::cout << "  Imagen generada en: " << sal << ".png\n";
            break;
        }
        case 3: {
            std::cout << "  ID de la capa: ";
            int id; std::cin >> id; limpiarBuffer();
            Capa* c = arbolCapas.buscar(id);
            if (!c) { std::cout << "  Capa no encontrada.\n"; break; }
            std::string sal = dirGraficas + "gen_capa_" + std::to_string(id);
            generarImagenPixeles({c}, sal);
            std::cout << "  Imagen generada en: " << sal << ".png\n";
            break;
        }
        case 4: {
            std::cout << "  Nombre del usuario: ";
            std::string nombre; std::getline(std::cin, nombre);
            Usuario* usr = arbolUsuarios.buscar(nombre);
            if (!usr) { std::cout << "  Usuario no encontrado.\n"; break; }
            // Mostrar imagenes del usuario
            NodoImagenUsuario* ni = usr->imagenes.cabeza;
            if (!ni) { std::cout << "  El usuario no tiene imagenes.\n"; break; }
            std::cout << "  Imagenes del usuario:\n";
            while (ni) {
                std::cout << "    ID=" << ni->imagen->id << "\n";
                ni = ni->siguiente;
            }
            std::cout << "  Seleccione ID de imagen: ";
            int id; std::cin >> id; limpiarBuffer();
            ni = usr->imagenes.cabeza;
            Imagen* imgSel = nullptr;
            while (ni) {
                if (ni->imagen->id == id) { imgSel = ni->imagen; break; }
                ni = ni->siguiente;
            }
            if (!imgSel) { std::cout << "  Imagen no encontrada en la lista del usuario.\n"; break; }
            std::vector<Capa*> capas;
            NodoListaCapa* nc = imgSel->capas.cabeza;
            while (nc) { capas.push_back(nc->capa); nc = nc->siguiente; }
            std::string sal = dirGraficas + "gen_usr_" + nombre + "_img_" + std::to_string(id);
            generarImagenPixeles(capas, sal);
            std::cout << "  Imagen generada en: " << sal << ".png\n";
            break;
        }
        default: return;
    }
    pausar();
}

// ==================== MENU CRUD USUARIOS ====================
void menuCRUDUsuarios() {
    encabezado("CRUD - USUARIOS");
    std::cout << "  [1] Agregar usuario\n";
    std::cout << "  [2] Eliminar usuario\n";
    std::cout << "  [3] Modificar nombre de usuario\n";
    std::cout << "  [4] Buscar usuario\n";
    std::cout << "  [0] Volver\n";
    std::cout << "  Opcion: ";
    int op; std::cin >> op; limpiarBuffer();

    switch (op) {
        case 1: {
            std::cout << "  Nombre del nuevo usuario: ";
            std::string nombre; std::getline(std::cin, nombre);
            if (arbolUsuarios.buscar(nombre)) {
                std::cout << "  El usuario ya existe.\n";
            } else {
                arbolUsuarios.insertar(nombre);
                std::cout << "  Usuario '" << nombre << "' agregado.\n";
            }
            break;
        }
        case 2: {
            std::cout << "  Nombre del usuario a eliminar: ";
            std::string nombre; std::getline(std::cin, nombre);
            if (arbolUsuarios.eliminar(nombre))
                std::cout << "  Usuario eliminado.\n";
            else
                std::cout << "  Usuario no encontrado.\n";
            break;
        }
        case 3: {
            std::cout << "  Nombre actual del usuario: ";
            std::string viejo; std::getline(std::cin, viejo);
            Usuario* usr = arbolUsuarios.buscar(viejo);
            if (!usr) { std::cout << "  Usuario no encontrado.\n"; break; }
            std::cout << "  Nuevo nombre: ";
            std::string nuevo; std::getline(std::cin, nuevo);
            if (arbolUsuarios.buscar(nuevo)) { std::cout << "  Ya existe un usuario con ese nombre.\n"; break; }
            // Insertar nuevo con la lista de imagenes del viejo
            arbolUsuarios.insertar(nuevo);
            Usuario* usrNuevo = arbolUsuarios.buscar(nuevo);
            // Copiar lista de imagenes
            NodoImagenUsuario* ni = usr->imagenes.cabeza;
            while (ni) {
                usrNuevo->imagenes.agregar(ni->imagen);
                ni = ni->siguiente;
            }
            arbolUsuarios.eliminar(viejo);
            std::cout << "  Usuario renombrado a '" << nuevo << "'.\n";
            break;
        }
        case 4: {
            std::cout << "  Nombre a buscar: ";
            std::string nombre; std::getline(std::cin, nombre);
            Usuario* usr = arbolUsuarios.buscar(nombre);
            if (!usr) { std::cout << "  Usuario no encontrado.\n"; break; }
            std::cout << "  Usuario encontrado: " << usr->nombre << "\n  Imagenes:\n";
            imprimirListaCapas(ListaCapas()); // solo para reusar, hacerlo manual:
            NodoImagenUsuario* ni = usr->imagenes.cabeza;
            if (!ni) std::cout << "    (sin imagenes)\n";
            while (ni) { std::cout << "    -> img_" << ni->imagen->id << "\n"; ni = ni->siguiente; }
            break;
        }
        default: return;
    }
    pausar();
}

// ==================== MENU CRUD IMAGENES ====================
void menuCRUDImagenes() {
    encabezado("CRUD - IMAGENES");
    std::cout << "  [1] Agregar imagen\n";
    std::cout << "  [2] Eliminar imagen\n";
    std::cout << "  [0] Volver\n";
    std::cout << "  Opcion: ";
    int op; std::cin >> op; limpiarBuffer();

    switch (op) {
        case 1: {
            std::cout << "  Nombre del usuario: ";
            std::string nombre; std::getline(std::cin, nombre);
            Usuario* usr = arbolUsuarios.buscar(nombre);
            if (!usr) { std::cout << "  Usuario no encontrado.\n"; break; }
            std::cout << "  ID de la nueva imagen: ";
            int id; std::cin >> id; limpiarBuffer();
            if (listaImagenes.buscar(id)) { std::cout << "  Ya existe imagen con ese ID.\n"; break; }
            Imagen* img = new Imagen(); img->id = id;
            // Agregar capas
            std::cout << "  IDs de capas (separados por coma, Enter para ninguna): ";
            std::string capasStr; std::getline(std::cin, capasStr);
            // parsear
            std::stringstream ss(capasStr);
            std::string tok;
            while (std::getline(ss, tok, ',')) {
                std::string t = tok; while(!t.empty()&&(t[0]==' '||t[0]=='\t'))t=t.substr(1);
                while(!t.empty()&&(t.back()==' '||t.back()=='\t'))t.pop_back();
                if (t.empty()) continue;
                try {
                    int idC = std::stoi(t);
                    Capa* c = arbolCapas.buscar(idC);
                    if (c) img->capas.agregar(c);
                    else std::cout << "  Capa " << idC << " no encontrada, ignorada.\n";
                } catch (...) {}
            }
            listaImagenes.insertar(img);
            usr->imagenes.agregar(img);
            std::cout << "  Imagen " << id << " agregada y asignada a " << nombre << ".\n";
            break;
        }
        case 2: {
            std::cout << "  Nombre del usuario: ";
            std::string nombre; std::getline(std::cin, nombre);
            Usuario* usr = arbolUsuarios.buscar(nombre);
            if (!usr) { std::cout << "  Usuario no encontrado.\n"; break; }
            std::cout << "  ID de imagen a eliminar: ";
            int id; std::cin >> id; limpiarBuffer();
            // Eliminar de la lista del usuario
            usr->imagenes.eliminar(id);
            // Eliminar de la lista circular global
            listaImagenes.eliminar(id);
            std::cout << "  Imagen " << id << " eliminada.\n";
            break;
        }
        default: return;
    }
    pausar();
}

// ==================== MENU ESTADO MEMORIA ====================
void menuEstadoMemoria() {
    encabezado("ESTADO DE LA MEMORIA (GRAFICAS)");
    std::cout << "  [1] Ver lista de imagenes\n";
    std::cout << "  [2] Ver arbol de capas\n";
    std::cout << "  [3] Ver capa (matriz dispersa)\n";
    std::cout << "  [4] Ver imagen y arbol de capas\n";
    std::cout << "  [5] Ver arbol de usuarios\n";
    std::cout << "  [0] Volver\n";
    std::cout << "  Opcion: ";
    int op; std::cin >> op; limpiarBuffer();

    switch (op) {
        case 1: {
            imprimirListaCircular();
            std::string sal = dirGraficas + "lista_imagenes";
            graficarListaImagenes(listaImagenes, sal);
            std::cout << "  Grafica: " << sal << ".png\n";
            break;
        }
        case 2: {
            if (!arbolCapas.raiz) { std::cout << "  Arbol vacio.\n"; break; }
            std::string sal = dirGraficas + "arbol_capas";
            graficarABBCapas(arbolCapas, sal);
            std::cout << "  Grafica: " << sal << ".png\n";
            break;
        }
        case 3: {
            std::cout << "  ID de capa: ";
            int id; std::cin >> id; limpiarBuffer();
            Capa* c = arbolCapas.buscar(id);
            if (!c) { std::cout << "  Capa no encontrada.\n"; break; }
            std::string sal = dirGraficas + "capa_" + std::to_string(id);
            graficarMatrizDispersa(c, sal);
            std::cout << "  Grafica: " << sal << ".png\n";
            break;
        }
        case 4: {
            std::cout << "  ID de imagen: ";
            int id; std::cin >> id; limpiarBuffer();
            NodoImagen* ni = listaImagenes.buscar(id);
            if (!ni) { std::cout << "  Imagen no encontrada.\n"; break; }
            std::string sal = dirGraficas + "imgArbol_" + std::to_string(id);
            graficarImagenYArbol(ni, arbolCapas, sal);
            std::cout << "  Grafica: " << sal << ".png\n";
            break;
        }
        case 5: {
            if (!arbolUsuarios.raiz) { std::cout << "  Arbol vacio.\n"; break; }
            std::string sal = dirGraficas + "arbol_usuarios";
            graficarABBUsuarios(arbolUsuarios, sal);
            std::cout << "  Grafica: " << sal << ".png\n";
            break;
        }
        default: return;
    }
    pausar();
}

// ==================== MENU PRINCIPAL ====================
int main() {
    // Crear directorio de graficas
    system("mkdir -p graficas");

    std::cout << "\n╔══════════════════════════════════════════╗\n";
    std::cout << "║  GENERADOR DE IMAGENES POR CAPAS         ║\n";
    std::cout << "║  Estructura de Datos I - URL 2026        ║\n";
    std::cout << "╚══════════════════════════════════════════╝\n";

    bool salir = false;
    while (!salir) {
        encabezado("MENU PRINCIPAL");
        std::cout << "  [1] Carga Masiva\n";
        std::cout << "  [2] Generacion de Imagenes\n";
        std::cout << "  [3] CRUD Usuarios\n";
        std::cout << "  [4] CRUD Imagenes\n";
        std::cout << "  [5] Estado de la Memoria (Graficas)\n";
        std::cout << "  [0] Salir\n";
        std::cout << "\n  Opcion: ";
        int op; std::cin >> op; limpiarBuffer();

        switch (op) {
            case 1: menuCargaMasiva(); break;
            case 2: menuGeneracion(); break;
            case 3: menuCRUDUsuarios(); break;
            case 4: menuCRUDImagenes(); break;
            case 5: menuEstadoMemoria(); break;
            case 0: salir = true; break;
            default: std::cout << "  Opcion invalida.\n"; break;
        }
    }
    std::cout << "\n  Hasta luego!\n\n";
    return 0;
}
