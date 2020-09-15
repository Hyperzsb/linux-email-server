#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#include<unistd.h>
#include<sys/types.h>
#include<sys/socket.h>
#include<netinet/in.h>
#include<arpa/inet.h>
#include<cjson/cJSON.h>
#include "leadao.h"
#include "type.h"

#define ip_zsb "192.168.43.207"
char ip_s[50];
int listen_fd;
int connection;

void quyinhao(char *str) {
    int n = strlen(str);
    if (str[0] != '\"') {
        printf("yinhao error\n");
    }
    for (int i = 1; i < n; i++) {
        if (str[i] == '\"') {
            printf("yinhao success!\n");
            str[i - 1] = '\0';
            return;
        }
        str[i - 1] = str[i];
    }
}

void send_mail(cJSON *content) {
    auto *mysql_dao = new MySQL_DAO(ip_zsb, 3306, "email_admin", "email_admin_passwd", "email_system");
    mysql_dao->Connect();
    char *sender = cJSON_Print(cJSON_GetObjectItem(content, "sender"));
    printf("sender:%s\n", sender);
    char *recipient = cJSON_Print(cJSON_GetObjectItem(content, "recipient"));
    printf("recipient:%s\n", recipient);
    char *title = cJSON_Print(cJSON_GetObjectItem(content, "title"));
    printf("title:%s\n", title);
    char *body = cJSON_Print(cJSON_GetObjectItem(content, "body"));
    printf("body:%s\n", body);
    quyinhao(sender);
    quyinhao(recipient);
    quyinhao(title);
    quyinhao(body);
    Email email{};
    email.sender = new char[30];
    email.recipient = new char[30];
    email.title = new char[100];
    email.body = new char[10000];
    sprintf(email.sender, sender);
    sprintf(email.recipient, recipient);
    sprintf(email.title, title);
    sprintf(email.body, body);
    Status back;
    back = mysql_dao->SendEmail(ip_s, "null", sender, &email);
    char buf[30];
    if (back == EXPECTED_SUCCESS) {
        strcpy(buf, "EXPECTED_SUCCESS");
        printf("EXPECTED_SUCCESS");
    } else if (back = UNEXPECTED_SUCCESS) {
        strcpy(buf, "UNEXPECTED_SUCCESS");
        printf("UNEXPECTED_SUCCESS");
    } else if (back = EXPECTED_ERROR) {
        strcpy(buf, "EXPECTED_ERROR");
        printf("EXPECTED_ERROR");
    } else {
        strcpy(buf, "UNEXPECTED_ERROR");
        printf("UNEXPECTED_ERROR");
    }
    write(connection, buf, strlen(buf));
    delete mysql_dao;
    free(sender);
    free(recipient);
    free(title);
    free(body);
}

