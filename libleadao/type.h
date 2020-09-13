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
};

struct SignInFeedback {
    SignInStatus status;
    char *token;
    char *nickname;
    char *description;
};

struct Email {
    char *sender;
    char *recipient;
    char *time;
    char *title;
    char *body;
};

struct EmailFeedBack {
    Status status;
    int email_num;
    Email **email;
};

#endif //LIBLEADAO_TYPE_H
