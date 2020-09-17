#include <cstdio>
#include <cstring>
#include <cstdlib>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <cJSON.h>
#include "leadao.h"
#include "type.h"

#define BUFFER_SIZE 1024
#define ip_zsb "192.168.43.207"
char ip_s[50];
int listen_fd;
int connection;

int file_size(char *filename) {
    FILE *fp = fopen(filename, "r");
    if (!fp) return -1;
    fseek(fp, 0L, SEEK_END);
    int size = ftell(fp);
    fclose(fp);

    return size;
}


void StdLog(LogLevel level, const char *msg) {
    time_t time_type;
    time(&time_type);
    char time_str[64] = {0};
    switch (level) {
        case INFO: {
            strftime(time_str, sizeof(time_str), "[Server][INFO    ~ %Y-%m-%d %H:%M:%S] ", localtime(&time_type));
            fputs(time_str, stdout);
            fputs(msg, stdout);
            fputs("\n", stdout);
            break;
        }
        case WARNING: {
            strftime(time_str, sizeof(time_str), "[Server][WARNING ~ %Y-%m-%d %H:%M:%S] ", localtime(&time_type));
            fputs(time_str, stdout);
            fputs(msg, stdout);
            fputs("\n", stdout);
            break;
        }
        case ERROR: {
            strftime(time_str, sizeof(time_str), "[Server][ERROR   ~ %Y-%m-%d %H:%M:%S] ", localtime(&time_type));
            fputs(time_str, stderr);
            fputs(msg, stderr);
            fputs("\n", stderr);
            break;
        }
    }
}


void normalize_str(char *str) {
    int n = strlen(str);
    for (int i = 1; i < n; i++) {
        if (str[i] == '\"') {
            str[i - 1] = '\0';
            return;
        }
        str[i - 1] = str[i];
    }
}

void p_e_s(char *username, char *act) {
    char log_str[300] = {0};
    sprintf(log_str, "%s %s EXPECTED_SUCCESS", username, act);
    StdLog(INFO, log_str);
}

void p_u_s(char *username, char *act) {
    char log_str[300] = {0};
    sprintf(log_str, "%s %s UNEXPECTED_SUCCESS", username, act);
    StdLog(ERROR, log_str);
}

void p_e_e(char *username, char *act) {
    char log_str[300] = {0};
    sprintf(log_str, "%s %s EXPECTED_ERROR", username, act);
    StdLog(ERROR, log_str);
}

void p_u_e(char *username, char *act) {
    char log_str[300] = {0};
    sprintf(log_str, "%s %s UNEXPECTED_ERROR", username, act);
    StdLog(ERROR, log_str);
}

void send_attachment(char *f_title, int byte) {
    char buffer[BUFFER_SIZE];
    strcpy(buffer, "send_me_file");
    write(connection, buffer, strlen(buffer));
    memset(buffer, 0, sizeof(buffer));
    char file_name[100] = "/home/zpy/CLionProjects/leaserver/";
    strcat(file_name, f_title);
    FILE *fp = fopen(file_name, "w");
    if (fp == NULL) {
        printf("File:\t%s ERROR!\n", buffer);
        char ss[50];
        sprintf(ss, "File:\t%s ERROR!\n", buffer);
        write(connection, ss, strlen(ss));
    } else {
        bzero(buffer, BUFFER_SIZE);
        int file_block_length = 0;
        while (1) {
            int read_status = read(connection, buffer, sizeof(buffer));
            fprintf(fp, buffer);
            if (read_status == -1) {
                char ss[50] = "read_error!";
                printf("read_error!");
                write(connection, ss, strlen(ss));
            }
            if (read_status == 0) {
                char ss[50] = "read_success!";
                printf("read_success!");
                write(connection, ss, strlen(ss));
                break;
            }
            file_block_length += read_status;
            if (read_status >= byte) {
                break;
            }
            bzero(buffer, BUFFER_SIZE);
        }
    }
}

void fetch_attachment(char *file_name, int byte) {
    char buffer[BUFFER_SIZE];
    FILE *fp = fopen(file_name, "r");
    if (fp == NULL) {
        printf("File:\t%s ERROR!\n", buffer);
        sprintf(buffer, "File:\t%s ERROR!\n", buffer);
        write(connection, buffer, strlen(buffer));
        memset(buffer, 0, sizeof(buffer));
    } else {
        bzero(buffer, BUFFER_SIZE);
        int file_block_length = 0;
        while (1) {
            fscanf(fp, buffer);
            write(connection, buffer, strlen(buffer));
            file_block_length += BUFFER_SIZE;
            if (file_block_length >= byte) {
                return;
            }
            bzero(buffer, BUFFER_SIZE);
        }
    }
};


