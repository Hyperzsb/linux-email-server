#include "leadao.h"

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

char *MySQL_DAO::GetAccountHost(const char *account_name) {
    char *host_name = new char[31];
    memset(host_name, 0, 31);
    int account_name_len = strlen(account_name), host_name_len = 0;
    for (int i = 0; i < account_name_len; i++)
        if (account_name[i] != '@') {
            host_name[host_name_len] = account_name[i];
            host_name_len++;
        } else {
            break;
        }
    host_name[host_name_len] = '\0';
    return host_name;
}

char *MySQL_DAO::GetAccountDomain(const char *account_name) {
    char *domain_name = new char[31];
    memset(domain_name, 0, 31);
    int account_name_len = strlen(account_name), host_name_len = 0, domain_name_len = 0;
    for (int i = 0; i < account_name_len; i++)
        if (account_name[i] != '@') {
            host_name_len++;
        } else {
            break;
        }
    for (int i = host_name_len + 1; i < account_name_len; i++) {
        domain_name[domain_name_len] = account_name[i];
        domain_name_len++;
    }
    domain_name[domain_name_len] = '\0';
    return domain_name;
}

char *MySQL_DAO::GetStatusName(Status status) {
    char *name = new char[10];
    switch (status) {
        case EXPECTED_SUCCESS: {
            strcpy(name, "EXPECTED_SUCCESS");
            break;
        }
        case UNEXPECTED_SUCCESS: {
            strcpy(name, "UNEXPECTED_SUCCESS");
            break;
        }
        case EXPECTED_ERROR: {
            strcpy(name, "EXPECTED_ERROR");
            break;
        }
        case UNEXPECTED_ERROR: {
            strcpy(name, "UNEXPECTED_ERROR");
            break;
        }
    }
    return name;
}

char *MySQL_DAO::GetActivityName(ActivityType type) {
    char *name = new char[10];
    switch (type) {
        case SIGN_UP: {
            strcpy(name, "SIGN_UP");
            break;
        }
        case SIGN_IN: {
            strcpy(name, "SIGN_IN");
            break;
        }
        case SIGN_OUT: {
            strcpy(name, "SIGN_OUT");
            break;
        }
        case RECOVER: {
            strcpy(name, "RECOVER");
            break;
        }
        case DELETE: {
            strcpy(name, "DELETE");
            break;
        }
    }
    return name;
}

SQLFeedback *MySQL_DAO::GetAccountID(const char *account_name) {
    char *id, *host_name, *domain_name, query[200] = {0}, log_str[200] = {0};
    auto *feedback = new SQLFeedback;
    host_name = GetAccountHost(account_name), domain_name = GetAccountDomain(account_name);
    sprintf(query, "select id from account_info where host = '%s' and domain = '%s';",
            host_name, domain_name);
    delete[]host_name;
    delete[]domain_name;
    if (mysql_query(connection, query) == 0) {
        MYSQL_RES *result = mysql_store_result(connection);
        if (result != nullptr) {
            if (mysql_num_rows(result) == 0) {
                mysql_free_result(result);
                memset(log_str, 0, 200);
                sprintf(log_str, "No such account '%s'", account_name);
                // Log
                StdLog(WARNING, log_str);
                feedback->status = EXPECTED_ERROR;
                feedback->data = nullptr;
                return feedback;
            }
            MYSQL_ROW result_row;
            result_row = mysql_fetch_row(result);
            feedback->status = EXPECTED_SUCCESS;
            feedback->data = new char[strlen(result_row[0])];
            strcpy(feedback->data, result_row[0]);
            mysql_free_result(result);
            return feedback;
        } else {
            // Log
            StdLog(ERROR, mysql_error(connection));
            feedback->status = UNEXPECTED_ERROR;
            feedback->data = nullptr;
            mysql_free_result(result);
            return feedback;
        }
    } else {
        // Log
        StdLog(ERROR, mysql_error(connection));
        feedback->status = UNEXPECTED_ERROR;
        feedback->data = nullptr;
        return feedback;
    }
}

