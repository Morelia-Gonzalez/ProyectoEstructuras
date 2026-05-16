#include "cargador.h"
#include <fstream>
#include <sstream>
#include <iostream>
#include <cctype>
#include <vector>

// ==================== UTILIDADES DE PARSING ====================
static std::string trim(const std::string& s) {
    size_t start = s.find_first_not_of(" \t\r\n");
    if (start == std::string::npos) return "";
    size_t end = s.find_last_not_of(" \t\r\n");
    return s.substr(start, end - start + 1);
}

static std::vector<std::string> split(const std::string& s, char delim) {
    std::vector<std::string> tokens;
    std::stringstream ss(s);
    std::string tok;
    while (std::getline(ss, tok, delim)) {
        std::string t = trim(tok);
        if (!t.empty()) tokens.push_back(t);
    }
    return tokens;
}

// ==================== CARGA MASIVA DE CAPAS (.cap) ====================
// Formato lexico:
//   ID_ENTERO '{' (FILA ',' COLUMNA ',' COLOR ';')* '}'
bool cargarCapas(const std::string& archivo, ABBCapas& arbol) {
    std::ifstream f(archivo);
    if (!f.is_open()) {
        std::cerr << "[ERROR] No se pudo abrir: " << archivo << std::endl;
        return false;
    }
    std::string linea;
    int linNum = 0;
    int capaActualId = -1;
    bool dentroBloque = false;
    int capasLeidas = 0;

    while (std::getline(f, linea)) {
        linNum++;
        linea = trim(linea);
        if (linea.empty()) continue;

        if (!dentroBloque) {
            // Esperamos: ID {
            // puede estar en la misma linea: "1 {" o "1{"
            size_t posLlave = linea.find('{');
            if (posLlave != std::string::npos) {
                std::string idStr = trim(linea.substr(0, posLlave));
                if (idStr.empty() || !std::isdigit(idStr[0])) {
                    std::cerr << "[WARN] Linea " << linNum << ": se esperaba ID numerico antes de '{'\n";
                    continue;
                }
                capaActualId = std::stoi(idStr);
                // Insertar capa en el ABB (nombre por defecto "capa_ID")
                arbol.insertar(capaActualId, "capa_" + idStr);
                dentroBloque = true;
            }
            // else: linea sin '{', ignorar (cabecera, comentario, etc.)
        } else {
            // Dentro del bloque
            if (linea == "}") {
                dentroBloque = false;
                capasLeidas++;
                continue;
            }
            // Esperamos: fila,columna,color;
            // Quitar el ';' final si existe
            if (!linea.empty() && linea.back() == ';') linea.pop_back();
            auto partes = split(linea, ',');
            if (partes.size() < 3) {
                std::cerr << "[WARN] Linea " << linNum << ": formato incorrecto '" << linea << "'\n";
                continue;
            }
            try {
                int fila = std::stoi(partes[0]);
                int columna = std::stoi(partes[1]);
                std::string color = trim(partes[2]);
                // Buscar la capa recién insertada
                Capa* capa = arbol.buscar(capaActualId);
                if (capa) {
                    capa->matriz->insertar(fila, columna, color);
                }
            } catch (...) {
                std::cerr << "[WARN] Linea " << linNum << ": error al parsear '" << linea << "'\n";
            }
        }
    }
    if (dentroBloque) {
        std::cerr << "[WARN] Archivo termino sin cerrar bloque de capa " << capaActualId << "\n";
    }
    std::cout << "  Capas cargadas: " << capasLeidas << std::endl;
    return true;
}

