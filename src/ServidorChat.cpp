#include "ServidorChat.h"
#include <iostream>
#include <unistd.h>
#include <arpa/inet.h>
#include <thread>
#include <cstring>
#include <sstream>


// Constructor que inicializa el puerto del servidor
ServidorChat::ServidorChat(int puerto)
    : puerto(puerto), descriptorServidor(-1) {}


// Manejar la comunicación con un cliente
void ServidorChat::manejarCliente(int descriptorCliente) {
    char buffer[1024];
    std::string nombreUsuario;

    // Solicitar el nombre del usuario
    send(descriptorCliente, "Ingrese su nombre: ", 20, 0);
    ssize_t bytesRecibidos = recv(descriptorCliente, buffer, 1024, 0);
    if (bytesRecibidos <= 0) {
        close(descriptorCliente);
        return;
    }

    nombreUsuario = std::string(buffer, bytesRecibidos);
    nombreUsuario.erase(nombreUsuario.find_last_not_of(" \n\r\t") + 1); // Eliminar espacios en blanco

    {
        std::lock_guard<std::mutex> lock(mutexUsuarios);
        usuarios.emplace_back(nombreUsuario, descriptorCliente);
    }

    // Notificar a todos los usuarios que un nuevo usuario se ha conectado
    std::string mensajeBienvenida = nombreUsuario + " se ha conectado al chat.\n";
    enviarMensajeATodos(mensajeBienvenida, descriptorCliente);

    // Manejar los mensajes del cliente
    while (true) {
        memset(buffer, 0, sizeof(buffer));
        bytesRecibidos = recv(descriptorCliente, buffer, 1024, 0);

        if (bytesRecibidos <= 0) {
            // El cliente se ha desconectado
            {
                std::lock_guard<std::mutex> lock(mutexUsuarios);
                for (auto it = usuarios.begin(); it != usuarios.end(); ++it) {
                    if (it->obtenerDescriptorSocket() == descriptorCliente) {
                        std::string mensajeDespedida = it->obtenerNombreUsuario() + " se ha desconectado del chat.\n";
                        enviarMensajeATodos(mensajeDespedida, descriptorCliente);
                        usuarios.erase(it);
                        break;
                    }
                }
            }
            close(descriptorCliente);
            break;
        }

        std::string mensaje = std::string(buffer, bytesRecibidos);

        // Procesar comandos del protocolo
        if (mensaje.substr(0, 9) == "@usuarios") {
            enviarListaUsuarios(descriptorCliente);
        } else if (mensaje.substr(0, 9) == "@conexion") {
            enviarDetallesConexion(descriptorCliente);
        } else if (mensaje.substr(0, 6) == "@salir") {
            close(descriptorCliente);
            break;
        } else if (mensaje.substr(0, 2) == "@h") {
            std::string ayuda = "Comandos disponibles:\n"
                                "@usuarios - Lista de usuarios conectados\n"
                                "@conexion - Muestra la conexión y el número de usuarios\n"
                                "@salir - Desconectar del chat\n";
            send(descriptorCliente, ayuda.c_str(), ayuda.size(), 0);
        } else {
            // Enviar el mensaje a todos los usuarios
            mensaje = nombreUsuario + ": " + mensaje;
            enviarMensajeATodos(mensaje, descriptorCliente);
        }
    }
}

// Enviar un mensaje a todos los usuarios conectados, excepto al remitente
void ServidorChat::enviarMensajeATodos(const std::string& mensaje, int descriptorRemitente) {
    std::lock_guard<std::mutex> lock(mutexUsuarios);
    for (const auto& usuario : usuarios) {
        if (usuario.obtenerDescriptorSocket() != descriptorRemitente) {
            send(usuario.obtenerDescriptorSocket(), mensaje.c_str(), mensaje.size(), 0);
        }
    }
}

// Enviar la lista de usuarios conectados al cliente especificado
void ServidorChat::enviarListaUsuarios(int descriptorCliente) {
    std::lock_guard<std::mutex> lock(mutexUsuarios);
    std::string listaUsuarios = "Usuarios conectados:\n";
    for (const auto& usuario : usuarios) {
        listaUsuarios += usuario.obtenerNombreUsuario() + "\n";
    }
    send(descriptorCliente, listaUsuarios.c_str(), listaUsuarios.size(), 0);
}

// Enviar los detalles de la conexión y el número de usuarios conectados
void ServidorChat::enviarDetallesConexion(int descriptorCliente) {
    std::lock_guard<std::mutex> lock(mutexUsuarios);
    std::string detalles = "Número de usuarios conectados: " + std::to_string(usuarios.size()) + "\n";
    send(descriptorCliente, detalles.c_str(), detalles.size(), 0);
}