SQLFeedback *MySQL_DAO::GetAccountName(const char *account_id) {
    char *host_name, *domain_name, query[200] = {0}, log_str[200] = {0};
    auto *feedback = new SQLFeedback;
    sprintf(query, "select host, domain from account_info where id = '%s';", account_id);
    if (mysql_query(connection, query) == 0) {
        MYSQL_RES *result = mysql_store_result(connection);
        if (result != nullptr) {
            if (mysql_num_rows(result) == 0) {
                mysql_free_result(result);
                memset(log_str, 0, 200);
                sprintf(log_str, "No such account id '%s'", account_id);
                // Log
                StdLog(WARNING, log_str);
                feedback->status = EXPECTED_ERROR;
                feedback->data = nullptr;
                return feedback;
            }
            MYSQL_ROW result_row;
            result_row = mysql_fetch_row(result);
            host_name = new char[30], domain_name = new char[30];
            sprintf(host_name, "%s", result_row[0]);
            sprintf(domain_name, "%s", result_row[1]);
            feedback->data = new char[strlen(result_row[0]) + strlen(result_row[1]) + 2];
            sprintf(feedback->data, "%s@%s", host_name, domain_name);
            feedback->status = EXPECTED_SUCCESS;
            mysql_free_result(result);
            return feedback;
        } else {
            // Log
            StdLog(ERROR, mysql_error(connection));
            feedback->status = UNEXPECTED_ERROR;
            feedback->data = nullptr;
            mysql_free_result(result);
            return feedback;
        }
    } else {
        // Log
        StdLog(ERROR, mysql_error(connection));
        feedback->status = UNEXPECTED_ERROR;
        feedback->data = nullptr;
        return feedback;
    }
}

Status MySQL_DAO::AccountActivityLog(const char *ip, const char *account_name, ActivityType type, Status status) {
    char *activity_name = GetActivityName(type);
    char *status_name = GetStatusName(status);
    // Log
    char log_str[200] = {0};
    sprintf(log_str, "Activity log at ( ip %s | account %s | type %s | status %s )",
            ip, account_name, activity_name, status_name);
    StdLog(INFO, log_str);
    // Generate activity ID
    SHA256_CTX sha256_ctx;
    SHA256_Init(&sha256_ctx);
    SHA256_Update(&sha256_ctx, ip, strlen(ip));
    SHA256_Update(&sha256_ctx, account_name, strlen(account_name));
    char time_str[64] = {0};
    time_t time_type;
    time(&time_type);
    sprintf(time_str, "%d", (int) time_type);
    SHA256_Update(&sha256_ctx, time_str, strlen(time_str));
    SHA256_Update(&sha256_ctx, activity_name, strlen(activity_name));
    SHA256_Update(&sha256_ctx, status_name, strlen(status_name));
    unsigned char raw_hash_id[32] = {0};
    SHA256_Final(raw_hash_id, &sha256_ctx);
    char hash_id[16] = {0};
    for (int i = 0; i < 8; i++)
        sprintf(hash_id + i * 2, "%02x", raw_hash_id[i]);
    strftime(time_str, sizeof(time_str), "%Y-%m-%d %H:%M:%S", localtime(&time_type));
    char query[200] = {0};
    sprintf(query, "insert into account_activity (id, account_name, ip, action, time, status) "
                   "value ('%s', '%s', '%s', %d, '%s', %d);", hash_id, account_name, ip, type, time_str, status);
    if (mysql_query(connection, query) == 0) {
        memset(log_str, 0, 200);
        sprintf(log_str, "Activity log at ( ip %s | account %s | type %s | status %s ) successfully",
                ip, account_name, activity_name, status_name);
        StdLog(INFO, log_str);
        delete activity_name;
        delete status_name;
        return EXPECTED_SUCCESS;
    } else {
        StdLog(ERROR, mysql_error(connection));
        delete activity_name;
        delete status_name;
        return EXPECTED_ERROR;
    }
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
    this->mysql_host = new char[strlen(host)];
    strcpy(this->mysql_host, host);
    this->mysql_port = port;
    this->username = new char[strlen(user)];
    strcpy(this->username, user);
    this->password = new char[strlen(passwd)];
    strcpy(this->password, passwd);
    this->database = new char[strlen(db)];
    strcpy(this->database, db);
    // Init connection
    this->connection = mysql_init(nullptr);
    if (connection != nullptr)
        // Log
        StdLog(INFO, "Init connection succeeded");
    else
        // Log
        StdLog(ERROR, "Init connection failed");
}

MySQL_DAO::~MySQL_DAO() {
    if (this->connection != nullptr) {
        mysql_close(connection);
        // Log
        StdLog(INFO, "Closed established connection. Process shutting down");
    } else {
        // Log
        StdLog(WARNING, "No connection has been established. Process shutting down");
    }
    delete this->mysql_host;
    delete this->username;
    delete this->password;
    delete this->database;
}

bool MySQL_DAO::Connect() {
    // Try real connection
    connection = mysql_real_connect(connection, this->mysql_host, this->username, this->password, this->database,
                                    this->mysql_port, nullptr, 0);
    if (connection) {
        // Log
        StdLog(INFO, "Real connection establishment succeeded");
        // Set charset
        if (!mysql_set_character_set(connection, "utf8")) {
            // Log
            char log_str[200] = {0};
            sprintf(log_str, "Successfully set charset: %s", mysql_character_set_name(connection));
            StdLog(INFO, log_str);
        }
        return true;
    } else {
        // Log
        StdLog(ERROR, "Real connection establishment failed");
        return false;
    }
}

