#include <iostream>
#include <sys/socket.h>
#include <netinet/in.h>
#include <cstring>
#include <unistd.h>
#include <arpa/inet.h> // Include this for inet_pton

void sendMessage(int port, const std::string& message) {
    int clientSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (clientSocket < 0) {
        std::cerr << "Socket creation failed\n";
        return;
    }

    sockaddr_in serverAddr;
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(port);

    // Convert IP address from text to binary form
    if (inet_pton(AF_INET, "127.0.0.1", &serverAddr.sin_addr) <= 0) {
        std::cerr << "Invalid address / Address not supported\n";
        close(clientSocket);
        return;
    }

    if (connect(clientSocket, (sockaddr*)&serverAddr, sizeof(serverAddr)) < 0) {
        std::cerr << "Connection failed\n";
        close(clientSocket);
        return;
    }

    send(clientSocket, message.c_str(), message.size(), 0);

    char buffer[1024];
    ssize_t bytesRead = recv(clientSocket, buffer, sizeof(buffer), 0);
    if (bytesRead > 0) {
        buffer[bytesRead] = '\0';
        std::cout << "Received response: " << buffer << std::endl;
    } else {
        std::cerr << "Failed to receive response\n";
    }

    close(clientSocket);
}

int main() {
    sendMessage(5000, "Hello from client to server 5000!");
    sendMessage(5003, "Hello from client to server 5001!");
    sendMessage(5002, "Hello from client to server 5002!");

    return 0;
}
