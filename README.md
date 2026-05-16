-

## Descripción
Sistema que genera imágenes por capas usando estructuras de datos implementadas manualmente en C++. Cada capa es una matriz dispersa simplemente enlazada; las capas se almacenan en un ABB; las imágenes se organizan en una lista circular doblemente enlazada; los usuarios se almacenan en un ABB con listas enlazadas de imágenes.

## Estructuras de Datos Utilizadas
| Estructura | Uso |
|------------|-----|
| Matriz Dispersa (simplemente enlazada) | Almacena los píxeles de cada capa |
| Árbol Binario de Búsqueda (ABB) | Almacena las capas |
| Lista Circular Doblemente Enlazada | Almacena las imágenes |
| Árbol Binario de Búsqueda (ABB) | Almacena los usuarios |
| Lista Simplemente Enlazada | Lista de imágenes por imagen y por usuario |

## Requisitos
- g++ (C++17 o superior)
- Graphviz (`dot`)
- Sistema operativo: Linux/macOS/Windows (con WSL)

## Compilación
```bash
make
```
O manualmente:
```bash
g++ -std=c++17 src/main.cpp src/estructuras.cpp src/graficador.cpp src/cargador.cpp -o generador
```

## Ejecución
```bash
./generador
```

## Archivos de Datos
Los archivos de ejemplo se encuentran en `datos/`:

### Capas (`*.cap`)
```
1 {
fila,columna,color;
...
}
```

### Imágenes (`*.im`)
```
id{idcapa,idcapa,...}
```

### Usuarios (`*.usr`)
```
nombre:idimagen,idimagen,...;
```

## Orden de Carga Masiva
1. Capas (`.cap`)
2. Imágenes (`.im`)
3. Usuarios (`.usr`)

## Funcionalidades
- **Carga Masiva**: Analizador léxico para los 3 tipos de archivo
- **Generación de imágenes**: Por recorrido (preorden/inorden/postorden), por imagen, por capa, por usuario
- **CRUD**: Usuarios e Imágenes
- **Estado de Memoria (Graphviz)**:
  - Lista circular de imágenes
  - Árbol ABB de capas
  - Matriz dispersa de una capa
  - Imagen + árbol de capas con punteros
  - Árbol ABB de usuarios

## Organización del Código
```
src/
  estructuras.h / .cpp   — Definición e implementación de todas las estructuras
  graficador.h / .cpp    — Generación de gráficas DOT + PNG con Graphviz
  cargador.h / .cpp      — Analizador léxico y carga masiva de archivos
  main.cpp               — Menú principal e interfaz de usuario
datos/
  capas.cap              — Ejemplo de capas predefinidas
  imagenes.im            — Ejemplo de imágenes predefinidas
  datos.usr              — Ejemplo de usuarios predefinidos
graficas/                — Imágenes PNG generadas por Graphviz
Makefile
```