// TODO: Add account activity record
SignUpStatus MySQL_DAO::SignUp(const char *ip, const char *host, const char *domain, const char *passwd,
                               const char *nickname, const char *description,
                               const char *recovery_question, const char *recovery_answer) {
    // Log
    char log_str[200] = {0};
    sprintf(log_str, "Account '%s@%s' is trying to sign up", host, domain);
    StdLog(INFO, log_str);
    // Check account (host & domain)
    char account_name[70] = {0};
    sprintf(account_name, "%s@%s", host, domain);
    SQLFeedback *sql_feedback = GetAccountID(account_name);
    if (sql_feedback->status == EXPECTED_ERROR) {
        // Log
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
                "(id, host, domain, password, nickname, description, recovery_question, recovery_answer, locked) "
                "value ('%s', '%s', '%s', '%s', '%s', '%s', '%s', '%s', %d);",
                hash_id, host, domain, passwd, nickname, description,
                recovery_question, recovery_answer, 0);
        if (mysql_query(connection, add_account_query) == 0) {
            delete sql_feedback;
            // Log
            memset(log_str, 0, 200);
            sprintf(log_str, "Add account '%s@%s' successfully", host, domain);
            StdLog(INFO, log_str);
            // Log to MySQL
            AccountActivityLog(ip, account_name, SIGN_UP, EXPECTED_SUCCESS);
            return SIGN_UP_SUCCESS;
        } else {
            delete sql_feedback;
            // Log
            StdLog(ERROR, mysql_error(connection));
            // Log to MySQL
            AccountActivityLog(ip, account_name, SIGN_UP, UNEXPECTED_ERROR);
            return SIGN_UP_ERROR;
        }
    } else if (sql_feedback->status == EXPECTED_SUCCESS) {
        delete sql_feedback;
        memset(log_str, 0, 200);
        sprintf(log_str, "Account '%s@%s' already exists", host, domain);
        // Log
        StdLog(WARNING, log_str);
        // Log to MySQL
        AccountActivityLog(ip, account_name, SIGN_UP, EXPECTED_ERROR);
        return SIGN_UP_ACCOUNT_CONFLICT;
    } else {
        delete sql_feedback;
        // Log
        StdLog(ERROR, mysql_error(connection));
        // Log to MySQL
        AccountActivityLog(ip, account_name, SIGN_UP, UNEXPECTED_ERROR);
        return SIGN_UP_ERROR;
    }
}

