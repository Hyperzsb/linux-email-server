#ifndef DAO_LIB_LESDAO_H
#define DAO_LIB_LESDAO_H

#include <string>
#include <ctime>
#include <mysql/mysql.h>

using namespace std;

class MySQL_DAO {

private:
    string *mysql_host;
    unsigned int mysql_port;
    string *username;
    string *password;
    string *database;
    MYSQL *connection;

    static void ErrorLog(const char *msg);

    static void InfoLog(const char *msg);

public:
    MySQL_DAO();

    MySQL_DAO(const char *host, short port, const char *user, const char *passwd, const char *db);

    ~MySQL_DAO();

    bool connect();
};

#endif //DAO_LIB_LESDAO_H
