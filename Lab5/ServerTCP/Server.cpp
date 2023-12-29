/*
Author: Arati Ganesh
Class: ECE6122 
Last Date Modified: 11/18/2023
Description: Contains all the utility functions related to the TCP Server
*/

#include <iostream>
#include <vector>
#include <thread>
#include <mutex>
#include <SFML/Network.hpp>
#include <cstring>
#include <atomic>

std::string lastReceivedMessage;
std::mutex messageMutex;
std::vector<std::thread> clientThreads;
std::atomic<bool> serverRunning(true);

struct tcpMessage {
    unsigned char nVersion;
    unsigned char nType;
    unsigned short nMsgLen;
    char chMsg[1000];
};

struct ClientInfo {
    sf::TcpSocket* socket;
    std::string lastMessage;
};

std::vector<ClientInfo> clients;
std::mutex clientsMutex;

void broadcastMessage(const tcpMessage& message, const sf::TcpSocket* senderSocket) {
    std::lock_guard<std::mutex> lock(clientsMutex);
    for (auto& client : clients) {
        if (client.socket != senderSocket) {
            client.socket->send(&message, sizeof(message));
        }
    }
}

void handleClient(sf::TcpSocket* clientSocket) {
    if (clientSocket == nullptr) return;

    sf::SocketSelector selector;
    selector.add(*clientSocket);

    while (serverRunning) {
        if (selector.wait(sf::milliseconds(100))) {
            tcpMessage inMsg;
            std::size_t received;
            sf::Socket::Status status = clientSocket->receive(&inMsg, sizeof(inMsg), received);

            if (status == sf::Socket::Done) {
                if (static_cast<int>(inMsg.nVersion) == 102) {
                    std::lock_guard<std::mutex> lock(messageMutex);
                    lastReceivedMessage = std::string(inMsg.chMsg, inMsg.nMsgLen);

                    if (static_cast<int>(inMsg.nType) == 77) {
                        broadcastMessage(inMsg, clientSocket);
                    } else if (static_cast<int>(inMsg.nType) == 201) {
                        std::string messageStr(inMsg.chMsg, inMsg.nMsgLen);
                        std::reverse(messageStr.begin(), messageStr.end());
                        std::strncpy(inMsg.chMsg, messageStr.c_str(), sizeof(inMsg.chMsg));
                        inMsg.nMsgLen = static_cast<unsigned short>(messageStr.size());
                        clientSocket->send(&inMsg, sizeof(inMsg));
                    }
                }
            } else if (status == sf::Socket::Disconnected) {
                break; // Exit loop if client is disconnected
            }
        }
    }

    clientSocket->disconnect();
    std::lock_guard<std::mutex> lock(clientsMutex);
    clients.erase(std::remove_if(clients.begin(), clients.end(), 
                                 [&clientSocket](const ClientInfo& client) { 
                                     return client.socket == clientSocket; 
                                 }), 
                  clients.end());
    delete clientSocket;
}

void acceptClients(sf::TcpListener& listener) {
    listener.setBlocking(false);

    while (serverRunning) {
        sf::TcpSocket* clientSocket = new sf::TcpSocket;
        if (listener.accept(*clientSocket) == sf::Socket::Done) {
            std::lock_guard<std::mutex> lock(clientsMutex);
            clients.push_back({clientSocket, ""});
            clientThreads.push_back(std::thread(handleClient, clientSocket));
        } else {
            delete clientSocket;
        }
    }
}

void runTcpServer(unsigned short port) {
    sf::TcpListener listener;
    if (listener.listen(port) != sf::Socket::Done) {
        std::cerr << "Failed to listen on port " << port << std::endl;
        return;
    }

    std::thread clientAcceptThread(acceptClients, std::ref(listener));

    std::string command;
    while (true) {
        std::cout << "Please enter command: ";
        std::getline(std::cin, command);

        if (command == "msg") {
            std::lock_guard<std::mutex> lock(messageMutex);
            std::cout << "Last message: " << lastReceivedMessage << std::endl;
        } else if (command == "clients") {
            std::lock_guard<std::mutex> lock(clientsMutex);
            std::cout << "Number of Clients: " << clients.size() << std::endl;
            for (const auto& client : clients) {
                std::cout << "IP Address: " << client.socket->getRemoteAddress() 
                          << " | Port: " << client.socket->getRemotePort() << std::endl;
            }
        } else if (command == "exit") {
            serverRunning = false;
            listener.close();
            break;
        }
    }

    clientAcceptThread.join();
    for (auto& thread : clientThreads) {
        if (thread.joinable()) thread.join();
    }
}

int main() {
    const unsigned short port = 50001;
    runTcpServer(port);
    return 0;
}