// TODO: Add account activity record
SignInFeedback *MySQL_DAO::SignIn(const char *ip, const char *account_name, const char *account_passwd) {
    // Log
    char log_str[200] = {0};
    sprintf(log_str, "Account '%s' is trying to login", account_name);
    StdLog(INFO, log_str);
    // Verify account name
    char *host_name = GetAccountHost(account_name), *domain_name = GetAccountDomain(account_name);
    char query[200] = {0};
    sprintf(query, "select id, password from account_info where host = '%s' and domain = '%s';",
            host_name, domain_name);
    delete[]host_name;
    delete[]domain_name;
    if (mysql_query(connection, query) == 0) {
        MYSQL_RES *result = mysql_store_result(connection);
        if (result != nullptr) {
            if (mysql_num_rows(result) == 0) {
                // Log
                memset(log_str, 0, 200);
                sprintf(log_str, "No such account '%s'", account_name);
                StdLog(WARNING, log_str);
                auto *feedback = new SignInFeedback;
                feedback->status = SIGN_IN_INVALID_ACCOUNT;
                // Log to MySQL
                AccountActivityLog(ip, account_name, SIGN_IN, EXPECTED_ERROR);
                return feedback;
            }
            MYSQL_ROW result_row;
            result_row = mysql_fetch_row(result);
            char *id = result_row[0];
            char *valid_passwd = result_row[1];
            mysql_free_result(result);
            if (strcmp(account_passwd, valid_passwd) == 0) {
                // Log
                memset(log_str, 0, 200);
                sprintf(log_str, "Account '%s' matches its given password",
                        account_name);
                StdLog(INFO, log_str);
                auto *feedback = new SignInFeedback;
                feedback->status = SIGN_IN_SUCCESS;
                // TODO: Add account token record
                // Generate token
                SHA256_CTX sha256_ctx;
                SHA256_Init(&sha256_ctx);
                SHA256_Update(&sha256_ctx, ip, strlen(ip));
                SHA256_Update(&sha256_ctx, account_name, strlen(account_name));
                char time_str[20] = {0};
                time_t time_type;
                time(&time_type);
                sprintf(time_str, "%d", (int) time_type);
                SHA256_Update(&sha256_ctx, time_str, strlen(time_str));
                unsigned char raw_token[32] = {0};
                SHA256_Final(raw_token, &sha256_ctx);
                char *token = new char[16];
                for (int i = 0; i < 8; i++)
                    sprintf(token + i * 2, "%02x", raw_token[i]);
                feedback->token = token;
                // Get account info details
                memset(query, 0, 200);
                sprintf(query, "select nickname, description from account_info where id = '%s';", id);
                if (mysql_query(connection, query) == 0) {
                    result = mysql_store_result(connection);
                    if (result != nullptr) {
                        result_row = mysql_fetch_row(result);
                        char *nickname = new char[strlen(result_row[0])];
                        strcpy(nickname, result_row[0]);
                        feedback->nickname = nickname;
                        char *description = new char[strlen(result_row[1])];
                        strcpy(description, result_row[1]);
                        feedback->description = description;
                    } else {
                        // Log
                        StdLog(ERROR, mysql_error(connection));
                    }
                    mysql_free_result(result);
                } else {
                    // Log
                    StdLog(ERROR, mysql_error(connection));
                }
                // Log
                memset(log_str, 0, 200);
                sprintf(log_str, "Account '%s' sign in successfully", account_name);
                StdLog(INFO, log_str);
                // Log to MySQL
                AccountActivityLog(ip, account_name, SIGN_IN, EXPECTED_SUCCESS);
                return feedback;
            } else {
                auto *feedback = new SignInFeedback;
                feedback->status = SIGN_IN_INVALID_PASSWD;
                // Log
                memset(log_str, 0, 200);
                sprintf(log_str, "Account '%s' does not match its password, sign in unsuccessfully",
                        account_name);
                StdLog(WARNING, log_str);
                // Log to MySQL
                AccountActivityLog(ip, account_name, SIGN_IN, EXPECTED_ERROR);
                return feedback;
            }
        } else {
            auto *feedback = new SignInFeedback;
            feedback->status = SIGN_IN_ERROR;
            // Log
            StdLog(ERROR, mysql_error(connection));
            mysql_free_result(result);
            // Log to MySQL
            AccountActivityLog(ip, account_name, SIGN_IN, EXPECTED_ERROR);
            return feedback;
        }
    } else {
        auto *feedback = new SignInFeedback;
        feedback->status = SIGN_IN_ERROR;
        // Log
        StdLog(ERROR, mysql_error(connection));
        // Log to MySQL
        AccountActivityLog(ip, account_name, SIGN_IN, EXPECTED_ERROR);
        return feedback;
    }
}

// TODO: Add account activity record
RecoverFeedback *MySQL_DAO::GetRecoverQuestion(const char *ip, const char *account_name) {
    // Log
    char log_str[200] = {0};
    sprintf(log_str, "Account '%s' is trying to get recovery question", account_name);
    StdLog(INFO, log_str);
    auto *recover_feedback = new RecoverFeedback;
    // Get sender account ID
    char *account_id;
    SQLFeedback *sender_feedback = GetAccountID(account_name);
    if (sender_feedback->status == EXPECTED_SUCCESS) {
        account_id = sender_feedback->data;
    } else {
        recover_feedback->status = UNEXPECTED_ERROR;
        recover_feedback->token = nullptr;
        recover_feedback->question = nullptr;
        // Log
        StdLog(ERROR, mysql_error(connection));
        // Log to MySQL
        AccountActivityLog(ip, account_name, RECOVER, UNEXPECTED_ERROR);
        return recover_feedback;
    }
    char query[200] = {0};
    sprintf(query, "select recovery_question from account_info where id = '%s';", account_id);
    if (mysql_query(connection, query) == 0) {
        MYSQL_RES *result = mysql_store_result(connection);
        recover_feedback->status = EXPECTED_SUCCESS;
        recover_feedback->token = nullptr;
        MYSQL_ROW result_row = mysql_fetch_row(result);
        recover_feedback->question = new char[strlen(result_row[0])];
        strcpy(recover_feedback->question, result_row[0]);
        // Log
        memset(log_str, 0, 200);
        sprintf(log_str, "Account '%s' gets recovery question successfully", account_name);
        StdLog(INFO, log_str);
        // Log to MySQL
        AccountActivityLog(ip, account_name, RECOVER, EXPECTED_SUCCESS);
        return recover_feedback;
    } else {
        recover_feedback->status = UNEXPECTED_ERROR;
        recover_feedback->token = nullptr;
        recover_feedback->question = nullptr;
        // Log
        StdLog(ERROR, mysql_error(connection));
        // Log to MySQL
        AccountActivityLog(ip, account_name, RECOVER, UNEXPECTED_ERROR);
        return recover_feedback;
    }
}

