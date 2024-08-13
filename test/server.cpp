#include <iostream>
#include <thread>
#include <netinet/in.h>
#include <unistd.h>
#include <cstring>

void handleClient(int clientSocket) {
    char buffer[1024];
    ssize_t bytesRead;
    while ((bytesRead = recv(clientSocket, buffer, sizeof(buffer), 0)) > 0) {
        buffer[bytesRead] = '\0';
        std::cout << "Received message: " << buffer << std::endl;
        send(clientSocket, buffer, bytesRead, 0);
    }
    close(clientSocket);
}

void startServer(int port) {
    int serverSocket = socket(AF_INET, SOCK_STREAM, 0);
    sockaddr_in serverAddr;
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_addr.s_addr = INADDR_ANY;
    serverAddr.sin_port = htons(port);

    bind(serverSocket, (sockaddr*)&serverAddr, sizeof(serverAddr));
    listen(serverSocket, 5);

    while (true) {
        int clientSocket = accept(serverSocket, nullptr, nullptr);
        std::thread(handleClient, clientSocket).detach();
    }
}

int main() {
    std::thread serverThread1(startServer, 5000);
    std::thread serverThread2(startServer, 5001);
    std::thread serverThread3(startServer, 5002);

    serverThread1.join();
    serverThread2.join();
    serverThread3.join();

    return 0;
}