void send_mail(cJSON *content) {
    printf("111\n");
    auto *mysql_dao = new MySQL_DAO(ip_zsb, 3306, "email_admin", "email_admin_passwd", "email_system");
    mysql_dao->Connect();
    char *sender = cJSON_Print(cJSON_GetObjectItem(content, "sender"));
    char *recipient = cJSON_Print(cJSON_GetObjectItem(content, "recipient"));
    char *title = cJSON_Print(cJSON_GetObjectItem(content, "title"));
    char *body = cJSON_Print(cJSON_GetObjectItem(content, "body"));
    char *f_title = cJSON_Print(cJSON_GetObjectItem(content, "f_title"));
    char *fl = cJSON_Print(cJSON_GetObjectItem(content, "file"));
    printf("222\n");
    char log_str[300] = {0};
    sprintf(log_str, "Get JSON: sender: %s, recipient: %s, title: %s, body: %s", sender, recipient, title, body);
    StdLog(INFO, log_str);
    normalize_str(sender);
    normalize_str(recipient);
    normalize_str(title);
    normalize_str(body);
    normalize_str(f_title);
    Email email{};
    email.sender = new char[30];
    email.recipient = new char[30];
    email.title = new char[100];
    email.body = new char[10000];
    int f = 0;
    if (strcmp(f_title, "null") == 0) {
        email.accessory_route = nullptr;
        f = 1;
    } else {
        email.accessory_route = new char[100];
        sprintf(email.accessory_route, "/home/zpy/CLionProjects/leaserver/%s", f_title);
    }
    sprintf(email.sender, sender);
    sprintf(email.recipient, recipient);
    sprintf(email.title, title);
    sprintf(email.body, body);
    Status back;
    back = mysql_dao->SendEmail(ip_s, "null", sender, &email);
    char buf[30];
    if (back == EXPECTED_SUCCESS) {
        strcpy(buf, "EXPECTED_SUCCESS");
        p_e_s(sender, "send_mail");
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
    MySQL_DAO mysql_dao = MySQL_DAO(ip_zsb, 3306, "email_admin", "email_admin_passwd", "email_system");
    mysql_dao.Connect();
    char *username = cJSON_Print(cJSON_GetObjectItem(content, "username"));
    printf("username:%s\n", username);
    char *type = cJSON_Print(cJSON_GetObjectItem(content, "type"));
    printf("type:%s\n", type);
    normalize_str(username);
    normalize_str(type);
    EmailType tp = OUT;
    if (strcmp(type, "inbox") == 0) {
        tp = IN;
    } else if (strcmp(type, "outbox") == 0) {
        tp = OUT;
    }
    EmailFeedback *email_feedback = mysql_dao.FetchEmail(ip_s, "null", username, tp);
    printf("[console] email_status:%d, email_num:%d\n", email_feedback->status, email_feedback->email_num);
    char str[1000];
    memset(str, 0, 1000);
    cJSON *root = cJSON_CreateObject();
    cJSON *title = cJSON_CreateObject();
    char s[50];
    sprintf(s, "%d", email_feedback->email_num);
    cJSON_AddItemToObject(root, "number", cJSON_CreateString(s));
    cJSON_AddItemToObject(root, "content", title);

    if (email_feedback->status == EXPECTED_SUCCESS) {
        strcpy(str, "EXPECTED_SUCCESS");
        write(connection, str, strlen(str));
        memset(str, 0, sizeof(str));
        int read_status = read(connection, str, sizeof(str));
        if (read_status == -1) {
            strcpy(str, "error1!");
            printf("%s\n", str);
            write(connection, str, strlen(str));
            memset(str, 0, sizeof(str));
            return;
        }
        printf("str:%s\n", str);
        if (strcmp(str, "REQUEST_FOR_SENDING_PACKAGE") != 0) {
            strcpy(str, "error2!");
            printf("%s\n", str);
            write(connection, str, strlen(str));
            memset(str, 0, sizeof(str));
            return;
        }
        for (int i = 0; i < email_feedback->email_num; i++) {
            char ss[50] = {0};
            sprintf(ss, "%d", i + 1);
            char a[50] = {0};
            sprintf(a, "cont%d", i + 1);
            char b[50] = {0};
            sprintf(b, "time%d", i + 1);
            char c[50] = {0};
            sprintf(c, "f_title%d", i + 1);
            char d[50] = {0};
            sprintf(c, "file%d", i + 1);
            cJSON_AddItemToObject(title, ss, cJSON_CreateString(email_feedback->email[i]->title));
            cJSON_AddItemToObject(title, email_feedback->email[i]->title,
                                  cJSON_CreateString(email_feedback->email[i]->body));
            if (tp == IN)
                cJSON_AddItemToObject(title, a, cJSON_CreateString(email_feedback->email[i]->sender));
            else if (tp == OUT)
                cJSON_AddItemToObject(title, a, cJSON_CreateString(email_feedback->email[i]->recipient));
            cJSON_AddItemToObject(title, b, cJSON_CreateString(email_feedback->email[i]->time));
            if (email_feedback->email[i]->accessory_route == nullptr) {
                cJSON_AddItemToObject(title, c, cJSON_CreateString("null"));
                cJSON_AddItemToObject(title, d, cJSON_CreateString("null"));
            } else {
                char fl[20000];
                char file_name[100];
                strcpy(file_name, email_feedback->email[i]->accessory_route);
                FILE *fp = fopen(file_name, "r");
                if (fp == NULL) {
                    printf("File:\t%s ERROR!\n", file_name);
                    return;
                } else {
                    fscanf(fp, fl);
                }
                cJSON_AddItemToObject(title, c, cJSON_CreateString(email_feedback->email[i]->accessory_route));
                cJSON_AddItemToObject(title, d, cJSON_CreateString(fl));
            }
            printf("[console] Email No.%d: sender: %s, recipient: %s, time: %s, title: %s, body: %s\n", i,
                   email_feedback->email[i]->sender, email_feedback->email[i]->recipient,
                   email_feedback->email[i]->time, email_feedback->email[i]->title, email_feedback->email[i]->body);
        }
        strcpy(str, cJSON_PrintUnformatted(root));
        write(connection, str, strlen(str));
        memset(str, 0, sizeof(str));
    } else {
        strcpy(str, "ERROR!");
        printf("%s\n", str);
        write(connection, str, strlen(str));
        memset(str, 0, sizeof(str));
    }
}

void registerr(cJSON *content) {
    // Init MySQL connection
    auto *mySqlDao = new MySQL_DAO(ip_zsb, 3306, "email_admin", "email_admin_passwd", "email_system");
    // Connect to MySQL server
    mySqlDao->Connect();
    // Sign in function
    char *host = cJSON_Print(cJSON_GetObjectItem(content, "host"));
    printf("host:%s\n", host);
    char *domain = cJSON_Print(cJSON_GetObjectItem(content, "domain"));
    printf("domain:%s\n", domain);
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
    normalize_str(host);
    normalize_str(domain);
    normalize_str(password);
    normalize_str(nickname);
    normalize_str(description);
    normalize_str(recovery_question);
    normalize_str(recovery_answer);
    SignUpStatus signup_status;
    printf("%s\n", ip_s);
    signup_status = mySqlDao->SignUp(ip_s, host, domain, password, nickname, description, recovery_question,
                                     recovery_answer);
    char buf[50];
    if (signup_status == SignUpStatus::SIGN_UP_SUCCESS) {
        strcpy(buf, "SIGN_UP_SUCCESS");
        printf("[console] Account %s@%s sign up successfully\n", host, domain);
    } else if (signup_status == SignUpStatus::SIGN_UP_ACCOUNT_CONFLICT) {
        strcpy(buf, "SIGN_UP_ACCOUNT_CONFLICT");
        printf("[console] Account %s@%s sign up unsuccessfully, already exists\n", host, domain);
    } else {
        strcpy(buf, "UNEXPECTED_ERROR");
        printf("[console] Account %s@%s sign up unsuccessfully, unexpected error\n", host, domain);
    }
    write(connection, buf, strlen(buf));
    //delete (mySqlDao);
    free(host);
    free(domain);
    free(password);
    free(nickname);
    free(description);
    free(recovery_question);
    free(recovery_answer);
}

void login(cJSON *content) {
    auto *mysql_dao = new MySQL_DAO(ip_zsb, 3306, "email_admin", "email_admin_passwd", "email_system");
    mysql_dao->Connect();
    char *username = cJSON_Print(cJSON_GetObjectItem(content, "username"));
    printf("username:%s\n", username);
    char *password = cJSON_Print(cJSON_GetObjectItem(content, "password"));
    printf("password:%s\n", password);
    normalize_str(username);
    normalize_str(password);
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
    //delete (mysql_dao);
    free(username);
    free(password);

}

void get_recovery_question(cJSON *content) {
    auto *mysql_dao = new MySQL_DAO(ip_zsb, 3306, "email_admin", "email_admin_passwd", "email_system");
    mysql_dao->Connect();
    char *username = cJSON_Print(cJSON_GetObjectItem(content, "username"));
    printf("username:%s\n", username);
    normalize_str(username);
    printf("322\n");
    RecoverFeedback *recover_feedback = mysql_dao->GetRecoverQuestion(ip_s, username);
    printf("333\n");
    char buf[500];
    if (recover_feedback->status == Status::EXPECTED_SUCCESS) {
        printf("111\n");
        strcpy(buf, recover_feedback->question);
        printf("[console] Recovery question: %s\n", recover_feedback->question);
    } else {
        printf("222\n");
        strcpy(buf, "ERROR!");
        printf("[console] Get recovery question unsuccessfully\n");
    }
    write(connection, buf, strlen(buf));
    delete mysql_dao;
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
    normalize_str(username);
    normalize_str(recovery_answer);
    normalize_str(new_password);
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
    write(connection, buf, strlen(buf));
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
    normalize_str(username);
    Status delete_status = mysql_dao->Delete(ip_s, "null", username);
    char buf[50];
    if (delete_status == EXPECTED_SUCCESS) {
        strcpy(buf, "EXPECTED_SUCCESS");
        printf("[console] Account %s deletes itself successfully\n", username);
    } else {
        strcpy(buf, "ERROR!");
        printf("[console] Account %s deletes itself unsuccessfully\n", username);
    }
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
    normalize_str(sender);
    normalize_str(title);
    normalize_str(body);
    normalize_str(recipient);
    Email draft{};
    draft.sender = new char[30];
    draft.recipient = new char[30];
    draft.title = new char[100];
    draft.body = new char[10000];
    draft.accessory_route = nullptr;
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
    normalize_str(username);
    printf("username:%s\n", username);
    EmailFeedback *draft_feedback = mysql_dao->FetchDraft(ip_s, "null", username);
    printf("[console] email_status:%d, email_num:%d\n", draft_feedback->status, draft_feedback->email_num);
    normalize_str(username);
    char str[1000];
    memset(str, 0, sizeof(str));
    cJSON *root = cJSON_CreateObject();
    cJSON *title = cJSON_CreateObject();
    cJSON_AddItemToObject(root, "number", cJSON_CreateNumber(draft_feedback->email_num));
    cJSON_AddItemToObject(root, "content", title);
    if (draft_feedback->status == EXPECTED_SUCCESS) {
        strcpy(str, "EXPECTED_SUCCESS");
        write(connection, str, strlen(str));
        memset(str, 0, sizeof(str));
        int read_status = read(connection, str, sizeof(str));
        if (read_status == -1) {
            char s[50];
            strcpy(s, "error!");
            printf("%s\n", s);
            write(connection, s, strlen(s));
            return;
        }
        if (strcmp(str, "REQUEST_FOR_SENDING_PACKAGE") != 0) {
            char s[50];
            strcpy(s, "error command!");
            printf("%s\n", s);
            write(connection, s, strlen(s));
            return;
        }
        for (int i = 0; i < draft_feedback->email_num; i++) {
            char ss[50] = {0};
            sprintf(ss, "%d", i + 1);
            char a[50] = {0};
            sprintf(a, "cont%d", i + 1);
            char b[50] = {0};
            sprintf(b, "time%d", i + 1);
            cJSON_AddItemToObject(title, ss, cJSON_CreateString(draft_feedback->email[i]->title));
            cJSON_AddItemToObject(title, draft_feedback->email[i]->title,
                                  cJSON_CreateString(draft_feedback->email[i]->body));
            cJSON_AddItemToObject(title, a, cJSON_CreateString(draft_feedback->email[i]->sender));
            cJSON_AddItemToObject(title, b, cJSON_CreateString(draft_feedback->email[i]->time));
            printf("[console] Email No.%d: sender: %s, recipient: %s, time: %s, title: %s, body: %s\n", i,
                   draft_feedback->email[i]->sender, draft_feedback->email[i]->recipient,
                   draft_feedback->email[i]->time, draft_feedback->email[i]->title, draft_feedback->email[i]->body);
        }
        strcpy(str, cJSON_PrintUnformatted(root));
        write(connection, str, strlen(str));
        memset(str, 0, sizeof(str));
    } else {
        strcpy(str, "ERROR!");
        printf("%s\n", str);
        write(connection, str, strlen(str));
        memset(str, 0, sizeof(str));
    }
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
    normalize_str(username);
    normalize_str(fri);
    normalize_str(remark);
    Contact *contact = new Contact;
    contact->contact_name = new char[50];
    contact->alias = new char[50];
    strcpy(contact->contact_name, fri);
    strcpy(contact->alias, remark);
    Status contact_status = mysql_dao->SetContact(ip_s, "null", username, contact);
    char buf[50];
    if (contact_status == EXPECTED_SUCCESS) {
        strcpy(buf, "EXPECTED_SUCCESS");
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
    normalize_str(username);
    ContactFeedback *contact_feedback = mysql_dao->GetContact(ip_s, "null", username);
    cJSON *root = cJSON_CreateObject();
    cJSON *title = cJSON_CreateObject();
    cJSON_AddItemToObject(root, "number", cJSON_CreateNumber(contact_feedback->contact_num));
    cJSON_AddItemToObject(root, "content", title);
    char str[10000];
    memset(str, 0, sizeof(str));
    if (contact_feedback->status == EXPECTED_SUCCESS) {
        printf("contact_num %d\n", contact_feedback->contact_num);\
        int n = contact_feedback->contact_num;
        strcpy(str, "EXPECTED_SUCCESS");
        write(connection, str, strlen(str));
        memset(str, 0, sizeof(str));
        read(connection, str, sizeof(str));
        printf("%s\n", str);
        memset(str, 0, sizeof(str));
        for (int i = 0; i < n; i++) {
            char *ss;
            ss = new char[100];
            sprintf(ss, "%d", i + 1);
            cJSON_AddItemToObject(title, ss, cJSON_CreateString(contact_feedback->contact[i]->contact_name));
            cJSON_AddItemToObject(title, contact_feedback->contact[i]->contact_name,
                                  cJSON_CreateString(contact_feedback->contact[i]->alias));
            printf("%s %s\n", contact_feedback->contact[i]->contact_name, contact_feedback->contact[i]->alias);
            delete ss;
        }
        strcpy(str, cJSON_PrintUnformatted(root));
        printf("%s\n", str);
        write(connection, str, strlen(str));
    } else {
        strcpy(str, "ERROR!");
        write(connection, str, strlen(str));
    }
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
    char *cmd = cJSON_Print(Command);
    printf("cmd:%s\n", cmd);
    cJSON *contentt = cJSON_GetObjectItem(data, "content");
    if (contentt == NULL) {
        printf("error:content\n");
        cJSON_Delete(contentt);
        return;
    }
    if (strcmp(cmd, "\"send_mail\"") == 0) {
        send_mail(contentt);
    }
    if (strcmp(cmd, "\"fetch_mail\"") == 0) {
        fetch_mail(contentt);
    }
    if (strcmp(cmd, "\"register\"") == 0) {
        registerr(contentt);
    }
    if (strcmp(cmd, "\"login\"") == 0) {
        login(contentt);
    }
    if (strcmp(cmd, "\"recovery\"") == 0) {
        recovery(contentt);
    }
    if (strcmp(cmd, "\"get_recovery_question\"") == 0) {
        get_recovery_question(contentt);
    }
    if (strcmp(cmd, "\"delete\"") == 0) {
        deletee(contentt);
    }
    if (strcmp(cmd, "\"save_draft\"") == 0) {
        save_draft(contentt);
    }
    if (strcmp(cmd, "\"fetch_draft\"") == 0) {
        fetch_draft(contentt);
    }
    if (strcmp(cmd, "\"set_contact\"") == 0) {
        set_contact(contentt);
    }
    if (strcmp(cmd, "\"get_contact\"") == 0) {
        get_contact(contentt);
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
                cJSON *root;
                root = cJSON_Parse(buffer);
                printf("%s\n", cJSON_Print(root));
                printf("s1\n");
                memset(ip_s, 0, sizeof(ip_s));
                strcpy(ip_s, inet_ntoa(client_addr.sin_addr));
                solve(root);
                printf("s2\n");
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
