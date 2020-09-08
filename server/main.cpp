#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <cstdio>
#include <cstring>

int main() {
    int listen_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (listen_fd == -1)
        printf("Error while creating socket!\n");
    else
        printf("Successfully created socket!\n");

    struct sockaddr_in server_addr{};
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = inet_addr("127.0.0.1");
    server_addr.sin_port = htons(6000);

    // 在初始化之前，设置 socket 选项，使得 socket 不必等待地址 TIME_WAIT 状态释放。
    int on = 1;
    if (setsockopt(listen_fd, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on)) == -1)
        printf("Error while setting socket option!\n");
    else
        printf("Successfully set socket option!\n");

    if (bind(listen_fd, (struct sockaddr *) &server_addr, sizeof(server_addr)) == -1)
        printf("Error while binding socket!\n");
    else
        printf("Successfully bind socket!\n");

    if (listen(listen_fd, SOMAXCONN) == -1)
        printf("Error while start listening!\n");
    else
        printf("Successfully started listening!\n");

    struct sockaddr_in client_addr{};
    // client_len 必须初始化，否则 accept() 会失败。
    socklen_t client_len = sizeof(client_addr);
    int connection = accept(listen_fd, (struct sockaddr *) &client_addr, &client_len);
    if (connection == -1)
        printf("Error while init accept!\n");
    else {
        printf("Successfully inited accept!\n");
        printf("ip: %s, port: %d\n", inet_ntoa(client_addr.sin_addr), ntohs(client_addr.sin_port));
    }

    char buffer[2048];
    while (true) {
        memset(buffer, 0, sizeof(buffer));
        read(connection, buffer, sizeof(buffer));
        fputs(buffer, stdout);
        write(connection, buffer, strlen(buffer));
    }

    close(connection);
    close(listen_fd);

    return 0;
}