void ServidorChat::iniciar() {
    // Crear el socket del servidor
    descriptorServidor = socket(AF_INET, SOCK_STREAM, 0);
    if (descriptorServidor == -1) {
        std::cerr << "Error al crear el socket del servidor.\n";
        return;
    }

    sockaddr_in direccionServidor;
    direccionServidor.sin_family = AF_INET;
    direccionServidor.sin_port = htons(puerto);
    direccionServidor.sin_addr.s_addr = INADDR_ANY;

    // Asociar el socket a la dirección y puerto
    if (bind(descriptorServidor, (sockaddr*)&direccionServidor, sizeof(direccionServidor)) == -1) {
        std::cerr << "Error al hacer bind del socket del servidor.\n";
        return;
    }

    // Poner el servidor en modo escucha
    if (listen(descriptorServidor, 10) == -1) {
        std::cerr << "Error al poner el servidor en modo escucha.\n";
        return;
    }

    std::cout << "Servidor iniciado en el puerto " << puerto << ". Esperando conexiones...\n";

    // Conectar a otros servidores federados
    conectarAServidoresFederados();

    // Aceptar conexiones entrantes
    while (true) {
        sockaddr_in direccionCliente;
        socklen_t tamanoDireccionCliente = sizeof(direccionCliente);
        int descriptorCliente = accept(descriptorServidor, (sockaddr*)&direccionCliente, &tamanoDireccionCliente);

        if (descriptorCliente == -1) {
            std::cerr << "Error al aceptar la conexión de un cliente.\n";
            continue;
        }

        // Crear un hilo para manejar el cliente
        std::thread hiloCliente(&ServidorChat::manejarCliente, this, descriptorCliente);
        hiloCliente.detach();
    }
}

// Comunicación entre servidores
void ServidorChat::comunicacionEntreServidores(const std::string& direccion) {
    int descriptorServidorFederado = socket(AF_INET, SOCK_STREAM, 0);
    if (descriptorServidorFederado == -1) {
        std::cerr << "Error al crear el socket para el servidor federado.\n";
        return;
    }

    sockaddr_in direccionServidor;
    direccionServidor.sin_family = AF_INET;
    direccionServidor.sin_port = htons(puerto);
    inet_pton(AF_INET, direccion.c_str(), &direccionServidor.sin_addr);

    if (connect(descriptorServidorFederado, (sockaddr*)&direccionServidor, sizeof(direccionServidor)) == -1) {
        std::cerr << "Error al conectar con el servidor federado.\n";
        close(descriptorServidorFederado);
        return;
    }

    // Handle communication with the federated server
    char buffer[1024];
    while (true) {
        ssize_t bytesRecibidos = recv(descriptorServidorFederado, buffer, sizeof(buffer), 0);
        if (bytesRecibidos <= 0) {
            std::cerr << "Desconectado del servidor federado.\n";
            close(descriptorServidorFederado);
            break;
        }

        std::string mensaje(buffer, bytesRecibidos);
        manejarMensajeDeServidor(mensaje);
    }
}

// Método para conectar a otros servidores federados
void ServidorChat::conectarAServidoresFederados() {
    for (const auto& direccion : servidoresFederados) {
        std::thread(&ServidorChat::comunicacionEntreServidores, this, direccion).detach();
    }
}



void ServidorChat::broadcastMessage(const std::string& mensaje) {
    std::lock_guard<std::mutex> lock(mutexUsuarios);
    for (const auto& usuario : usuarios) {
            send(usuario.obtenerDescriptorSocket(), mensaje.c_str(), mensaje.size(), 0);
    }
}

// Example implementation of processing commands
void ServidorChat::processCommand(const std::string& comando) {
    // Handle commands from federated servers
    if (comando == "STATUS_UPDATE") {
        // Process status update
    } else if (comando == "USER_LIST") {
        // Process user list update
    }
    // Add more command handling as needed
}

void ServidorChat::manejarMensajeDeServidor(const std::string& mensaje) {
    // Example format: "COMMAND: details" or "MESSAGE: content"
    std::istringstream stream(mensaje);
    std::string tipoMensaje;
    std::string contenido;
    
    // Get message type and content
    std::getline(stream, tipoMensaje, ':');
    std::getline(stream, contenido);

    // Remove leading and trailing spaces from content
    contenido.erase(contenido.find_last_not_of(" \n\r\t")+1);
    
    if (tipoMensaje == "MESSAGE") {
        // Broadcast message to all users
        broadcastMessage(contenido);
    } else if (tipoMensaje == "COMMAND") {
        // Process command
        processCommand(contenido);
    } else {
        std::cerr << "Unknown message type: " << tipoMensaje << std::endl;
    }
}