void fetch_mail(cJSON *content) {
    printf("fetch!\n");
    auto *mysql_dao = new MySQL_DAO(ip_zsb, 3306, "email_admin", "email_admin_passwd", "email_system");
    mysql_dao->Connect();
    char *username = cJSON_Print(cJSON_GetObjectItem(content, "username"));
    printf("username:%s\n", username);
    char *type = cJSON_Print(cJSON_GetObjectItem(content, "type"));
    printf("type:%s\n", type);
    quyinhao(username);
    quyinhao(type);
    EmailType tp;
    if (strcmp(type, "\"inbox\"") == 0) {
        tp = IN;
    } else if (strcmp(type, "\"outbox\"") == 0) {
        tp = OUT;
    }
    EmailFeedback *email_feedback = mysql_dao->FetchEmail(ip_s, "null", username, tp);
    printf("[console] email_status:%d, email_num:%d\n", email_feedback->status, email_feedback->email_num);

    cJSON *root = cJSON_CreateObject();
    cJSON *title = cJSON_CreateObject();
    cJSON_AddItemToObject(root, "number", cJSON_CreateNumber(email_feedback->email_num));
    cJSON_AddItemToObject(root, "content", title);
    if (email_feedback->status == EXPECTED_SUCCESS)
        for (int i = 0; i < email_feedback->email_num; i++) {
            char ss[50] = "title_";
            ss[6] = i + 1 + '0';
            ss[7] = '\0';
            cJSON_AddItemToObject(title, ss, cJSON_CreateString(email_feedback->email[i]->title));
            cJSON_AddItemToObject(title, email_feedback->email[i]->title,
                                  cJSON_CreateString(email_feedback->email[i]->body));
            if (tp == IN)
                cJSON_AddItemToObject(title, email_feedback->email[i]->title,
                                      cJSON_CreateString(email_feedback->email[i]->sender));
            else if (tp == OUT)
                cJSON_AddItemToObject(title, email_feedback->email[i]->title,
                                      cJSON_CreateString(email_feedback->email[i]->recipient));
            printf("[console] Email No.%d: sender: %s, recipient: %s, time: %s, title: %s, body: %s\n", i,
                   email_feedback->email[i]->sender, email_feedback->email[i]->recipient,
                   email_feedback->email[i]->time, email_feedback->email[i]->title, email_feedback->email[i]->body);

        }

    // Close MySQL connection
    char *str = cJSON_PrintUnformatted(root);
    write(connection, str, strlen(str));

    delete mysql_dao;
    free(username);
    free(type);
}

void registerr(cJSON *content) {
    // Init MySQL connection
    auto *mySqlDao = new MySQL_DAO(ip_zsb, 3306, "email_admin", "email_admin_passwd", "email_system");
    // Connect to MySQL server
    mySqlDao->Connect();
    // Sign in function
    char *host = cJSON_Print(cJSON_GetObjectItem(content, "host"));
    printf("host:%s\n", host);
    char *domin = cJSON_Print(cJSON_GetObjectItem(content, "domin"));
    printf("domin:%s\n", domin);
    char *password = cJSON_Print(cJSON_GetObjectItem(content, "password"));
    printf("password:%s\n", password);
    char *nickname = cJSON_Print(cJSON_GetObjectItem(content, "nickname"));
    printf("nickname:%s\n", nickname);
    char *description = cJSON_Print(cJSON_GetObjectItem(content, "description"));
    printf("description:%s\n", description);
    char *recovery_question = cJSON_Print(cJSON_GetObjectItem(content, "recovery_question"));
    printf("recovery_question:%s\n", recovery_question);
    char *recovery_answer = cJSON_Print(cJSON_GetObjectItem(content, "recovery_answer"));
    printf("recovery_answer:%s\n", recovery_answer);
    quyinhao(host);
    quyinhao(domin);
    quyinhao(password);
    quyinhao(nickname);
    quyinhao(description);
    quyinhao(recovery_question);
    quyinhao(recovery_answer);
    SignUpStatus signup_status;
    signup_status = mySqlDao->SignUp(ip_s, host, domin, password, nickname, description, recovery_question,
                                     recovery_answer);
    char buf[50];
    if (signup_status == SignUpStatus::SIGN_UP_SUCCESS) {
        strcpy(buf, "SIGN_UP_SUCCESS");
        printf("[console] Account %s@%s sign up successfully\n", host, domin);
    } else if (signup_status == SignUpStatus::SIGN_UP_ACCOUNT_CONFLICT) {
        strcpy(buf, "SIGN_UP_ACCOUNT_CONFLICT");
        printf("[console] Account %s@%s sign up unsuccessfully, already exists\n", host, domin);
    } else {
        strcpy(buf, "UNEXPECTED_ERROR");
        printf("[console] Account %s@%s sign up unsuccessfully, unexpected error\n", host, domin);
    }
    write(connection, buf, strlen(buf));
    delete (mySqlDao);
    free(host);
    free(domin);
    free(password);
    free(nickname);
    free(description);
    free(recovery_question);
    free(recovery_answer);
    printf("1ok\n");
}

