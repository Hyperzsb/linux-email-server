#include "lesdao.h"

void MySQL_DAO::ErrorLog(const char *msg) {
    time_t time_type;
    time(&time_type);
    char time_str[64];
    strftime(time_str, sizeof(time_str), "[ERROR ~ %Y-%m-%d %H:%M:%S] ", localtime(&time_type));
    fputs(time_str, stderr);
    fputs(msg, stderr);
    fputs("\n", stderr);
}

void MySQL_DAO::InfoLog(const char *msg) {
    time_t time_type;
    time(&time_type);
    char time_str[64];
    strftime(time_str, sizeof(time_str), "[INFO ~ %Y-%m-%d %H:%M:%S] ", localtime(&time_type));
    fputs(time_str, stdout);
    fputs(msg, stdout);
    fputs("\n", stdout);
}

MySQL_DAO::MySQL_DAO() {
    this->mysql_host = nullptr;
    this->mysql_port = 0;
    this->username = nullptr;
    this->password = nullptr;
    this->database = nullptr;
    this->connection = nullptr;
}

MySQL_DAO::MySQL_DAO(const char *host, short port, const char *user, const char *passwd, const char *db) {
    this->mysql_host = new string(host);
    this->mysql_port = port;
    this->username = new string(user);
    this->password = new string(passwd);
    this->database = new string(db);
    // Init connection
    this->connection = mysql_init(nullptr);
    if (connection == nullptr)
        ErrorLog("Init connection failed");
    else
        InfoLog("Init connection succeeded");
}

MySQL_DAO::~MySQL_DAO() {
    if (this->connection != nullptr) {
        mysql_close(connection);
        InfoLog("Closed connection");
    }
    delete this->mysql_host;
    delete this->username;
    delete this->password;
    delete this->database;
}

bool MySQL_DAO::connect() {
    // Try real connection
    connection = mysql_real_connect(connection, this->mysql_host->c_str(), this->username->c_str(),
                                    this->password->c_str(), this->database->c_str(), this->mysql_port,
                                    nullptr, 0);
    if (connection) {
        InfoLog("Real connection establishment succeeded");
        return true;
    } else {
        ErrorLog("Real connection establishment failed");
        return false;
    }
}