// TODO: Add account activity record
RecoverStatus MySQL_DAO::Recover(const char *ip, const char *token, const char *account_name,
                                 const char *answer, const char *passwd) {
    // Log
    char log_str[200] = {0};
    sprintf(log_str, "Account '%s' is trying to recover", account_name);
    StdLog(INFO, log_str);
    // Get sender account ID
    char *account_id;
    SQLFeedback *sender_feedback = GetAccountID(account_name);
    if (sender_feedback->status == EXPECTED_SUCCESS) {
        account_id = sender_feedback->data;
    } else {
        // Log
        StdLog(ERROR, mysql_error(connection));
        // Log to MySQL
        AccountActivityLog(ip, account_name, RECOVER, UNEXPECTED_ERROR);
        return RECOVER_ERROR;
    }
    char query[200] = {0};
    sprintf(query, "select recovery_answer from account_info where id = '%s';", account_id);
    if (mysql_query(connection, query) == 0) {
        MYSQL_RES *result = mysql_store_result(connection);
        MYSQL_ROW result_row = mysql_fetch_row(result);
        if (strcmp(answer, result_row[0]) == 0) {
            memset(query, 0, 200);
            sprintf(query, "update account_info set password = '%s' where id = '%s';", passwd, account_id);
            if (mysql_query(connection, query) == 0) {
                // Log
                memset(log_str, 0, 200);
                sprintf(log_str, "Account '%s' recovers successfully", account_name);
                StdLog(INFO, log_str);
                // Log to MySQL
                AccountActivityLog(ip, account_name, RECOVER, EXPECTED_SUCCESS);
                return RECOVER_SUCCESS;
            } else {
                // Log
                StdLog(ERROR, mysql_error(connection));
                // Log to MySQL
                AccountActivityLog(ip, account_name, RECOVER, UNEXPECTED_ERROR);
                return RECOVER_ERROR;
            }
        } else {
            // Log
            memset(log_str, 0, 200);
            sprintf(log_str, "Account '%s' recovery failed: invalid answer", account_name);
            StdLog(WARNING, log_str);
            // Log to MySQL
            AccountActivityLog(ip, account_name, RECOVER, EXPECTED_ERROR);
            return RECOVER_INVALID_ANSWER;
        }
    } else {
        // Log
        StdLog(ERROR, mysql_error(connection));
        // Log to MySQL
        AccountActivityLog(ip, account_name, RECOVER, UNEXPECTED_ERROR);
        return RECOVER_ERROR;
    }
}

Status MySQL_DAO::Delete(const char *ip, const char *token, const char *account_name) {
    // Log
    char log_str[200] = {0};
    sprintf(log_str, "Account '%s' is trying to delete its account", account_name);
    StdLog(INFO, log_str);
    // Get sender account ID
    char *account_id;
    SQLFeedback *sender_feedback = GetAccountID(account_name);
    if (sender_feedback->status == EXPECTED_SUCCESS) {
        account_id = sender_feedback->data;
    } else {
        // Log
        StdLog(ERROR, mysql_error(connection));
        // Log to MySQL
        AccountActivityLog(ip, account_name, DELETE, UNEXPECTED_ERROR);
        return UNEXPECTED_ERROR;
    }
    char query[200] = {0};
    sprintf(query, "delete from account_info where id = '%s';", account_id);
    if (mysql_query(connection, query) == 0) {
        // Log
        memset(log_str, 0, 200);
        sprintf(log_str, "Account '%s' deletes its account successfully", account_name);
        // Log to MySQL
        AccountActivityLog(ip, account_name, DELETE, EXPECTED_SUCCESS);
        return EXPECTED_SUCCESS;
    } else {
        // Log
        StdLog(ERROR, mysql_error(connection));
        // Log to MySQL
        AccountActivityLog(ip, account_name, DELETE, UNEXPECTED_ERROR);
        return UNEXPECTED_ERROR;
    }
}

