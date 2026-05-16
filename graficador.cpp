#include "graficador.h"
#include <fstream>
#include <sstream>
#include <cstdlib>
#include <vector>
#include <map>
#include <iostream>

void generarPNG(const std::string& archivoDot, const std::string& archivoPNG) {
    std::string cmd = "dot -Tpng \"" + archivoDot + "\" -o \"" + archivoPNG + "\"";
    int ret = system(cmd.c_str());
    if (ret == 0)
        std::cout << "  [OK] Imagen generada: " << archivoPNG << std::endl;
    else
        std::cout << "  [ERROR] No se pudo generar: " << archivoPNG << std::endl;
}

// -------------------- MATRIZ DISPERSA --------------------
void graficarMatrizDispersa(const Capa* capa, const std::string& salida) {
    std::string dotFile = salida + ".dot";
    std::ofstream f(dotFile);
    f << "digraph MatrizDispersa {\n";
    f << "  rankdir=LR;\n";
    f << "  node [shape=record fontsize=10];\n";
    f << "  label=\"Capa " << capa->id << " - " << capa->nombre << " (Matriz Dispersa)\";\n";
    f << "  labelloc=t;\n\n";

    // Nodo cabecera de la matriz
    f << "  matriz [label=\"matriz | id=" << capa->id << "\" shape=record fillcolor=lightblue style=filled];\n";

    MatrizDispersa* m = capa->matriz;
    NodoFila* fila = m->filas;
    std::string prevFila = "matriz";

    while (fila) {
        std::string idFila = "fila_" + std::to_string(fila->fila);
        f << "  " << idFila << " [label=\"{" << fila->fila << "}\" fillcolor=lightyellow style=filled];\n";
        f << "  " << prevFila << " -> " << idFila << ";\n";

        NodoColumna* col = fila->columnas;
        std::string prevCol = idFila;
        while (col) {
            std::string idCol = "nodo_" + std::to_string(fila->fila) + "_" + std::to_string(col->columna);
            // color de fondo del nodo = el color del pixel
            std::string hexColor = col->color;
            if (hexColor.size() > 0 && hexColor[0] != '#') hexColor = "#" + hexColor;
            f << "  " << idCol << " [label=\"{col=" << col->columna << " | " << col->color << "}\" "
              << "fillcolor=\"" << hexColor << "\" style=filled];\n";
            f << "  " << prevCol << " -> " << idCol << ";\n";
            prevCol = idCol;
            col = col->siguiente;
        }
        prevFila = idFila;
        fila = fila->siguiente;
    }

    f << "}\n";
    f.close();
    generarPNG(dotFile, salida + ".png");
}

// -------------------- ABB CAPAS --------------------
static void dotABBCapas(std::ofstream& f, const Capa* nodo) {
    if (!nodo) return;
    f << "  capa_" << nodo->id << " [label=\"{capa_" << nodo->id << " | " << nodo->nombre << "}\" "
      << "shape=record fillcolor=lightgreen style=filled];\n";
    if (nodo->izquierdo) {
        f << "  capa_" << nodo->id << " -> capa_" << nodo->izquierdo->id << " [label=\"izq\"];\n";
        dotABBCapas(f, nodo->izquierdo);
    }
    if (nodo->derecho) {
        f << "  capa_" << nodo->id << " -> capa_" << nodo->derecho->id << " [label=\"der\"];\n";
        dotABBCapas(f, nodo->derecho);
    }
}

void graficarABBCapas(const ABBCapas& abb, const std::string& salida) {
    std::string dotFile = salida + ".dot";
    std::ofstream f(dotFile);
    f << "digraph ABBCapas {\n";
    f << "  node [shape=record fontsize=10];\n";
    f << "  label=\"Arbol Binario de Busqueda - Capas\";\n";
    f << "  labelloc=t;\n\n";
    dotABBCapas(f, abb.raiz);
    f << "}\n";
    f.close();
    generarPNG(dotFile, salida + ".png");
}

