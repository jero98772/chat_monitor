# Compilador y flags
CXX = g++
CXXFLAGS = -std=c++11 -Iinclude -Wall -Wextra

# Directorios
SRC_DIR = src
INCLUDE_DIR = include
BUILD_DIR = build

# Archivos fuente y de cabecera
SRCS = $(wildcard $(SRC_DIR)/*.cpp) main.cpp
OBJS = $(SRCS:%.cpp=$(BUILD_DIR)/%.o)

# Archivo ejecutable
TARGET = $(BUILD_DIR)/chat

# Regla por defecto
all: $(TARGET)

# Regla para crear el directorio de compilación
$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)/$(SRC_DIR)

# Regla para compilar el ejecutable
$(TARGET): $(OBJS)
	$(CXX) $(CXXFLAGS) -o $@ $^

# Regla para compilar los archivos objeto
$(BUILD_DIR)/%.o: %.cpp | $(BUILD_DIR)
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Limpiar archivos compilados
clean:
	rm -rf $(BUILD_DIR)

# Ejecutar el servidor
run-servidor: $(TARGET)
	./$(TARGET) servidor $(PORT)

# Ejecutar el cliente
run-cliente: $(TARGET)
	./$(TARGET) cliente $(ADDRESS) $(PORT)

# Ejecutar el cliente
run-sc: $(TARGET)
	./$(TARGET) superclient $(FILE)

#./build/chat servidor 6666
#./build/chat servidor 9999
#./build/chat servidor 3333

# Declarar reglas como phony
.PHONY: all clean run-servidor run-cliente