Status MySQL_DAO::SendEmail(const char *ip, const char *token, const char *account_name, Email *email) {
    // TODO: Validate account authentication status
    // Log
    char log_str[200] = {0};
    sprintf(log_str, "Account '%s' is trying to send email to account '%s'",
            email->sender, email->recipient);
    StdLog(INFO, log_str);
    // Get account ID
    char *sender_id, *recipient_id;
    // Get sender account ID
    SQLFeedback *sender_feedback = GetAccountID(email->sender);
    if (sender_feedback->status == EXPECTED_SUCCESS) {
        sender_id = sender_feedback->data;
    } else {
        // Log
        StdLog(ERROR, mysql_error(connection));
        return UNEXPECTED_ERROR;
    }
    // Get recipient account sID
    SQLFeedback *recipient_feedback = GetAccountID(email->recipient);
    if (recipient_feedback->status == EXPECTED_SUCCESS) {
        recipient_id = recipient_feedback->data;
    } else {
        // Log
        StdLog(ERROR, mysql_error(connection));
        return UNEXPECTED_ERROR;
    }
    // Add email to database
    SHA256_CTX sha256_ctx;
    SHA256_Init(&sha256_ctx);
    SHA256_Update(&sha256_ctx, sender_id, strlen(sender_id));
    SHA256_Update(&sha256_ctx, recipient_id, strlen(recipient_id));
    char time_str[20] = {0};
    time_t time_type;
    time(&time_type);
    sprintf(time_str, "%d", (int) time_type);
    SHA256_Update(&sha256_ctx, time_str, strlen(time_str));
    unsigned char raw_email_id[32] = {0};
    SHA256_Final(raw_email_id, &sha256_ctx);
    char email_id[17] = {0};
    for (int i = 0; i < 8; i++)
        sprintf(email_id + i * 2, "%02x", raw_email_id[i]);
    email_id[16] = '\0';
    strftime(time_str, sizeof(time_str), "%Y-%m-%d %H:%M:%S", localtime(&time_type));
    char query[11000] = {0};
    sprintf(query, "insert into email (id, sender_id, recipient_id, time, title, body, status)"
                   "value ('%s', '%s', '%s', '%s', '%s', '%s', %d);",
            email_id, sender_id, recipient_id, time_str, email->title, email->body, 0);
    delete sender_feedback;
    delete recipient_feedback;
    if (mysql_query(connection, query) == 0) {
        // Log
        memset(log_str, 0, 200);
        sprintf(log_str, "Sending email from '%s' to '%s' successfully", email->sender, email->recipient);
        StdLog(INFO, log_str);
        return EXPECTED_SUCCESS;
    } else {
        // Log
        StdLog(ERROR, mysql_error(connection));
        return UNEXPECTED_ERROR;
    }
}

EmailFeedback *MySQL_DAO::FetchEmail(const char *ip, const char *token, const char *account_name, EmailType type) {
    // TODO: Validate account authentication status
    // Log
    char log_str[200] = {0};
    sprintf(log_str, "Account '%s' is trying to fetch email", account_name);
    StdLog(INFO, log_str);
    // Get account ID
    auto *email_feedback = new EmailFeedback;
    char *account_id;
    SQLFeedback *sql_feedback = GetAccountID(account_name);
    if (sql_feedback->status == EXPECTED_SUCCESS) {
        account_id = sql_feedback->data;
    } else {
        // Log
        StdLog(ERROR, mysql_error(connection));
        email_feedback->status = UNEXPECTED_ERROR;
        email_feedback->email_num = 0;
        email_feedback->email = nullptr;
        return email_feedback;
    }
    char query[200] = {0};
    if (type == IN)
        sprintf(query, "select sender_id, recipient_id, time, title, body from email "
                       "where recipient_id = '%s';", account_id);
    else
        sprintf(query, "select sender_id, recipient_id, time, title, body from email "
                       "where sender_id = '%s';", account_id);
    if (mysql_query(connection, query) == 0) {
        MYSQL_RES *result = mysql_store_result(connection);
        int email_num = mysql_num_rows(result);
        email_feedback->status = EXPECTED_SUCCESS;
        email_feedback->email_num = email_num;
        email_feedback->email = new Email *[email_num];
        MYSQL_ROW result_row;
        for (int i = 0; i < email_num; i++) {
            result_row = mysql_fetch_row(result);
            email_feedback->email[i] = new Email;
            SQLFeedback *name_sql_feedback = GetAccountName(result_row[0]);
            email_feedback->email[i]->sender = new char[strlen(name_sql_feedback->data)];
            strcpy(email_feedback->email[i]->sender, name_sql_feedback->data);
            delete name_sql_feedback;
            name_sql_feedback = GetAccountName(result_row[1]);
            email_feedback->email[i]->recipient = new char[strlen(name_sql_feedback->data)];
            strcpy(email_feedback->email[i]->recipient, name_sql_feedback->data);
            delete name_sql_feedback;
            email_feedback->email[i]->time = new char[strlen(result_row[2])];
            strcpy(email_feedback->email[i]->time, result_row[2]);
            email_feedback->email[i]->title = new char[strlen(result_row[3])];
            strcpy(email_feedback->email[i]->title, result_row[3]);
            email_feedback->email[i]->body = new char[strlen(result_row[4])];
            strcpy(email_feedback->email[i]->body, result_row[4]);
        }
        // Log
        memset(log_str, 0, 200);
        sprintf(log_str, "Account '%s' fetches email successfully", account_name);
        StdLog(INFO, log_str);
        return email_feedback;
    } else {
        // Log
        StdLog(ERROR, mysql_error(connection));
        email_feedback->status = UNEXPECTED_ERROR;
        email_feedback->email_num = 0;
        email_feedback->email = nullptr;
        return email_feedback;
    }
}