void login(cJSON *content) {
    auto *mysql_dao = new MySQL_DAO(ip_zsb, 3306, "email_admin", "email_admin_passwd", "email_system");
    mysql_dao->Connect();
    char *username = cJSON_Print(cJSON_GetObjectItem(content, "username"));
    printf("username:%s\n", username);
    char *password = cJSON_Print(cJSON_GetObjectItem(content, "password"));
    printf("password:%s\n", password);
    quyinhao(username);
    quyinhao(password);
    SignInFeedback *feedback;
    feedback = mysql_dao->SignIn(ip_s, username, password);
    char buf[50];
    if (feedback->status == SignInStatus::SIGN_IN_SUCCESS) {
        strcpy(buf, "SIGN_IN_SUCCESS");
        printf("[console] Sign in successfully: token: %s, nickname: %s, description: %s\n", feedback->token,
               feedback->nickname, feedback->description);
    } else if (feedback->status == SignInStatus::SIGN_IN_ERROR) {
        strcpy(buf, "SIGN_IN_ERROR");
        printf("[console] Sign in error");
    } else if (feedback->status == SignInStatus::SIGN_IN_INVALID_ACCOUNT) {
        strcpy(buf, "SIGN_IN_INVALID_ACCOUNT");
        printf("[console] Account invalid");
    } else {
        strcpy(buf, "SIGN_IN_INVALID_PASSWD");
        printf("[console] Password invalid");
    }
    write(connection, buf, strlen(buf));
    delete (mysql_dao);
    free(username);
    free(password);
}

void get_recovery_question(cJSON *content) {
    auto *mysql_dao = new MySQL_DAO(ip_zsb, 3306, "email_admin", "email_admin_passwd", "email_system");
    mysql_dao->Connect();
    char *username = cJSON_Print(cJSON_GetObjectItem(content, "username"));
    printf("username:%s\n", username);
    quyinhao(username);
    RecoverFeedback *recover_feedback = mysql_dao->GetRecoverQuestion(ip_s, username);
    char buf[500];
    if (recover_feedback->status == Status::EXPECTED_SUCCESS) {
        strcpy(buf, recover_feedback->question);
        printf("[console] Recovery question: %s\n", recover_feedback->question);
    } else {
        strcpy(buf, "ERROR!");
        printf("[console] Get recovery question unsuccessfully\n");
    }
    write(connection, buf, strlen(buf));
    delete recover_feedback;
    free(username);
}

void recovery(cJSON *content) {
    auto *mysql_dao = new MySQL_DAO(ip_zsb, 3306, "email_admin", "email_admin_passwd", "email_system");
    mysql_dao->Connect();
    char *username = cJSON_Print(cJSON_GetObjectItem(content, "username"));
    printf("username:%s\n", username);
    char *recovery_answer = cJSON_Print(cJSON_GetObjectItem(content, "recovery_answer"));
    printf("recovery_answer:%s\n", recovery_answer);
    char *new_password = cJSON_Print(cJSON_GetObjectItem(content, "new_password"));
    printf("new_password:%s\n", new_password);
    quyinhao(username);
    quyinhao(recovery_answer);
    quyinhao(new_password);
    char buf[50];
    RecoverStatus recover_status = mysql_dao->Recover(ip_s, "null", username, recovery_answer, new_password);
    if (recover_status == RECOVER_SUCCESS) {
        strcpy(buf, "RECOVER_SUCCESS");
        printf("[console] Account %s recovers successfully\n", username);
    } else if (recover_status == RECOVER_INVALID_ANSWER) {
        strcpy(buf, "RECOVER_INVALID_ANSWER");
        printf("[console] Account %s recovers unsuccessfully: invalid answer\n", username);
    } else {
        strcpy(buf, "ERROR!");
        printf("[console] Account %s recovers unsuccessfully\n", username);
    }
    delete mysql_dao;
    free(username);
    free(recovery_answer);
    free(new_password);
}

