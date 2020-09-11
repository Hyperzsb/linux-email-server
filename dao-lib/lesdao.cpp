#include "lesdao.h"

void MySQL_DAO::StdLog(LogLevel level, const char *msg) {
    time_t time_type;
    time(&time_type);
    char time_str[64] = {0};
    switch (level) {
        case INFO: {
            strftime(time_str, sizeof(time_str), "[INFO    ~ %Y-%m-%d %H:%M:%S] ", localtime(&time_type));
            fputs(time_str, stdout);
            fputs(msg, stdout);
            fputs("\n", stdout);
            break;
        }
        case WARNING: {
            strftime(time_str, sizeof(time_str), "[WARNING ~ %Y-%m-%d %H:%M:%S] ", localtime(&time_type));
            fputs(time_str, stdout);
            fputs(msg, stdout);
            fputs("\n", stdout);
            break;
        }
        case ERROR: {
            strftime(time_str, sizeof(time_str), "[ERROR   ~ %Y-%m-%d %H:%M:%S] ", localtime(&time_type));
            fputs(time_str, stderr);
            fputs(msg, stderr);
            fputs("\n", stderr);
            break;
        }
    }
}

MySQL_DAO::MySQL_DAO() {
    this->mysql_host = nullptr;
    this->mysql_port = 0;
    this->username = nullptr;
    this->password = nullptr;
    this->database = nullptr;
    this->connection = nullptr;
    this->connection_status = false;
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
        StdLog(ERROR, "Init connection failed");
    else
        StdLog(INFO, "Init connection succeeded");
    this->connection_status = false;
}

MySQL_DAO::~MySQL_DAO() {
    if (this->connection != nullptr) {
        mysql_close(connection);
        StdLog(INFO, "Closed established connection. Process shutting down");
    } else {
        StdLog(WARNING, "No connection has been established. Process shutting down");
    }
    delete this->mysql_host;
    delete this->username;
    delete this->password;
    delete this->database;
}

bool MySQL_DAO::Connect() {
    // Try real connection
    connection = mysql_real_connect(connection, this->mysql_host->c_str(), this->username->c_str(),
                                    this->password->c_str(), this->database->c_str(), this->mysql_port,
                                    nullptr, 0);
    if (connection) {
        StdLog(INFO, "Real connection establishment succeeded");
        // Set charset
        if (!mysql_set_character_set(connection, "utf8")) {
            string log_str = "Successfully set charset: ";
            log_str.append(mysql_character_set_name(connection));
            StdLog(INFO, log_str.c_str());
        }
        this->connection_status = true;
        return true;
    } else {
        StdLog(ERROR, "Real connection establishment failed");
        return false;
    }
}