Status MySQL_DAO::SaveDraft(const char *ip, const char *token, const char *account_name, Email *draft) {
    // TODO: Validate account authentication status
    // Log
    char log_str[200] = {0};
    sprintf(log_str, "Account '%s' is trying to save draft", draft->sender);
    StdLog(INFO, log_str);
    // Get account ID
    char *account_id;
    SQLFeedback *sql_feedback = GetAccountID(draft->sender);
    if (sql_feedback->status == EXPECTED_SUCCESS) {
        account_id = sql_feedback->data;
    } else {
        // Log
        StdLog(ERROR, mysql_error(connection));
        return UNEXPECTED_ERROR;
    }
    // Add email to database
    SHA256_CTX sha256_ctx;
    SHA256_Init(&sha256_ctx);
    SHA256_Update(&sha256_ctx, account_id, strlen(account_id));
    if (draft->recipient != nullptr)
        SHA256_Update(&sha256_ctx, draft->recipient, strlen(draft->recipient));
    char time_str[20] = {0};
    time_t time_type;
    time(&time_type);
    sprintf(time_str, "%d", (int) time_type);
    SHA256_Update(&sha256_ctx, time_str, strlen(time_str));
    unsigned char raw_email_id[32] = {0};
    SHA256_Final(raw_email_id, &sha256_ctx);
    char email_id[17] = {0};
    for (int i = 0; i < 8; i++)
        sprintf(email_id + i * 2, "%02x", raw_email_id[i]);
    email_id[16] = '\0';
    strftime(time_str, sizeof(time_str), "%Y-%m-%d %H:%M:%S", localtime(&time_type));
    char query[11000] = {0};
    sprintf(query, "insert into draft (id, sender_id, recipient_name, time, title, body)"
                   "value ('%s', '%s', '%s', '%s', '%s', '%s');",
            email_id, account_id, draft->recipient, time_str, draft->title, draft->body);
    if (mysql_query(connection, query) == 0) {
        // Log
        memset(log_str, 0, 200);
        sprintf(log_str, "Saving draft to account '%s' successfully", draft->sender);
        StdLog(INFO, log_str);
        return EXPECTED_SUCCESS;
    } else {
        // Log
        StdLog(ERROR, mysql_error(connection));
        return UNEXPECTED_ERROR;
    }
}

EmailFeedback *MySQL_DAO::FetchDraft(const char *ip, const char *token, const char *account_name) {
    // TODO: Validate account authentication status
    // Log
    char log_str[200] = {0};
    sprintf(log_str, "Account '%s' is trying to fetch draft", account_name);
    StdLog(INFO, log_str);
    // Get account ID
    auto *draft_feedback = new EmailFeedback;
    char *account_id;
    SQLFeedback *sql_feedback = GetAccountID(account_name);
    if (sql_feedback->status == EXPECTED_SUCCESS) {
        account_id = sql_feedback->data;
    } else {
        // Log
        StdLog(ERROR, mysql_error(connection));
        draft_feedback->status = UNEXPECTED_ERROR;
        draft_feedback->email_num = 0;
        draft_feedback->email = nullptr;
        return draft_feedback;
    }
    char query[200] = {0};
    sprintf(query, "select sender_id, recipient_name, time, title, body from draft "
                   "where sender_id = '%s';", account_id);
    if (mysql_query(connection, query) == 0) {
        MYSQL_RES *result = mysql_store_result(connection);
        int draft_num = mysql_num_rows(result);
        draft_feedback->status = EXPECTED_SUCCESS;
        draft_feedback->email_num = draft_num;
        draft_feedback->email = new Email *[draft_num];
        MYSQL_ROW result_row;
        for (int i = 0; i < draft_num; i++) {
            result_row = mysql_fetch_row(result);
            draft_feedback->email[i] = new Email;
            SQLFeedback *name_sql_feedback = GetAccountName(result_row[0]);
            draft_feedback->email[i]->sender = new char[strlen(name_sql_feedback->data)];
            strcpy(draft_feedback->email[i]->sender, name_sql_feedback->data);
            delete name_sql_feedback;
            draft_feedback->email[i]->recipient = new char[strlen(result_row[1])];
            strcpy(draft_feedback->email[i]->recipient, result_row[1]);
            draft_feedback->email[i]->time = new char[strlen(result_row[2])];
            strcpy(draft_feedback->email[i]->time, result_row[2]);
            draft_feedback->email[i]->title = new char[strlen(result_row[3])];
            strcpy(draft_feedback->email[i]->title, result_row[3]);
            draft_feedback->email[i]->body = new char[strlen(result_row[4])];
            strcpy(draft_feedback->email[i]->body, result_row[4]);
        }
        // Log
        memset(log_str, 0, 200);
        sprintf(log_str, "Account '%s' fetches draft successfully", account_name);
        StdLog(INFO, log_str);
        return draft_feedback;
    } else {
        // Log
        StdLog(ERROR, mysql_error(connection));
        draft_feedback->status = UNEXPECTED_ERROR;
        draft_feedback->email_num = 0;
        draft_feedback->email = nullptr;
        return draft_feedback;
    }
}

