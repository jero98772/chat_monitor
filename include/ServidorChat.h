#ifndef SERVIDORCHAT_H
#define SERVIDORCHAT_H

#include <string>
#include <vector>
#include <mutex>
#include "Usuario.h"

class ServidorChat {
public:
    ServidorChat(int puerto);
    void iniciar();
    void manejarComando(const std::string& comando);
    void manejarMensajeDeServidor(const std::string& mensaje);
private:
    int puerto;
    int descriptorServidor;
    std::vector<std::string> servidoresFederados; // List of federated servers
    std::mutex mutexUsuarios;
    std::vector<Usuario> usuarios; // Correct
    void manejarCliente(int descriptorCliente);     
    void aceptarConexiones();
    void gestionarConexiones();
    void comunicacionEntreServidores(const std::string& direccion);
    void enviarMensajeATodos(const std::string& mensaje, int descriptorRemitente);
    void enviarListaUsuarios(int descriptorCliente);
    void enviarDetallesConexion(int descriptorCliente);
    void conectarAServidoresFederados(); // Connect to federated servers

    void broadcastMessage(const std::string& mensaje);
    void processCommand(const std::string& comando);
};

#endif // SERVIDORCHAT_H