// -------------------- LISTA CIRCULAR IMAGENES --------------------
void graficarListaImagenes(const ListaCircularImagenes& lista, const std::string& salida) {
    std::string dotFile = salida + ".dot";
    std::ofstream f(dotFile);
    f << "digraph ListaImagenes {\n";
    f << "  rankdir=LR;\n";
    f << "  node [shape=record fontsize=10];\n";
    f << "  label=\"Lista Circular Doblemente Enlazada - Imagenes\";\n";
    f << "  labelloc=t;\n\n";

    if (!lista.cabeza) { f << "  vacio [label=\"Lista vacia\" shape=plaintext];\n}\n"; f.close(); generarPNG(dotFile, salida+".png"); return; }

    // Nodos de imagenes
    NodoImagen* curr = lista.cabeza;
    do {
        int id = curr->imagen->id;
        f << "  img_" << id << " [label=\"{<prev>|img_" << id << "|<next>}\" fillcolor=lightyellow style=filled];\n";
        curr = curr->siguiente;
    } while (curr != lista.cabeza);

    // Flechas siguiente (LR)
    curr = lista.cabeza;
    do {
        f << "  img_" << curr->imagen->id << ":next -> img_" << curr->siguiente->imagen->id << ":prev [color=blue];\n";
        curr = curr->siguiente;
    } while (curr != lista.cabeza);

    // Flechas anterior (arriba)
    curr = lista.cabeza;
    do {
        f << "  img_" << curr->imagen->id << ":prev -> img_" << curr->anterior->imagen->id << ":next [color=red style=dashed];\n";
        curr = curr->siguiente;
    } while (curr != lista.cabeza);

    // Lista de capas de cada imagen (debajo)
    curr = lista.cabeza;
    do {
        int id = curr->imagen->id;
        NodoListaCapa* nc = curr->imagen->capas.cabeza;
        std::string prev = "img_" + std::to_string(id);
        int idx = 0;
        while (nc) {
            std::string nid = "icapa_" + std::to_string(id) + "_" + std::to_string(idx);
            f << "  " << nid << " [label=\"capa_" << nc->capa->id << "\" fillcolor=lightcyan style=filled];\n";
            f << "  " << prev << " -> " << nid << " [color=darkgreen];\n";
            prev = nid;
            nc = nc->siguiente;
            idx++;
        }
        curr = curr->siguiente;
    } while (curr != lista.cabeza);

    f << "}\n";
    f.close();
    generarPNG(dotFile, salida + ".png");
}

// -------------------- ABB USUARIOS --------------------
static void dotABBUsuarios(std::ofstream& f, const Usuario* nodo) {
    if (!nodo) return;
    f << "  usr_" << nodo->nombre << " [label=\"{" << nodo->nombre << "}\" "
      << "shape=record fillcolor=lightblue style=filled];\n";

    // Lista de imagenes del usuario
    NodoImagenUsuario* ni = nodo->imagenes.cabeza;
    int idx = 0;
    std::string prev = "usr_" + nodo->nombre;
    while (ni) {
        std::string nid = "uimg_" + nodo->nombre + "_" + std::to_string(idx);
        f << "  " << nid << " [label=\"img_" << ni->imagen->id << "\" fillcolor=lightyellow style=filled];\n";
        f << "  " << prev << " -> " << nid << " [color=darkgreen];\n";
        prev = nid;
        ni = ni->siguiente;
        idx++;
    }

    if (nodo->izquierdo) {
        f << "  usr_" << nodo->nombre << " -> usr_" << nodo->izquierdo->nombre << " [label=\"izq\"];\n";
        dotABBUsuarios(f, nodo->izquierdo);
    }
    if (nodo->derecho) {
        f << "  usr_" << nodo->nombre << " -> usr_" << nodo->derecho->nombre << " [label=\"der\"];\n";
        dotABBUsuarios(f, nodo->derecho);
    }
}

void graficarABBUsuarios(const ABBUsuarios& abb, const std::string& salida) {
    std::string dotFile = salida + ".dot";
    std::ofstream f(dotFile);
    f << "digraph ABBUsuarios {\n";
    f << "  node [shape=record fontsize=10];\n";
    f << "  label=\"Arbol Binario de Busqueda - Usuarios\";\n";
    f << "  labelloc=t;\n\n";
    dotABBUsuarios(f, abb.raiz);
    f << "}\n";
    f.close();
    generarPNG(dotFile, salida + ".png");
}

// -------------------- IMAGEN + ARBOL CAPAS --------------------
static void dotABBCapasConEnlaces(std::ofstream& f, const Capa* nodo, const std::vector<int>& capasImg) {
    if (!nodo) return;
    bool resaltado = false;
    for (int id : capasImg) if (id == nodo->id) { resaltado = true; break; }
    f << "  tcapa_" << nodo->id << " [label=\"{capa_" << nodo->id << " | " << nodo->nombre << "}\" "
      << "shape=record fillcolor=" << (resaltado ? "orange" : "lightgreen") << " style=filled];\n";
    if (nodo->izquierdo) {
        f << "  tcapa_" << nodo->id << " -> tcapa_" << nodo->izquierdo->id << ";\n";
        dotABBCapasConEnlaces(f, nodo->izquierdo, capasImg);
    }
    if (nodo->derecho) {
        f << "  tcapa_" << nodo->id << " -> tcapa_" << nodo->derecho->id << ";\n";
        dotABBCapasConEnlaces(f, nodo->derecho, capasImg);
    }
}

