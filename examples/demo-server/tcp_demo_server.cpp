#include <iostream>
#include <cstring>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>

int main(int argc, char** argv) {
    int port = 9000;
    if (argc > 1) port = std::stoi(argv[1]);

    int sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock < 0) {
        std::cerr << "Failed to create socket: " << strerror(errno) << "\n";
        return 1;
    }

    int opt = 1;
    setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    struct sockaddr_in addr;
    std::memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = INADDR_ANY;
    addr.sin_port = htons(port);

    if (bind(sock, (struct sockaddr*)&addr, sizeof(addr)) < 0) {
        std::cerr << "Bind failed: " << strerror(errno) << "\n";
        return 1;
    }

    if (listen(sock, 10) < 0) {
        std::cerr << "Listen failed: " << strerror(errno) << "\n";
        return 1;
    }

    std::cout << "GatewayForge TCP Demo Server listening on port " << port << "...\n";

    while (true) {
        struct sockaddr_in client_addr;
        socklen_t client_len = sizeof(client_addr);
        int client = accept(sock, (struct sockaddr*)&client_addr, &client_len);
        if (client < 0) continue;

        std::cout << "[TCP] Client connected from "
                  << inet_ntoa(client_addr.sin_addr) << ":"
                  << ntohs(client_addr.sin_port) << "\n";

        uint8_t buf[4096];
        while (true) {
            ssize_t n = recv(client, buf, sizeof(buf), 0);
            if (n <= 0) break;
            if (n >= 10) {
                uint16_t magic = (buf[0] << 8) | buf[1];
                uint8_t version = buf[2];
                uint8_t msg_type = buf[3];
                uint32_t seq = (buf[4] << 24) | (buf[5] << 16) | (buf[6] << 8) | buf[7];
                uint16_t payload_len = (buf[8] << 8) | buf[9];
                std::cout << "  msg_type=" << (int)msg_type
                          << " seq=" << seq
                          << " payload_len=" << payload_len
                          << " total=" << n << " bytes\n";

                // Echo basic ACK
                uint8_t ack[] = {0xCA, 0xFE, version, 0x02, 0, 0, 0, (uint8_t)(seq + 1), 0, 0};
                send(client, ack, sizeof(ack), 0);
            }
        }

        std::cout << "[TCP] Client disconnected\n";
        close(client);
    }

    close(sock);
    return 0;
}