Status MySQL_DAO::SetContact(const char *ip, const char *token, const char *account_name, Contact *contact) {
    // TODO: Validate account authentication status
    // Log
    char log_str[200] = {0};
    sprintf(log_str, "Account '%s' is trying to add contact '%s'", account_name, contact->contact_name);
    StdLog(INFO, log_str);
    // Get owner ID
    char *owner_id;
    SQLFeedback *owner_sql_feedback = GetAccountID(account_name);
    if (owner_sql_feedback->status == EXPECTED_SUCCESS) {
        owner_id = owner_sql_feedback->data;
    } else {
        // Log
        StdLog(ERROR, mysql_error(connection));
        return UNEXPECTED_ERROR;
    }
    // Get contact ID
    char *contact_id;
    SQLFeedback *contact_sql_feedback = GetAccountID(contact->contact_name);
    if (contact_sql_feedback->status == EXPECTED_SUCCESS) {
        contact_id = contact_sql_feedback->data;
    } else {
        // Log
        StdLog(ERROR, mysql_error(connection));
        return UNEXPECTED_ERROR;
    }
    char query[200] = {0};
    sprintf(query, "insert into contact (owner_id, contact_id, alias) value ('%s', '%s', '%s');",
            owner_id, contact_id, contact->alias);
    delete owner_sql_feedback;
    delete contact_sql_feedback;
    if (mysql_query(connection, query) == 0) {
        // Log
        memset(log_str, 0, 200);
        sprintf(log_str, "Add contact '%s' to '%s' successfully", contact->contact_name, account_name);
        StdLog(INFO, log_str);
        return EXPECTED_SUCCESS;
    } else {
        // Log
        StdLog(ERROR, mysql_error(connection));
        return UNEXPECTED_ERROR;
    }
}

ContactFeedback *MySQL_DAO::GetContact(const char *ip, const char *token, const char *account_name) {
    // TODO: Validate account authentication status
    // Log
    char log_str[200] = {0};
    sprintf(log_str, "Account '%s' is trying to get contact", account_name);
    StdLog(INFO, log_str);
    // Get account ID
    auto contact_feedback = new ContactFeedback;
    char *account_id;
    SQLFeedback *sql_feedback = GetAccountID(account_name);
    if (sql_feedback->status == EXPECTED_SUCCESS) {
        account_id = sql_feedback->data;
    } else {
        // Log
        StdLog(ERROR, mysql_error(connection));
        contact_feedback->status = UNEXPECTED_ERROR;
        contact_feedback->contact_num = 0;
        contact_feedback->contact = nullptr;
        return contact_feedback;
    }
    char query[200] = {0};
    sprintf(query, "select contact_id, alias from contact where owner_id = '%s';", account_id);
    if (mysql_query(connection, query) == 0) {
        MYSQL_RES *result = mysql_store_result(connection);
        int contact_num = mysql_num_rows(result);
        contact_feedback->status = EXPECTED_SUCCESS;
        contact_feedback->contact_num = contact_num;
        contact_feedback->contact = new Contact *[contact_num];
        char **contact = new char *[contact_num], **alias = new char *[contact_num];
        MYSQL_ROW result_row;
        for (int i = 0; i < contact_num; i++) {
            result_row = mysql_fetch_row(result);
            contact_feedback->contact[i] = new Contact;
            contact[i] = new char[strlen(result_row[0])];
            strcpy(contact[i], result_row[0]);
            contact_feedback->contact[i]->contact_name = contact[i];
            alias[i] = new char[strlen(result_row[1])];
            strcpy(alias[i], result_row[1]);
            contact_feedback->contact[i]->alias = alias[i];
        }
        // Log
        memset(log_str, 0, 200);
        sprintf(log_str, "Account '%s' fetches email successfully", account_name);
        StdLog(INFO, log_str);
        return contact_feedback;
    } else {
        // Log
        StdLog(ERROR, mysql_error(connection));
        contact_feedback->status = UNEXPECTED_ERROR;
        contact_feedback->contact_num = 0;
        contact_feedback->contact = nullptr;
        return contact_feedback;
    }
}