void deletee(cJSON *content) {
    auto *mysql_dao = new MySQL_DAO(ip_zsb, 3306, "email_admin", "email_admin_passwd", "email_system");
    mysql_dao->Connect();
    char *username = cJSON_Print(cJSON_GetObjectItem(content, "username"));
    printf("username:%s\n", username);
    quyinhao(username);
    Status delete_status = mysql_dao->Delete(ip_s, "null", username);
    char buf[50];
    if (delete_status == EXPECTED_SUCCESS) {
        strcpy(buf, "EXPECTED_SUCCESS");
        printf("[console] Account %s deletes itself successfully\n", username);
    } else {
        strcpy(buf, "ERROR!");
        printf("[console] Account %s deletes itself unsuccessfully\n", username);
    }\
    write(connection, buf, strlen(buf));
    delete mysql_dao;
    free(username);
}

void save_draft(cJSON *content) {
    auto *mysql_dao = new MySQL_DAO(ip_zsb, 3306, "email_admin", "email_admin_passwd", "email_system");
    mysql_dao->Connect();
    char *sender = cJSON_Print(cJSON_GetObjectItem(content, "sender"));
    printf("sender:%s\n", sender);
    char *title = cJSON_Print(cJSON_GetObjectItem(content, "title"));
    printf("title:%s\n", title);
    char *body = cJSON_Print(cJSON_GetObjectItem(content, "body"));
    printf("body:%s\n", body);
    char *recipient = cJSON_Print(cJSON_GetObjectItem(content, "recipient"));
    printf("recipient:%s\n", recipient);
    quyinhao(sender);
    quyinhao(title);
    quyinhao(body);
    quyinhao(recipient);
    Email draft{};
    draft.sender = new char[30];
    draft.recipient = new char[30];
    draft.title = new char[100];
    draft.body = new char[10000];
    sprintf(draft.sender, sender);
    sprintf(draft.recipient, recipient);
    sprintf(draft.title, title);
    sprintf(draft.body, body);
    Status draft_status = mysql_dao->SaveDraft(ip_s, "null", sender, &draft);
    char buf[50];
    if (draft_status == EXPECTED_SUCCESS) {
        strcpy(buf, "EXPECTED_SUCCESS");
        printf("[console] Account %s save draft successfully\n");
    } else {
        strcpy(buf, "ERROR!");
        printf("[console] Account %s save draft unsuccessfully\n");
    }
    write(connection, buf, strlen(buf));
    delete mysql_dao;
    free(sender);
    free(recipient);
    free(title);
    free(body);
}

void fetch_draft(cJSON *content) {
    auto *mysql_dao = new MySQL_DAO(ip_zsb, 3306, "email_admin", "email_admin_passwd", "email_system");
    mysql_dao->Connect();
    char *username = cJSON_Print(cJSON_GetObjectItem(content, "username"));
    printf("username:%s\n", username);
    EmailFeedback *draft_feedback = mysql_dao->FetchDraft(ip_s, "null", username);
    printf("[console] email_status:%d, email_num:%d\n", draft_feedback->status, draft_feedback->email_num);

    cJSON *root = cJSON_CreateObject();
    cJSON *title = cJSON_CreateObject();
    cJSON_AddItemToObject(root, "number", cJSON_CreateNumber(draft_feedback->email_num));
    cJSON_AddItemToObject(root, "content", title);
    if (draft_feedback->status == EXPECTED_SUCCESS)
        for (int i = 0; i < draft_feedback->email_num; i++) {
            char ss[50] = "title_";
            ss[6] = i + 1 + '0';
            ss[7] = '\0';
            cJSON_AddItemToObject(title, ss, cJSON_CreateString(draft_feedback->email[i]->title));
            cJSON_AddItemToObject(title, draft_feedback->email[i]->title,
                                  cJSON_CreateString(draft_feedback->email[i]->body));
            cJSON_AddItemToObject(title, draft_feedback->email[i]->title,
                                  cJSON_CreateString(draft_feedback->email[i]->sender));
            printf("[console] Email No.%d: sender: %s, recipient: %s, time: %s, title: %s, body: %s\n", i,
                   draft_feedback->email[i]->sender, draft_feedback->email[i]->recipient,
                   draft_feedback->email[i]->time, draft_feedback->email[i]->title, draft_feedback->email[i]->body);

        }
    // Close MySQL connection
    char *str = cJSON_PrintUnformatted(root);
    write(connection, str, strlen(str));
    delete draft_feedback;
    free(username);
}

