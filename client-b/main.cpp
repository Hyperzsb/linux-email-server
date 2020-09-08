#include <system_error>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <csignal>
#include <cstdio>
#include <cstring>

void print_error(const char *message) {
    perror(message);
    exit(EXIT_FAILURE);
}

void signal_handler(int signal) {
    printf("SINGLE:%d\n", signal);
    exit(EXIT_SUCCESS);
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

    // 创建子进程来处理业务内容
    pid_t pid;
    pid = fork();
    if (pid == -1) {
        // 创建子进程失败
        print_error("Error while establishing child process");
    } else if (pid == 0) {
        signal(SIGUSR1, signal_handler);
        // 创建子进程成功，进入子进程
        // 子进程不需要 listen_fd 套接字，将其关闭
        char buffer[2048];
        while (fgets(buffer, sizeof(buffer), stdin) != nullptr) {
            write(client_socket, buffer, strlen(buffer));
            memset(buffer, 0, sizeof(buffer));
        }
        close(client_socket);
        exit(EXIT_SUCCESS);
    } else {
        // 创建子进程成功，进入父进程
        // 父进程不需要 connect 套接字，将其关闭
        char buffer[2048];
        while (true) {
            memset(buffer, 0, sizeof(buffer));
            int read_status = read(client_socket, buffer, sizeof(buffer));
            // 当客户端关闭，read 返回 0
            if (read_status == 0) {
                printf("Connection closed successfully: ip-%s, port-%d\n", inet_ntoa(server_addr.sin_addr),
                       ntohs(server_addr.sin_port));
                // 结束子进程
                close(client_socket);
                kill(pid, SIGUSR1);
                exit(EXIT_SUCCESS);
            } else if (read_status == -1) {
                printf("Connection closed failed: ip-%s, port-%d\n", inet_ntoa(server_addr.sin_addr),
                       ntohs(server_addr.sin_port));
                // 结束子进程
                close(client_socket);
                kill(pid, SIGUSR1);
                exit(EXIT_FAILURE);
            } else
                fputs(buffer, stdout);
        }
    }


    close(client_socket);

    return 0;
}