#ifndef LIBLEADAO_TYPE_H
#define LIBLEADAO_TYPE_H

// Enums

enum LogLevel {
    INFO = 0,
    WARNING = 1,
    ERROR = 2
};

enum Status {
    EXPECTED_SUCCESS = 0,
    UNEXPECTED_SUCCESS = 1,
    EXPECTED_ERROR = 2,
    UNEXPECTED_ERROR = 3
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

enum RecoverStatus {
    RECOVER_SUCCESS = 0,
    RECOVER_ERROR = 1,
    RECOVER_INVALID_ANSWER = 2
};

enum EmailType {
    IN = 0,
    OUT = 1
};

// Structures

struct SQLFeedback {
    Status status;
    char *data;
    ~SQLFeedback();
};

struct Identity {
    char *ip;
    char *token;
    char *account_name;
    ~Identity();
};

struct SignInFeedback {
    SignInStatus status;
    char *token;
    char *nickname;
    char *description;
    ~SignInFeedback();
};

struct RecoverFeedback {
    Status status;
    char *token;
    char *question;
    ~RecoverFeedback();
};

struct Email {
    char *sender;
    char *recipient;
    char *time;
    char *title;
    char *body;
    ~Email();
};

struct EmailFeedback {
    Status status;
    int email_num;
    Email **email;
    EmailFeedback();
    ~EmailFeedback();
};

struct Contact {
    char *contact_name;
    char *alias;
    ~Contact();
};

struct ContactFeedback {
    Status status;
    int contact_num;
    Contact **contact;
    ContactFeedback();
    ~ContactFeedback();
};

#endif //LIBLEADAO_TYPE_H