void set_contact(cJSON *content) {
    auto *mysql_dao = new MySQL_DAO(ip_zsb, 3306, "email_admin", "email_admin_passwd", "email_system");
    mysql_dao->Connect();
    char *username = cJSON_Print(cJSON_GetObjectItem(content, "username"));
    printf("username:%s\n", username);
    char *fri = cJSON_Print(cJSON_GetObjectItem(content, "friend"));
    printf("friend:%s\n", fri);
    char *remark = cJSON_Print(cJSON_GetObjectItem(content, "remark"));
    printf("remark:%s\n", remark);
    quyinhao(username);
    quyinhao(fri);
    quyinhao(remark);
    Contact *contact;
    contact->contact_name = new char[50];
    contact->alias = new char[50];
    sprintf(contact->contact_name, fri);
    sprintf(contact->alias, remark);
    Status contact_status = mysql_dao->SetContact(ip_s, "null", username, contact);
    char buf[50];
    if (contact_status == EXPECTED_SUCCESS) {
        strcpy(buf, "EXPECTED_SUCCESS");
        printf("[console] Account %s fetch draft successfully\n");
    } else {
        strcpy(buf, "ERROR!");
        printf("[console] Account %s fetch draft unsuccessfully\n");
    }
    write(connection, buf, strlen(buf));
    delete mysql_dao;
    free(username);
    free(fri);
    free(remark);
}

