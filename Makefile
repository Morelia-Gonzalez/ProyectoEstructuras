CXX = g++
CXXFLAGS = -std=c++17 -Wall -Wextra -g
TARGET = generador
SRCDIR = src
SOURCES = $(SRCDIR)/main.cpp $(SRCDIR)/estructuras.cpp $(SRCDIR)/graficador.cpp $(SRCDIR)/cargador.cpp
HEADERS = $(SRCDIR)/estructuras.h $(SRCDIR)/graficador.h $(SRCDIR)/cargador.h

all: $(TARGET)

$(TARGET): $(SOURCES) $(HEADERS)
	$(CXX) $(CXXFLAGS) $(SOURCES) -o $(TARGET)
	@echo "Compilacion exitosa: ./$(TARGET)"

clean:
	rm -f $(TARGET) graficas/*.dot graficas/*.png

.PHONY: all clean
