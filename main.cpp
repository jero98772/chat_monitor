#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include <thread>
#include <chrono>
#include <iostream>
#include <functional>

#include "ClienteChat.h"
#include "ServidorChat.h"


//////////////////////////////////////
//DEFINES 
//#define banco
#ifdef banco


#endif 

#define productividad
#ifdef productividad


#endif 





//////////////////////////////////////


//////////////////////////////////////
//  AUXILIAR FUNCTIONS

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

void recibirMensajesDeServidor(ClienteChat& cliente) {
    cliente.recibirMensajes();  // Start receiving messages from the server
}


void enviarComandos(std::vector<ClienteChat>& clientes) {
    std::string mensaje;
    while (std::getline(std::cin, mensaje)) {
        std::cout<< "while"<<"\n";
        for (auto& cliente : clientes) {
            std::cout<< "for"<<"\n";
            if(mensaje=="/exit"){
                std::cout<< mensaje<<"\n";
                cliente.desconectar();//if i dont desconect myself it will give a error when you try to conectate againg with the same port
                //exit(0);
            }else{
            //comands of the macros #ifdef ... #endif
            #ifdef banco


            #endif
            #ifdef productividad


            #endif
            }
            cliente.manejarComando(mensaje);
        }
    }
}

void recibirMensajesDeTodos(std::vector<ClienteChat>& clientes, const std::string& fileName) {
    std::vector<std::thread> recvThreads;
    for (auto& cliente : clientes) {
        recvThreads.emplace_back(&ClienteChat::guardarMensajes, &cliente, fileName);
    }
    
    // Join all receiving threads
    for (auto& t : recvThreads) {
        t.join();
    }
}

//////////////////////////////////////

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
        std::vector<std::thread> threads;
        std::vector<ClienteChat> clientes;

        for (const auto& row : conections) {
            std::cout << row[0] << " : " << row[1] << std::endl;
            int puerto = std::stoi(row[1]);


            ClienteChat cliente(row[0], puerto);  // Inicializa el cliente con la dirección IP y puerto proporcionados
            cliente.conectarAlServidor();  // Conecta al servidor
            std::this_thread::sleep_for(std::chrono::microseconds(100));

            std::string mensaje;
            mensaje="monitor";
            cliente.manejarComando(mensaje);  // Envía el mensaje al servidor
            
            /*
            std::this_thread::sleep_for(std::chrono::microseconds(100));
            mensaje=":hola";
            cliente.manejarComando(mensaje);  // Envía el mensaje al servidor
            std::this_thread::sleep_for(std::chrono::microseconds(100));
            mensaje="mi nombre es fraile";
            cliente.manejarComando(mensaje);  // Envía el mensaje al servidor
            */
            clientes.push_back(cliente);
            //threads.emplace_back(recibirMensajesDeServidor, std::ref(cliente));

            

        }
        for (auto& thread : threads) {
            thread.join();
        }


        std::thread sendThread(enviarComandos, std::ref(clientes));

        auto threadFunc = std::bind(recibirMensajesDeTodos, std::placeholders::_1, "data.txt");
        std::thread recvThread(threadFunc, std::ref(clientes));
        sendThread.join();
        recvThread.join();

        /*


        */
    } else {
        std::cerr << "Modo desconocido: " << modo << "\n";
        return 1;
    }

    return 0;
}
