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

ABBCapas arbolCapas;
ListaCircularImagenes listaImagenes;
ABBUsuarios arbolUsuarios;
std::string dirGraficas = "graficas/";

int leerOpcion() {
    std::string linea;
    std::getline(std::cin, linea);
    if (!linea.empty() && linea.back() == '\r') linea.pop_back();
    try { return std::stoi(linea); } catch (...) { return -1; }
}

int leerEntero(const std::string& prompt) {
    std::cout << prompt;
    return leerOpcion();
}

std::string leerTexto(const std::string& prompt) {
    std::cout << prompt;
    std::string linea;
    std::getline(std::cin, linea);
    if (!linea.empty() && linea.back() == '\r') linea.pop_back();
    size_t s = linea.find_first_not_of(" \t");
    if (s == std::string::npos) return "";
    size_t e = linea.find_last_not_of(" \t");
    return linea.substr(s, e - s + 1);
}

void pausar() {
    std::cout << "\n  Presione Enter para continuar...";
    std::string dummy;
    std::getline(std::cin, dummy);
}

void encabezado(const std::string& titulo) {
    std::cout << "\n=======================================\n";
    std::cout << "  " << titulo << "\n";
    std::cout << "=========================================\n";
}

static void preorden(Capa* nodo, std::vector<Capa*>& res, int lim) {
    if (!nodo || (int)res.size() >= lim) return;
    res.push_back(nodo);
    preorden(nodo->izquierdo, res, lim);
    preorden(nodo->derecho, res, lim);
}
static void inorden(Capa* nodo, std::vector<Capa*>& res, int lim) {
    if (!nodo || (int)res.size() >= lim) return;
    inorden(nodo->izquierdo, res, lim);
    if ((int)res.size() < lim) res.push_back(nodo);
    inorden(nodo->derecho, res, lim);
}
static void postorden(Capa* nodo, std::vector<Capa*>& res, int lim) {
    if (!nodo || (int)res.size() >= lim) return;
    postorden(nodo->izquierdo, res, lim);
    postorden(nodo->derecho, res, lim);
    if ((int)res.size() < lim) res.push_back(nodo);
}

static void imprimirListaCircular() {
    if (!listaImagenes.cabeza) { std::cout << "  Lista vacia.\n"; return; }
    NodoImagen* curr = listaImagenes.cabeza;
    do {
        std::cout << "  [Imagen ID=" << curr->imagen->id << "] Capas: ";
        NodoListaCapa* nc = curr->imagen->capas.cabeza;
        if (!nc) std::cout << "(ninguna)";
        while (nc) { std::cout << nc->capa->id; if (nc->siguiente) std::cout << ","; nc = nc->siguiente; }
        std::cout << "\n";
        curr = curr->siguiente;
    } while (curr != listaImagenes.cabeza);
}

void menuCargaMasiva() {
    encabezado("CARGA MASIVA DE DATOS");
    std::cout << "  1. Cargar Capas (.cap)\n";
    std::cout << "  2. Cargar Imagenes (.im)\n";
    std::cout << "  3. Cargar Usuarios (.usr)\n";
    std::cout << "  4. Carga automatica (misma ruta base)\n";
    std::cout << "  0. Volver\n";
    std::cout << "  Opcion: ";
    int op = leerOpcion();
    switch (op) {
        case 1: { std::string r = leerTexto("  Ruta .cap: "); cargarCapas(r, arbolCapas); break; }
        case 2: { std::string r = leerTexto("  Ruta .im: "); cargarImagenes(r, listaImagenes, arbolCapas); break; }
        case 3: { std::string r = leerTexto("  Ruta .usr: "); cargarUsuarios(r, arbolUsuarios, listaImagenes); break; }
        case 4: {
            std::string base = leerTexto("  Ruta base (ej: datos/datos): ");
            std::cout << "  Cargando capas...\n";   cargarCapas(base+".cap", arbolCapas);
            std::cout << "  Cargando imagenes...\n"; cargarImagenes(base+".im", listaImagenes, arbolCapas);
            std::cout << "  Cargando usuarios...\n"; cargarUsuarios(base+".usr", arbolUsuarios, listaImagenes);
            break;
        }
        default: return;
    }
    pausar();
}

