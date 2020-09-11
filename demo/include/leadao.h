#ifndef LIBLEADAO_LEADAO_H
#define LIBLEADAO_LEADAO_H

#include <mysql/mysql.h>
#include <openssl/sha.h>
#include <string>
#include <ctime>
#include <cstring>

using namespace std;

enum LogLevel {
    INFO = 0, WARNING = 1, ERROR = 2
};

enum SignUpStatus {
    SIGN_UP_SUCCESS = 0,
    SIGN_UP_ERROR = 1,
    SIGN_UP_ACCOUNT_CONFLICT = 2
};

enum SignInStatus {
    SIGN_IN_SUCCESS = 0,
    SIGN_IN_ERROR = 1,
    SIGN_IN_INVALID_ACCOUNT = 2,
    SIGN_IN_INVALID_PASSWD = 3
};

class MySQL_DAO {

private:
    string *mysql_host;
    unsigned int mysql_port;
    string *username;
    string *password;
    string *database;
    MYSQL *connection;
    bool connection_status;

    static void StdLog(LogLevel level, const char *msg);

public:
    MySQL_DAO();

    MySQL_DAO(const char *host, short port, const char *user, const char *passwd, const char *db);

    ~MySQL_DAO();

    bool Connect();

    SignUpStatus SignUp(const char *host, const char *domain, const char *account_passwd,
                        const char *nickname, const char *description,
                        const char *recovery_question, const char *recovery_answer);

    SignInStatus SignIn(const char *account_name, const char *account_passwd);
};

#endif //LIBLEADAO_LEADAO_H
