#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>

#include "ClienteChat.h"
#include "ServidorChat.h"



std::vector<std::vector<std::string>> readAndParseFile(const std::string& filename) {
    std::ifstream file(filename);
    std::vector<std::vector<std::string>> matrix;
    std::string line;

    if (!file.is_open()) {
        std::cerr << "Error opening file: " << filename << std::endl;
        return matrix;
    }

    while (std::getline(file, line)) {
        std::stringstream ss(line);
        std::string value1, value2;

        if (std::getline(ss, value1, ':') && std::getline(ss, value2)) {
            matrix.push_back({value1, value2});
        }
    }

    file.close();
    return matrix;
}

int main(int argc, char* argv[]) {
    if (argc < 2) {
        std::cerr << "Uso: " << argv[0] << " <modo> <direccionIP> <puerto>\n";
        std::cerr << "Modos disponibles: servidor, cliente\n";
        return 1;
    }

    std::string modo = argv[1];

    if (modo == "servidor") {
        if (argc < 3) {
            std::cerr << "Uso: " << argv[0] << " servidor <puerto>\n";
            return 1;
        }
        int puerto = std::stoi(argv[2]);
        ServidorChat servidor(puerto);  // Inicializa el servidor con el puerto proporcionado
        servidor.iniciar();  // Inicia el servidor
    } else if (modo == "cliente") {
        if (argc < 4) {
            std::cerr << "Uso: " << argv[0] << " cliente <direccionIP> <puerto>\n";
            return 1;
        }
        std::string direccionIP = argv[2];
        int puerto = std::stoi(argv[3]);
        ClienteChat cliente(direccionIP, puerto);  // Inicializa el cliente con la dirección IP y puerto proporcionados
        cliente.conectarAlServidor();  // Conecta al servidor

        std::string mensaje;
        while (std::getline(std::cin, mensaje)) {
            cliente.manejarComando(mensaje);  // Envía el mensaje al servidor
        }

        cliente.desconectar();  // Desconecta del servidor
    }else if (modo == "superclient") {
        if (argc < 3) {
            std::cerr << "Uso: " << argv[1] << " supercliente <lista servidores.txt>\n";
            std::cerr << "Uso: " << argv[1] << argv[2];
            return 1;
        }

        std::string file = argv[2];
        auto conections=readAndParseFile(file);
        for (const auto& row : conections) {
            std::cout << row[0] << " : " << row[1] << std::endl;
            int puerto = std::stoi(row[1]);
            ClienteChat cliente(row[0], puerto);  // Inicializa el cliente con la dirección IP y puerto proporcionados
            cliente.conectarAlServidor();  // Conecta al servidor
        std::string mensaje;
        while (std::getline(std::cin, mensaje)) {
            cliente.manejarComando(mensaje);  // Envía el mensaje al servidor
        }
        cliente.desconectar();  // Desconecta del servidor
        }
        /*


        */
    } else {
        std::cerr << "Modo desconocido: " << modo << "\n";
        return 1;
    }

    return 0;
}