// ==================== CARGA MASIVA DE IMAGENES (.im) ====================
// Formato: ID{idcapa,...,idcapa}  (una imagen por linea)
bool cargarImagenes(const std::string& archivo, ListaCircularImagenes& lista, ABBCapas& arbCapas) {
    std::ifstream f(archivo);
    if (!f.is_open()) {
        std::cerr << "[ERROR] No se pudo abrir: " << archivo << std::endl;
        return false;
    }
    std::string linea;
    int linNum = 0;
    int imagenesLeidas = 0;

    while (std::getline(f, linea)) {
        linNum++;
        linea = trim(linea);
        if (linea.empty()) continue;

        // Parsear: ID{c1,c2,...}
        size_t posLlave1 = linea.find('{');
        size_t posLlave2 = linea.find('}');
        if (posLlave1 == std::string::npos || posLlave2 == std::string::npos) {
            std::cerr << "[WARN] Linea " << linNum << ": formato incorrecto '" << linea << "'\n";
            continue;
        }
        std::string idStr = trim(linea.substr(0, posLlave1));
        std::string capasStr = trim(linea.substr(posLlave1 + 1, posLlave2 - posLlave1 - 1));

        if (idStr.empty() || !std::isdigit(idStr[0])) {
            std::cerr << "[WARN] Linea " << linNum << ": ID invalido\n";
            continue;
        }
        int idImg = std::stoi(idStr);

        // Verificar que no exista
        if (lista.buscar(idImg)) {
            std::cerr << "[WARN] Imagen ID=" << idImg << " ya existe, ignorando.\n";
            continue;
        }

        Imagen* img = new Imagen();
        img->id = idImg;

        // Parsear lista de capas
        if (!capasStr.empty()) {
            auto partes = split(capasStr, ',');
            for (auto& p : partes) {
                p = trim(p);
                if (p.empty()) continue;
                try {
                    int idCapa = std::stoi(p);
                    Capa* capa = arbCapas.buscar(idCapa);
                    if (capa) img->capas.agregar(capa);
                    else std::cerr << "[WARN] Capa ID=" << idCapa << " no encontrada para imagen " << idImg << "\n";
                } catch (...) {
                    std::cerr << "[WARN] ID de capa invalido: '" << p << "'\n";
                }
            }
        }

        lista.insertar(img);
        imagenesLeidas++;
    }
    std::cout << "  Imagenes cargadas: " << imagenesLeidas << std::endl;
    return true;
}

// ==================== CARGA MASIVA DE USUARIOS (.usr) ====================
// Formato: nombre:idimg,...;
bool cargarUsuarios(const std::string& archivo, ABBUsuarios& arbol, ListaCircularImagenes& listImg) {
    std::ifstream f(archivo);
    if (!f.is_open()) {
        std::cerr << "[ERROR] No se pudo abrir: " << archivo << std::endl;
        return false;
    }
    std::string linea;
    int linNum = 0;
    int usersLeidos = 0;

    while (std::getline(f, linea)) {
        linNum++;
        linea = trim(linea);
        if (linea.empty()) continue;

        // Quitar ';' final
        if (!linea.empty() && linea.back() == ';') linea.pop_back();

        size_t posDos = linea.find(':');
        if (posDos == std::string::npos) {
            std::cerr << "[WARN] Linea " << linNum << ": falta ':' en '" << linea << "'\n";
            continue;
        }
        std::string nombre = trim(linea.substr(0, posDos));
        std::string imgsStr = trim(linea.substr(posDos + 1));

        if (nombre.empty()) { std::cerr << "[WARN] Linea " << linNum << ": nombre vacio\n"; continue; }

        arbol.insertar(nombre);
        Usuario* usr = arbol.buscar(nombre);
        if (!usr) continue;

        if (!imgsStr.empty()) {
            auto partes = split(imgsStr, ',');
            for (auto& p : partes) {
                p = trim(p);
                if (p.empty()) continue;
                try {
                    int idImg = std::stoi(p);
                    NodoImagen* ni = listImg.buscar(idImg);
                    if (ni) usr->imagenes.agregar(ni->imagen);
                    else std::cerr << "[WARN] Imagen ID=" << idImg << " no encontrada para usuario " << nombre << "\n";
                } catch (...) {
                    std::cerr << "[WARN] ID de imagen invalido: '" << p << "'\n";
                }
            }
        }
        usersLeidos++;
    }
    std::cout << "  Usuarios cargados: " << usersLeidos << std::endl;
    return true;
}
