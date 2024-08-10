#ifndef CLIENTECHAT_H
#define CLIENTECHAT_H

#include <string>

class ClienteChat {
public:
    ClienteChat(const std::string& direccionIP, int puerto);
    void conectarAlServidor();
    void manejarComando(const std::string& comando);
    void desconectar();
    
private:
    std::string direccionIP;
    int puerto;
    int descriptorCliente;
    bool conectado;
    void enviarMensaje(const std::string& mensaje);
    void recibirMensajes();
};

#endif // CLIENTECHAT_H