void get_contact(cJSON *content) {
    auto *mysql_dao = new MySQL_DAO(ip_zsb, 3306, "email_admin", "email_admin_passwd", "email_system");
    mysql_dao->Connect();
    char *username = cJSON_Print(cJSON_GetObjectItem(content, "username"));
    printf("username:%s\n", username);
    quyinhao(username);
    ContactFeedback *contact_feedback = mysql_dao->GetContact(ip_s, "null", username);
    cJSON *root = cJSON_CreateObject();
    cJSON *title = cJSON_CreateObject();
    cJSON_AddItemToObject(root, "number", cJSON_CreateNumber(contact_feedback->contact_num));
    cJSON_AddItemToObject(root, "content", title);
    char str[10000];
    if (contact_feedback->status == EXPECTED_SUCCESS) {
        printf("%d\n", contact_feedback->contact_num);
        for (int i = 0; i < contact_feedback->contact_num; i++) {
            char ss[50] = "friend_";
            ss[6] = i + 1 + '0';
            ss[7] = '\0';
            cJSON_AddItemToObject(title, ss, cJSON_CreateString(contact_feedback->contact[i]->contact_name));
            cJSON_AddItemToObject(title, ss, cJSON_CreateString(contact_feedback->contact[i]->alias);
            strcpy(str, cJSON_PrintUnformatted(root));
            printf("%s %s\n", contact_feedback->contact[i]->contact_name, contact_feedback->contact[i]->alias);
        }
    } else {
        strcpy(str, "ERROR!");
    }
    write(connection, str, strlen(str));
    delete mysql_dao;
    free(username);
}

void solve(cJSON *data) {
    if (data == NULL) {
        printf("error1:%s\n", cJSON_GetErrorPtr());
        cJSON_Delete(data);
        return;
    }
    cJSON *Command = cJSON_GetObjectItem(data, "command");
    if (Command == NULL) {
        printf("error:Command\n");
        cJSON_Delete(Command);
        return;
    }
    char *cmd = cJSON_Print(Command);    //��JSON�ṹ���ӡ���ַ����� ��Ҫ�Լ��ͷ�
    printf("cmd:%s\n", cmd);
    cJSON *contentt = cJSON_GetObjectItem(data, "content");
    if (contentt == NULL) {
        printf("error:content\n");
        cJSON_Delete(contentt);
        return;
    }
    if (strcmp(cmd, "send_mail") == 0) {
        //send_mail( contentt );
    }
    if (strcmp(cmd, "\"fetch_mail\"") == 0) {
        fetch_mail(contentt);
    }
    if (strcmp(cmd, "register") == 0) {
        registerr(contentt);
    }
    if (strcmp(cmd, "\"login\"") == 0) {
        printf("lll\n");
        login(contentt);
    }
    if (strcmp(cmd, "recovery") == 0) {
        //recovery( contentt );
    }
    if (strcmp(cmd, "delete") == 0) {
        //deletee( contentt );
    }
    if (strcmp(cmd, "draft box") == 0) {
        //draft( contentt );
    }
    if (strcmp(cmd, "friend") == 0) {
        //friendd( contentt );
    }
    free(cmd);
}

void print_error(const char *message) {
    perror(message);
    exit(EXIT_FAILURE);
}


int main() {
    listen_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (listen_fd == -1)
        print_error("Error while creating socket");
    else
        printf("Successfully created socket!\n");
    struct sockaddr_in server_addr{};
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = inet_addr("192.168.43.185");
    server_addr.sin_port = htons(6000);

    int on = 1;
    if (setsockopt(listen_fd, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on)) == -1)
        print_error("Error while setting socket option");
    else
        printf("Successfully set socket option!\n");
    if (bind(listen_fd, (struct sockaddr *) &server_addr, sizeof(server_addr)) == -1)
        print_error("Error while binding socket");
    else
        printf("Successfully bind socket!\n");

    if (listen(listen_fd, SOMAXCONN) == -1)
        print_error("Error while start listening!\n");
    else
        printf("Successfully started listening!\n");

    while (true) {
        struct sockaddr_in client_addr{};
        socklen_t client_len = sizeof(client_addr);
        connection = accept(listen_fd, (struct sockaddr *) &client_addr, &client_len);
        if (connection == -1)
            print_error("Error while init accept");
        else {
            printf("Successfully inited accept!\n");
            printf("Connection established: ip-%s, port-%d\n", inet_ntoa(client_addr.sin_addr),
                   ntohs(client_addr.sin_port));
        }
        pid_t pid;
        pid = fork();
        if (pid == -1) {
            print_error("Error while establishing child process");
        } else if (pid == 0) {
            close(listen_fd);
            char buffer[2048];
            while (true) {
                memset(buffer, 0, sizeof(buffer));
                int read_status = read(connection, buffer, sizeof(buffer));
                fputs(buffer, stdout);
                printf("success!\n");
                write(connection, buffer, strlen(buffer));
                cJSON *root;
                root = cJSON_Parse(buffer);
                printf("%s\n", cJSON_Print(root));
                printf("s1\n");
                solve(root);
                printf("s2\n");
                strcpy(ip_s, inet_ntoa(client_addr.sin_addr));
                if (read_status == 0) {
                    printf("Connection closed successfully: ip-%s, port-%d\n", inet_ntoa(client_addr.sin_addr),
                           ntohs(client_addr.sin_port));
                    exit(EXIT_SUCCESS);
                } else if (read_status == -1) {
                    printf("Connection closed failed: ip-%s, port-%d\n", inet_ntoa(client_addr.sin_addr),
                           ntohs(client_addr.sin_port));
                    exit(EXIT_FAILURE);
                }
            }
        } else {
            close(connection);
        }
    }
}
