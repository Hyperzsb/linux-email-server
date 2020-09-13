#ifndef LIBLEADAO_LEADAO_H
#define LIBLEADAO_LEADAO_H

#include <mysql/mysql.h>
#include <openssl/sha.h>
#include <cstdlib>
#include <cstdio>
#include <cstring>
#include <ctime>
#include "type.h"

using namespace std;

class MySQL_DAO {

private:
    char *mysql_host;
    unsigned int mysql_port;
    char *username;
    char *password;
    char *database;
    MYSQL *connection;

    static void StdLog(LogLevel level, const char *msg);

    static char *GetAccountHost(const char *account_name);

    static char *GetAccountDomain(const char *account_name);

    SQLFeedback *GetAccountID(const char *account_name);

    SQLFeedback *GetAccountName(const char *account_id);

public:
    MySQL_DAO();

    MySQL_DAO(const char *host, short port, const char *user, const char *passwd, const char *db);

    ~MySQL_DAO();

    bool Connect();

    SignUpStatus SignUp(const char *ip, const char *host, const char *domain, const char *passwd,
                        const char *nickname, const char *description,
                        const char *recovery_question, const char *recovery_answer);

    SignInFeedback *SignIn(const char *ip, const char *account_name, const char *account_passwd);

    Status SendEmail(const char *ip, const char *token, Email *email);

    EmailFeedBack *FetchEmail(const char *ip, const char *token, const char *account_name, EmailType type);
};

#endif //LIBLEADAO_LEADAO_H
