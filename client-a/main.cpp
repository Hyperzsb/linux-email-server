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
    // 创建监听套接字 listen_fd
    int listen_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (listen_fd == -1)
        print_error("Error while creating socket");
    else
        printf("Successfully created socket!\n");

    // 配置服务器地址
    struct sockaddr_in server_addr{};
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = inet_addr("127.0.0.1");
    server_addr.sin_port = htons(6000);

    // 在初始化之前，设置 socket 选项，使得 socket 不必等待地址 TIME_WAIT 状态释放
    // TODO: 目前会产生 CLOSE_WAIT 的状态，TBD
    int on = 1;
    if (setsockopt(listen_fd, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on)) == -1)
        print_error("Error while setting socket option");
    else
        printf("Successfully set socket option!\n");

    // 将服务器地址绑定到监听套接字 listen_fd 上
    if (bind(listen_fd, (struct sockaddr *) &server_addr, sizeof(server_addr)) == -1)
        print_error("Error while binding socket");
    else
        printf("Successfully bind socket!\n");

    if (listen(listen_fd, SOMAXCONN) == -1)
        print_error("Error while start listening!\n");
    else
        printf("Successfully started listening!\n");

    struct sockaddr_in client_addr{};
    // client_len 必须初始化，否则 accept() 会失败
    socklen_t client_len = sizeof(client_addr);
    // 从握手队列中捕捉并创建连接
    int connection = accept(listen_fd, (struct sockaddr *) &client_addr, &client_len);
    if (connection == -1)
        print_error("Error while init accept");
    else {
        printf("Successfully inited accept!\n");
        printf("Connection established: ip-%s, port-%d\n", inet_ntoa(client_addr.sin_addr),
               ntohs(client_addr.sin_port));
    }

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
            write(connection, buffer, strlen(buffer));
            memset(buffer, 0, sizeof(buffer));
        }
        close(connection);
        exit(EXIT_SUCCESS);
    } else {
        // 创建子进程成功，进入父进程
        // 父进程不需要 connect 套接字，将其关闭
        char buffer[2048];
        while (true) {
            memset(buffer, 0, sizeof(buffer));
            int read_status = read(connection, buffer, sizeof(buffer));
            // 当客户端关闭，read 返回 0
            if (read_status == 0) {
                printf("Connection closed successfully: ip-%s, port-%d\n", inet_ntoa(client_addr.sin_addr),
                       ntohs(client_addr.sin_port));
                // 结束子进程
                close(connection);
                kill(pid, SIGUSR1);
                exit(EXIT_SUCCESS);
            } else if (read_status == -1) {
                printf("Connection closed failed: ip-%s, port-%d\n", inet_ntoa(client_addr.sin_addr),
                       ntohs(client_addr.sin_port));
                // 结束子进程
                close(connection);
                kill(pid, SIGUSR1);
                exit(EXIT_FAILURE);
            } else
                fputs(buffer, stdout);
        }
    }

    close(listen_fd);

    return 0;
}