void menuGeneracion() {
    encabezado("GENERACION DE IMAGENES");
    std::cout << "  1. Por recorrido limitado\n  2. Por lista (id imagen)\n  3. Por capa\n  4. Por usuario\n  0. Volver\n";
    std::cout << "  Opcion: ";
    int op = leerOpcion();
    switch (op) {
        case 1: {
            int n = leerEntero("  Numero de capas: ");
            std::cout << "  Recorrido: 1. Inorden 2. Preorden 3.Postorden\n  Opcion: ";
            int tipo = leerOpcion();
            std::vector<Capa*> capas;
            if (tipo==1) inorden(arbolCapas.raiz,capas,n);
            else if (tipo==2) preorden(arbolCapas.raiz,capas,n);
            else postorden(arbolCapas.raiz,capas,n);
            if (capas.empty()) { std::cout << "  Sin capas.\n"; break; }
            generarImagenPixeles(capas, dirGraficas+"gen_recorrido");
            break;
        }
        case 2: {
            int id = leerEntero("  ID imagen: ");
            NodoImagen* ni = listaImagenes.buscar(id);
            if (!ni) { std::cout << "  No encontrada.\n"; break; }
            std::vector<Capa*> capas;
            NodoListaCapa* nc = ni->imagen->capas.cabeza;
            while (nc) { capas.push_back(nc->capa); nc = nc->siguiente; }
            generarImagenPixeles(capas, dirGraficas+"gen_img_"+std::to_string(id));
            break;
        }
        case 3: {
            int id = leerEntero("  ID capa: ");
            Capa* c = arbolCapas.buscar(id);
            if (!c) { std::cout << "  No encontrada.\n"; break; }
            generarImagenPixeles({c}, dirGraficas+"gen_capa_"+std::to_string(id));
            break;
        }
        case 4: {
            std::string nombre = leerTexto("  Nombre usuario: ");
            Usuario* usr = arbolUsuarios.buscar(nombre);
            if (!usr) { std::cout << "  No encontrado.\n"; break; }
            NodoImagenUsuario* ni = usr->imagenes.cabeza;
            if (!ni) { std::cout << "  Sin imagenes.\n"; break; }
            std::cout << "  Imagenes:\n";
            while (ni) { std::cout << "    ID=" << ni->imagen->id << "\n"; ni = ni->siguiente; }
            int id = leerEntero("  Seleccione ID: ");
            ni = usr->imagenes.cabeza;
            Imagen* imgSel = nullptr;
            while (ni) { if (ni->imagen->id==id) { imgSel=ni->imagen; break; } ni=ni->siguiente; }
            if (!imgSel) { std::cout << "  No encontrada.\n"; break; }
            std::vector<Capa*> capas;
            NodoListaCapa* nc = imgSel->capas.cabeza;
            while (nc) { capas.push_back(nc->capa); nc=nc->siguiente; }
            generarImagenPixeles(capas, dirGraficas+"gen_usr_"+nombre+"_"+std::to_string(id));
            break;
        }
        default: return;
    }
    pausar();
}

void menuCRUDUsuarios() {
    encabezado("USUARIOS");
    std::cout << "  1. Agregar  2. Eliminar  3. Modificar  4. Buscar  0. Volver\n";
    std::cout << "  Opcion: ";
    int op = leerOpcion();
    switch (op) {
        case 1: {
            std::string n = leerTexto("  Nombre: ");
            if (arbolUsuarios.buscar(n)) std::cout << "  Ya existe.\n";
            else { arbolUsuarios.insertar(n); std::cout << "  Agregado.\n"; }
            break;
        }
        case 2: {
            std::string n = leerTexto("  Nombre: ");
            std::cout << (arbolUsuarios.eliminar(n) ? "  Eliminado.\n" : "  No encontrado.\n");
            break;
        }
        case 3: {
            std::string viejo = leerTexto("  Nombre actual: ");
            Usuario* usr = arbolUsuarios.buscar(viejo);
            if (!usr) { std::cout << "  No encontrado.\n"; break; }
            std::string nuevo = leerTexto("  Nuevo nombre: ");
            if (arbolUsuarios.buscar(nuevo)) { std::cout << "  Ya existe.\n"; break; }
            arbolUsuarios.insertar(nuevo);
            Usuario* usrN = arbolUsuarios.buscar(nuevo);
            NodoImagenUsuario* ni = usr->imagenes.cabeza;
            while (ni) { usrN->imagenes.agregar(ni->imagen); ni=ni->siguiente; }
            arbolUsuarios.eliminar(viejo);
            std::cout << "  Renombrado.\n";
            break;
        }
        case 4: {
            std::string n = leerTexto("  Nombre: ");
            Usuario* usr = arbolUsuarios.buscar(n);
            if (!usr) { std::cout << "  No encontrado.\n"; break; }
            std::cout << "  Usuario: " << usr->nombre << "\n";
            NodoImagenUsuario* ni = usr->imagenes.cabeza;
            if (!ni) std::cout << "  Sin imagenes.\n";
            while (ni) { std::cout << "    -> img_" << ni->imagen->id << "\n"; ni=ni->siguiente; }
            break;
        }
        default: return;
    }
    pausar();
}

