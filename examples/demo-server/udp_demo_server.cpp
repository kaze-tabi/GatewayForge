#include <iostream>
#include <cstring>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>

int main(int argc, char** argv) {
    int port = 9001;
    if (argc > 1) port = std::stoi(argv[1]);

    int sock = socket(AF_INET, SOCK_DGRAM, 0);
    if (sock < 0) {
        std::cerr << "Failed to create socket: " << strerror(errno) << "\n";
        return 1;
    }

    struct sockaddr_in addr;
    std::memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = INADDR_ANY;
    addr.sin_port = htons(port);

    if (bind(sock, (struct sockaddr*)&addr, sizeof(addr)) < 0) {
        std::cerr << "Bind failed: " << strerror(errno) << "\n";
        return 1;
    }

    std::cout << "GatewayForge UDP Demo Server listening on port " << port << "...\n";

    uint8_t buf[4096];
    struct sockaddr_in client_addr;
    socklen_t client_len = sizeof(client_addr);

    while (true) {
        ssize_t n = recvfrom(sock, buf, sizeof(buf), 0,
                             (struct sockaddr*)&client_addr, &client_len);
        if (n < 0) continue;

        std::cout << "[UDP] Packet from "
                  << inet_ntoa(client_addr.sin_addr) << ":"
                  << ntohs(client_addr.sin_port)
                  << " (" << n << " bytes) ";

        if (n >= 10) {
            uint16_t magic = (buf[0] << 8) | buf[1];
            if (magic == 0xCAFE) {
                uint8_t version = buf[2];
                uint8_t msg_type = buf[3];
                uint64_t device_id = 0;
                for (int i = 0; i < 8; i++) device_id = (device_id << 8) | buf[4 + i];
                uint32_t seq = (buf[12] << 24) | (buf[13] << 16) | (buf[14] << 8) | buf[15];
                std::cout << "device=" << device_id
                          << " msg_type=" << (int)msg_type
                          << " seq=" << seq;
            }
        }
        std::cout << "\n";
    }

    close(sock);
    return 0;
}
