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

PORT ?= 12346

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
run-server: $(TARGET)
	./$(TARGET) server $(PORT)

# Ejecutar el cliente
run-client: $(TARGET)
	./$(TARGET) client $(PORT) 127.0.0.1 


# Ejecutar el middle servidor
run-middle-server: $(TARGET)
	./$(TARGET) server-middle $(PORT)

# Ejecutar el middle cliente
run-middle-client: $(TARGET)
	./$(TARGET) client-middle 127.0.0.1  $(PORT)


# Ejecutar el middle
run-middle: $(TARGET)
	./$(TARGET) middle 12345 127.0.0.1  $(PORT)



# Declarar reglas como phony
.PHONY: all clean run-servidor run-cliente
