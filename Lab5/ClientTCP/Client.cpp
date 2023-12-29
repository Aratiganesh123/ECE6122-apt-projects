/*
Author: Arati Ganesh
Class: ECE6122 
Last Date Modified: 11/18/2023
Description: Contains all the utility functions related to the TCP Client
*/

#include <iostream>
#include <cstdlib>
#include <cstring>
#include <SFML/Network.hpp>
#include <thread>

struct tcpMessage {
    unsigned char nVersion;
    unsigned char nType;
    unsigned short nMsgLen;
    char chMsg[1000];
};

sf::Socket::Status sendComplete(sf::TcpSocket& socket, const void* data, size_t size) {
    const char* dataPtr = static_cast<const char*>(data);
    size_t totalSent = 0;

    while (totalSent < size) {
        std::size_t sent;
        sf::Socket::Status status = socket.send(dataPtr + totalSent, size - totalSent, sent);
        if (status != sf::Socket::Done && status != sf::Socket::Partial) {
            return status;  // Return on any error
        }
        totalSent += sent;
    }

    return sf::Socket::Done;
}

void receiveMessages(sf::TcpSocket& socket, bool& running) {
    while (running) {
        tcpMessage inMsg;
        std::size_t received;
        sf::Socket::Status status = socket.receive(&inMsg, sizeof(inMsg), received);

        if (status == sf::Socket::Done) {
            std::cout << "Received Msg Type: " << static_cast<int>(inMsg.nType)
                << "; Msg: " << inMsg.chMsg << std::endl;
        }
        else if (status == sf::Socket::NotReady) {
            // No data available, yield to other threads
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
        }
        else if (status == sf::Socket::Disconnected || status == sf::Socket::Error) {
            std::cerr << "Socket disconnected or error occurred" << std::endl;
            running = false;
            
        }
    }
}

void runTcpClient(const sf::IpAddress& server, unsigned short port) {
    sf::TcpSocket socket;
    if (socket.connect(server, port) != sf::Socket::Done) {
        std::cerr << "Failed to connect to server " << server << std::endl;
        return;
    }

    socket.setBlocking(false);  // Set the socket to non-blocking mode

    std::cout << "Connected to server " << server << std::endl;

    unsigned char version = 0;
    unsigned char type = 0;
    std::string command, message;
    bool running = true; // Declare the running variable

    // Start the receiver thread
    std::thread receiverThread(receiveMessages, std::ref(socket), std::ref(running));

    while (running) {
        std::cout << "Please enter command: ";
        std::getline(std::cin, command);

        if (command.empty()) continue;

        if (command[0] == 'v' && command.size() > 2) {
            version = static_cast<unsigned char>(std::stoi(command.substr(2)));
        }
        else if (command[0] == 't' && command.size() > 2) {
            size_t spacePos = command.find(' ', 2);
            if (spacePos != std::string::npos) {
                type = static_cast<unsigned char>(std::stoi(command.substr(2, spacePos - 2)));
                message = command.substr(spacePos + 1);

                tcpMessage tcpMsg;
                tcpMsg.nVersion = version;
                tcpMsg.nType = type;
                tcpMsg.nMsgLen = static_cast<unsigned short>(message.size());
                std::strncpy(tcpMsg.chMsg, message.c_str(), sizeof(tcpMsg.chMsg));

                if (sendComplete(socket, &tcpMsg, sizeof(tcpMsg)) != sf::Socket::Done) {
                    std::cerr << "Failed to send message" << std::endl;
                }
            }
        }
        else if (command == "q") {
            running = false;
            break;
        }
    }

    std::cout << "Printed exit " << std::endl;
    if (receiverThread.joinable()) {
        receiverThread.join();
    }

    socket.disconnect();
}

int main(int argc, char* argv[])
{
    if (argc != 3) {
        std::cerr << "Usage: " << argv[0] << " <server_ip> <port>" << std::endl;
        return EXIT_FAILURE;
    }

    const sf::IpAddress server(argv[1]);
    const unsigned short port = static_cast<unsigned short>(std::stoi(argv[2]));

    runTcpClient(server, port);
    return EXIT_SUCCESS;
}
