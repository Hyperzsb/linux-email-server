#include <system_error>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <cstdio>
#include <cstring>

void print_error(const char *message) {
    perror(message);
    exit(EXIT_FAILURE);
}

int main() {
    int client_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (client_socket == -1)
        print_error("Error while creating socket");
    else
        printf("Successfully created socket!\n");

    struct sockaddr_in server_addr{};
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = inet_addr("127.0.0.1");
    server_addr.sin_port = htons(6000);

    if (connect(client_socket, (struct sockaddr *) &server_addr, sizeof(server_addr)) == -1)
        print_error("Error while connect to server");
    else
        printf("Successfully connected to server!\n");

    char send_buffer[2048] = {0};
    char receive_buffer[2048] = {0};
    while (fgets(send_buffer, sizeof(send_buffer), stdin) != nullptr) {
        write(client_socket, send_buffer, strlen(send_buffer));
        read(client_socket, receive_buffer, sizeof(receive_buffer));
        fputs(receive_buffer, stdout);
        memset(send_buffer, 0, sizeof(send_buffer));
        memset(receive_buffer, 0, sizeof(receive_buffer));
    }

    close(client_socket);

    return 0;
}