void graficarImagenYArbol(const NodoImagen* nodoImg, const ABBCapas& abb, const std::string& salida) {
    std::string dotFile = salida + ".dot";
    std::ofstream f(dotFile);
    f << "digraph ImagenYArbol {\n";
    f << "  node [shape=record fontsize=10];\n";
    f << "  label=\"Imagen " << nodoImg->imagen->id << " y su arbol de capas\";\n";
    f << "  labelloc=t;\n\n";

    // Lista de capas de la imagen
    std::vector<int> capasImg;
    NodoListaCapa* nc = nodoImg->imagen->capas.cabeza;
    int idx = 0;
    f << "  subgraph cluster_lista {\n  label=\"Lista de capas\";\n  color=blue;\n";
    std::string prevNode = "";
    while (nc) {
        capasImg.push_back(nc->capa->id);
        std::string nid = "lc_" + std::to_string(idx);
        f << "    " << nid << " [label=\"capa_" << nc->capa->id << "\" fillcolor=lightyellow style=filled];\n";
        if (!prevNode.empty()) f << "    " << prevNode << " -> " << nid << ";\n";
        prevNode = nid;
        nc = nc->siguiente;
        idx++;
    }
    f << "  }\n\n";

    // ABB de capas (resaltando las que pertenecen a esta imagen)
    f << "  subgraph cluster_abb {\n  label=\"ABB Capas\";\n  color=green;\n";
    dotABBCapasConEnlaces(f, abb.raiz, capasImg);
    f << "  }\n\n";

    // Enlace desde lista hacia ABB
    nc = nodoImg->imagen->capas.cabeza;
    idx = 0;
    while (nc) {
        f << "  lc_" << idx << " -> tcapa_" << nc->capa->id << " [color=red style=dashed label=\"ptr\"];\n";
        nc = nc->siguiente;
        idx++;
    }

    f << "}\n";
    f.close();
    generarPNG(dotFile, salida + ".png");
}

// -------------------- IMAGEN PIXELES (canvas) --------------------
void generarImagenPixeles(const std::vector<Capa*>& capas, const std::string& salida) {
    if (capas.empty()) {
        std::cout << "  No hay capas para generar imagen.\n";
        return;
    }

    // Determinar dimensiones maximas
    int maxFila = 0, maxCol = 0;
    for (auto* capa : capas) {
        if (capa->matriz->maxFila > maxFila) maxFila = capa->matriz->maxFila;
        if (capa->matriz->maxColumna > maxCol) maxCol = capa->matriz->maxColumna;
    }
    if (maxFila == 0 || maxCol == 0) {
        // Imagen negra de 1px
        std::string dotFile = salida + ".dot";
        std::ofstream f(dotFile);
        f << "graph G { node [label=\"#000000\" shape=square style=filled fillcolor=black]; px; }\n";
        f.close();
        generarPNG(dotFile, salida + ".png");
        return;
    }

    // Combinar capas: la ultima capa prevalece
    // Usamos un mapa fila->col->color
    std::map<int, std::map<int, std::string>> canvas;
    for (auto* capa : capas) {
        NodoFila* fila = capa->matriz->filas;
        while (fila) {
            NodoColumna* col = fila->columnas;
            while (col) {
                canvas[fila->fila][col->columna] = col->color;
                col = col->siguiente;
            }
            fila = fila->siguiente;
        }
    }

    // Generar dot con tabla HTML
    int pixSize = 20;
    std::string dotFile = salida + ".dot";
    std::ofstream f(dotFile);
    f << "graph G {\n";
    f << "  node [shape=plaintext];\n";
    f << "  img [label=<\n";
    f << "    <TABLE BORDER=\"0\" CELLBORDER=\"0\" CELLSPACING=\"0\" CELLPADDING=\"0\">\n";
    for (int r = 1; r <= maxFila; r++) {
        f << "    <TR>";
        for (int c = 1; c <= maxCol; c++) {
            std::string color = "#FFFFFF";
            if (canvas.count(r) && canvas.at(r).count(c)) {
                color = canvas.at(r).at(c);
                if (color[0] != '#') color = "#" + color;
            }
            f << "<TD WIDTH=\"" << pixSize << "\" HEIGHT=\"" << pixSize
              << "\" BGCOLOR=\"" << color << "\"></TD>";
        }
        f << "</TR>\n";
    }
    f << "    </TABLE>>];\n";
    f << "}\n";
    f.close();
    generarPNG(dotFile, salida + ".png");
}
