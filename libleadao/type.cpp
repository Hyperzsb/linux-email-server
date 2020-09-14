#include "type.h"
#include <cstdio>

SQLFeedback::~SQLFeedback() {
    delete[]this->data;
}

Identity::~Identity() {
    delete[]this->ip;
    delete[]this->token;
    delete[]this->account_name;
}

SignInFeedback::~SignInFeedback() {
    delete[]this->token;
    delete[]this->nickname;
    delete[]this->description;
}

RecoverFeedback::~RecoverFeedback() {
    delete[]this->token;
    delete[]this->question;
}


Email::~Email() {
    delete[]this->sender;
    delete[]this->recipient;
    delete[]this->time;
    delete[]this->title;
    delete[]this->body;
}

EmailFeedback::EmailFeedback() {
    this->status = UNEXPECTED_ERROR;
    this->email_num = 0;
    this->email = nullptr;
}

EmailFeedback::~EmailFeedback() {
    for (int i = 0; i < this->email_num; i++)
        delete[]this->email[i];
    delete[]this->email;
}

Contact::~Contact() {
    delete[]this->contact_name;
    delete[]this->alias;
}

ContactFeedback::ContactFeedback() {
    this->status = UNEXPECTED_ERROR;
    this->contact_num = 0;
    this->contact = nullptr;
}

ContactFeedback::~ContactFeedback() {
    for (int i = 0; i < this->contact_num; i++)
        delete[]this->contact[i];
    delete[]this->contact;
}