SignUpStatus MySQL_DAO::SignUp(const char *host, const char *domain, const char *account_passwd,
                               const char *nickname, const char *description,
                               const char *recovery_question, const char *recovery_answer) {
    // Log
    char log_str[200] = {0};
    sprintf(log_str, "Account '%s@%s' is trying to sign up", host, domain);
    StdLog(INFO, log_str);
    // Check host and domain
    char query[200] = {0};
    sprintf(query, "select id from account_info where host = '%s' and domain = '%s';",
            host, domain);
    if (mysql_query(connection, query) == 0) {
        MYSQL_RES *result = mysql_store_result(connection);
        if (result != nullptr) {
            int count = mysql_num_rows(result);
            mysql_free_result(result);
            // Check if the account already exists
            if (count == 0) {
                memset(log_str, 0, 200);
                sprintf(log_str, "Account '%s@%s' is a valid account", host, domain);
                StdLog(INFO, log_str);
                // Generate account Hash ID
                SHA256_CTX sha256_ctx;
                SHA256_Init(&sha256_ctx);
                SHA256_Update(&sha256_ctx, host, strlen(host));
                SHA256_Update(&sha256_ctx, domain, strlen(domain));
                unsigned char raw_hash_id[32] = {0};
                SHA256_Final(raw_hash_id, &sha256_ctx);
                char hash_id[16] = {0};
                for (int i = 0; i < 8; i++)
                    sprintf(hash_id + i * 2, "%02x", raw_hash_id[i]);
                // Add account to MySQL
                char add_account_query[700] = {0};
                sprintf(add_account_query,
                        "insert into account_info "
                        "(id, host, domain, password, recovery_question, recovery_answer, nickname, description, locked) "
                        "value ('%s', '%s', '%s', '%s', '%s', '%s', '%s', '%s', %d);",
                        hash_id, host, domain, account_passwd, recovery_question, recovery_answer,
                        nickname, description, 0);
                if (mysql_query(connection, add_account_query) == 0) {
                    memset(log_str, 0, 200);
                    sprintf(log_str, "Add account '%s@%s' successfully", host, domain);
                    StdLog(INFO, log_str);
                    return SIGN_UP_SUCCESS;
                } else {
                    StdLog(ERROR, mysql_error(connection));
                    return SIGN_UP_ERROR;
                }
            } else {
                memset(log_str, 0, 200);
                sprintf(log_str, "Account '%s@%s' already exists", host, domain);
                StdLog(WARNING, log_str);
                return SIGN_UP_ACCOUNT_CONFLICT;
            }
        } else {
            memset(log_str, 0, 200);
            StdLog(ERROR, mysql_error(connection));
            mysql_free_result(result);
            return SIGN_UP_ERROR;
        }
    } else {
        StdLog(ERROR, mysql_error(connection));
        return SIGN_UP_ERROR;
    }
}

SignInStatus MySQL_DAO::SignIn(const char *account_name, const char *account_passwd) {
    // Log
    char log_str[200] = {0};
    sprintf(log_str, "Account '%s' is trying to login", account_name);
    StdLog(INFO, log_str);
    // Divide host and domain
    char host_name[31] = {0}, domain_name[31] = {0};
    int account_name_len = strlen(account_name), host_name_len = 0, domain_name_len = 0;
    for (int i = 0; i < account_name_len; i++)
        if (account_name[i] != '@') {
            host_name[host_name_len] = account_name[i];
            host_name_len++;
        } else {
            break;
        }
    for (int i = host_name_len + 1; i < account_name_len; i++) {
        domain_name[domain_name_len] = account_name[i];
        domain_name_len++;
    }
    // Query from database
    char query[200] = {0};
    sprintf(query, R"(select password from account_info where host = '%s' and domain = '%s';)",
            host_name, domain_name);
    if (mysql_query(connection, query) == 0) {
        MYSQL_RES *result = mysql_store_result(connection);
        if (result != nullptr) {
            if (mysql_num_rows(result) == 0) {
                memset(log_str, 0, 200);
                sprintf(log_str, "No such account '%s'", account_name);
                StdLog(WARNING, log_str);
                return SIGN_IN_INVALID_ACCOUNT;
            }
            MYSQL_ROW result_row;
            result_row = mysql_fetch_row(result);
            string valid_passwd = result_row[0];
            mysql_free_result(result);
            if (strcmp(account_passwd, valid_passwd.c_str()) == 0) {
                memset(log_str, 0, 200);
                sprintf(log_str, "Account '%s' matches its given password", account_name);
                StdLog(INFO, log_str);
                return SIGN_IN_SUCCESS;
            } else {
                memset(log_str, 0, 200);
                sprintf(log_str, "Account '%s' does not match its password", account_name);
                StdLog(WARNING, log_str);
                return SIGN_IN_INVALID_PASSWD;
            }
        } else {
            StdLog(ERROR, mysql_error(connection));
            mysql_free_result(result);
            return SIGN_IN_ERROR;
        }
    } else {
        StdLog(ERROR, mysql_error(connection));
        return SIGN_IN_ERROR;
    }
}

