#include "ClienteChat.h"
#include <iostream>
#include <fstream>

#include <unistd.h>
#include <arpa/inet.h>
#include <thread>
#include <cstring>


void appendToFile(const std::string& fileName, const std::string& text) {
    std::ofstream file;
    
    // Open the file in append mode
    file.open(fileName, std::ios_base::app);
    
    if (file.is_open()) {
        file << text << std::endl;  // Append the text to the file
        file.close();  // Close the file
        return;
    } else {
        std::cerr << "Unable to open file." << std::endl;
        return;
    }
}


#ifdef accounting
std::string parsetext(const std::string& input) {
    // Find the position of the first colon
    size_t pos = input.find(':');
    
    // Check if a colon was found
    if (pos != std::string::npos) {
        // Return the substring starting after the first colon
        return input.substr(pos + 1);
    } else {
        // If no colon was found, return the original string
        return input;
    }
}

#endif

#ifdef productividad
std::vector<std::string> parsetext(const std::string& input) {
    //dont tested
    std::vector<std::string> result;
    std::stringstream ss(input);
    std::string line;

    // Split the input string by newline character
    while (std::getline(ss, line, '\n')) {
        // Find the position of the first colon
        size_t colonPos = line.find(':');
        
        // If a colon is found, create a substring from the character after the first colon to the end
        if (colonPos != std::string::npos) {
            result.push_back(line.substr(colonPos + 1));
        }
    }

    return result;
}
#endif




void parse(const std::string& fileName,const std::string& input) {
    //dont tested
    #ifdef productividad
    auto output =parsetext(const std::string& input);
    appendToFile(fileName, output[0]);
    appendToFile(fileName, output[1]);
    #endif
    #ifdef accounting
    auto output =parsetext(const std::string& input);
    appendToFile(fileName, output);
    #endif

}


// Constructor que inicializa la dirección IP y el puerto del servidor
ClienteChat::ClienteChat(const std::string& direccionIP, int puerto)
    : direccionIP(direccionIP), puerto(puerto), descriptorCliente(-1), conectado(false) {}

// Método para conectar al servidor
void ClienteChat::conectarAlServidor() {
    // Crear el socket del cliente
    descriptorCliente = socket(AF_INET, SOCK_STREAM, 0);
    if (descriptorCliente == -1) {
        std::cerr << "Error al crear el socket del cliente.\n";
        return;
    }

    sockaddr_in direccionServidor;
    direccionServidor.sin_family = AF_INET;
    direccionServidor.sin_port = htons(puerto);
    inet_pton(AF_INET, direccionIP.c_str(), &direccionServidor.sin_addr);

    // Conectar al servidor
    if (connect(descriptorCliente, (sockaddr*)&direccionServidor, sizeof(direccionServidor)) == -1) {
        std::cerr << "Error al conectar al servidor.\n";
        return;
    }

    conectado = true;

    // Iniciar un hilo para recibir mensajes del servidor
    std::thread hiloRecibir(&ClienteChat::recibirMensajes, this);
    hiloRecibir.detach();
}

// Método para manejar los comandos del usuario y enviarlos al servidor
void ClienteChat::manejarComando(const std::string& comando) {
    if (conectado) {
        send(descriptorCliente, comando.c_str(), comando.size(), 0);
    }
}

// Método para desconectar del servidor
void ClienteChat::desconectar() {
    if (conectado) {
        close(descriptorCliente);
        conectado = false;
    }
}

// Método para recibir mensajes del servidor
void ClienteChat::recibirMensajes() {
    char buffer[1024];
    while (conectado) {
        memset(buffer, 0, sizeof(buffer));
        ssize_t bytesRecibidos = recv(descriptorCliente, buffer, 1024, 0);
        if (bytesRecibidos <= 0) {
            std::cerr << "Desconectado del servidor.\n";
            desconectar();
            break;
        }
        std::cout << std::string(buffer, bytesRecibidos) << std::endl;
    }
}


void ClienteChat::guardarMensajes(const std::string& fileName) {
    //dont tested
    char buffer[1024];
    while (conectado) {
        memset(buffer, 0, sizeof(buffer));
        ssize_t bytesRecibidos = recv(descriptorCliente, buffer, sizeof(buffer), 0);
        if (bytesRecibidos <= 0) {
            std::cerr << "Desconectado del servidor.\n";
            desconectar();
            break;
        }
        //std::string mensaje(buffer, bytesRecibidos);

        std::string mensaje=std::string(buffer, bytesRecibidos);;
        std::cout<<mensaje<<"\n";
        parse(fileName, mensaje);
    }
    //return mensaje;
}
