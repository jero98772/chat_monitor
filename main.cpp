#include "ServidorChat.h"
#include "ClienteChat.h"
#include <iostream>
#include <string>
#include <thread>

// Function to start the chat server
void iniciarServidor(int puerto) {
    ServidorChat servidor(puerto);
    servidor.iniciar();
}

// Function to start the chat client
void iniciarCliente(const std::string& direccionIP, int puerto) {
    ClienteChat cliente(direccionIP, puerto);
    cliente.conectarAlServidor();

    std::string comando;
    while (true) {
        std::getline(std::cin, comando);
        if (comando == "@salir") {
            cliente.desconectar();
            break;
        }
        cliente.manejarComando(comando);
    }
}

int main(int argc, char* argv[]) {
    if (argc < 3) {
        std::cerr << "Uso: " << argv[0] << " <modo> <puerto> [direccionIP]" << std::endl;
        std::cerr << "Modos disponibles: servidor, cliente" << std::endl;
        return 1;
    }

    std::string modo = argv[1];
    int puerto = std::stoi(argv[2]);

    if (modo == "server") {
        std::thread servidorThread(iniciarServidor, puerto);
        servidorThread.join();
    }else if (modo == "client") {
        if (argc < 4) {
            std::cerr << "Falta la dirección IP del servidor para el modo cliente." << std::endl;
            return 1;
        }
        std::string direccionIP = argv[3];
        std::thread clienteThread(iniciarCliente, direccionIP, puerto);
        clienteThread.join();
    }else if (modo == "server-middle") {

        std::thread servidorThread(iniciarServidor, puerto);
        servidorThread.join();
    }else if (modo == "client-middle") {
        if (argc < 4) {
            std::cerr << "Falta la dirección IP del servidor para el modo cliente." << std::endl;
            return 1;
        }
        std::string direccionIP = argv[3];
        std::thread clienteThread(iniciarCliente, direccionIP, puerto);
        clienteThread.join();

    }else if (modo == "middle") {
        int puertoServidor = std::stoi(argv[2]);
        std::string direccionIP = argv[3];
        int puertoCliente = std::stoi(argv[4]);

        // Start the server in a separate thread
        std::thread servidorThread(iniciarServidor, puertoServidor);

        std::thread clienteThread(iniciarCliente, direccionIP, puertoCliente);

        // Join the server thread to wait for its completion
        servidorThread.join();
    } else {
        std::cerr << "Modo no válido. Use 'servidor' o 'cliente'." << std::endl;
        return 1;
    }

    return 0;
}