void menuCRUDImagenes() {
    encabezado("IMAGENES");
    std::cout << "  1. Agregar  2. Eliminar  0. Volver\n";
    std::cout << "  Opcion: ";
    int op = leerOpcion();
    switch (op) {
        case 1: {
            std::string nombre = leerTexto("  Nombre usuario: ");
            Usuario* usr = arbolUsuarios.buscar(nombre);
            if (!usr) { std::cout << "  Usuario no encontrado.\n"; break; }
            int id = leerEntero("  ID nueva imagen: ");
            if (listaImagenes.buscar(id)) { std::cout << "  ID ya existe.\n"; break; }
            Imagen* img = new Imagen(); img->id = id;
            std::string cs = leerTexto("  IDs capas (coma) o Enter: ");
            if (!cs.empty()) {
                std::stringstream ss(cs);
                std::string tok;
                while (std::getline(ss, tok, ',')) {
                    while (!tok.empty() && (tok[0]==' '||tok[0]=='\t'||tok[0]=='\r')) tok=tok.substr(1);
                    while (!tok.empty() && (tok.back()==' '||tok.back()=='\t'||tok.back()=='\r')) tok.pop_back();
                    if (tok.empty()) continue;
                    try { Capa* c=arbolCapas.buscar(std::stoi(tok)); if(c) img->capas.agregar(c); } catch(...) {}
                }
            }
            listaImagenes.insertar(img);
            usr->imagenes.agregar(img);
            std::cout << "  Imagen " << id << " agregada.\n";
            break;
        }
        case 2: {
            std::string nombre = leerTexto("  Nombre usuario: ");
            Usuario* usr = arbolUsuarios.buscar(nombre);
            if (!usr) { std::cout << "  Usuario no encontrado.\n"; break; }
            int id = leerEntero("  ID imagen: ");
            usr->imagenes.eliminar(id);
            listaImagenes.eliminar(id);
            std::cout << "  Imagen " << id << " eliminada.\n";
            break;
        }
        default: return;
    }
    pausar();
}

void menuEstadoMemoria() {
    encabezado("ESTADO DE LA MEMORIA (GRAFICAS)");
    std::cout << "  1. Lista imagenes\n  2. Arbol capas\n  3. Capa (matriz)\n  4. Imagen+arbol\n  5. Arbol usuarios\n  0. Volver\n";
    std::cout << "  Opcion: ";
    int op = leerOpcion();
    switch (op) {
        case 1: { imprimirListaCircular(); graficarListaImagenes(listaImagenes, dirGraficas+"lista_imagenes"); break; }
        case 2: { if (!arbolCapas.raiz) { std::cout << "  Vacio.\n"; break; } graficarABBCapas(arbolCapas, dirGraficas+"arbol_capas"); break; }
        case 3: {
            int id = leerEntero("  ID capa: ");
            Capa* c = arbolCapas.buscar(id);
            if (!c) { std::cout << "  No encontrada.\n"; break; }
            graficarMatrizDispersa(c, dirGraficas+"capa_"+std::to_string(id));
            break;
        }
        case 4: {
            int id = leerEntero("  ID imagen: ");
            NodoImagen* ni = listaImagenes.buscar(id);
            if (!ni) { std::cout << "  No encontrada.\n"; break; }
            graficarImagenYArbol(ni, arbolCapas, dirGraficas+"imgArbol_"+std::to_string(id));
            break;
        }
        case 5: { if (!arbolUsuarios.raiz) { std::cout << "  Vacio.\n"; break; } graficarABBUsuarios(arbolUsuarios, dirGraficas+"arbol_usuarios"); break; }
        default: return;
    }
    pausar();
}

int main() {
    system("mkdir graficas 2>nul");
    system("mkdir -p graficas 2>/dev/null");

    std::cout << "\n╔══════════════════════════════════════════╗\n";
    std::cout << "║  GENERADOR DE IMAGENES POR CAPAS         ║\n";
    std::cout << "╚══════════════════════════════════════════╝\n";
    std::cout << "  Archivos de datos en: datos/\n";
    std::cout << "  Graficas generadas en: graficas/\n";

    bool salir = false;
    while (!salir) {
        encabezado("MENU PRINCIPAL");
        std::cout << "  1. Carga Masiva\n  2. Generacion de Imagenes\n  3. Usuarios\n";
        std::cout << "  4. Imagenes\n  5. Estado de la Memoria (Graficas)\n  0. Salir\n";
        std::cout << "\n  Opcion: ";
        int op = leerOpcion();
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
