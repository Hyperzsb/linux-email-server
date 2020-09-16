#include "type.h"

SQLFeedback::SQLFeedback() {
    this->data = nullptr;
}

SQLFeedback::~SQLFeedback() {
    delete[]this->data;
}

Identity::Identity() {
    this->ip = nullptr;
    this->token = nullptr;
    this->account_name = nullptr;
}

Identity::~Identity() {
    delete[]this->ip;
    delete[]this->token;
    delete[]this->account_name;
}

SignInFeedback::SignInFeedback() {
    this->status = SIGN_IN_ERROR;
    this->token = nullptr;
    this->nickname = nullptr;
    this->description = nullptr;
}

SignInFeedback::~SignInFeedback() {
    delete[]this->token;
    delete[]this->nickname;
    delete[]this->description;
}

RecoverFeedback::RecoverFeedback() {
    this->status = UNEXPECTED_ERROR;
    this->token = nullptr;
    this->question = nullptr;
}

RecoverFeedback::~RecoverFeedback() {
    delete[]this->token;
    delete[]this->question;
}

Email::Email() {
    this->sender = nullptr;
    this->recipient = nullptr;
    this->time = nullptr;
    this->title = nullptr;
    this->body = nullptr;
    this->accessory_route = nullptr;
}

Email::~Email() {
    delete[]this->sender;
    delete[]this->recipient;
    delete[]this->time;
    delete[]this->title;
    delete[]this->body;
    delete[]this->accessory_route;
}

EmailFeedback::EmailFeedback() {
    this->status = UNEXPECTED_ERROR;
    this->email_num = 0;
    this->email = nullptr;
}

EmailFeedback::~EmailFeedback() {
    for (int i = 0; i < this->email_num; i++)
        delete this->email[i];
    delete[]this->email;
}

Contact::Contact() {
    this->contact_name = nullptr;
    this->alias = nullptr;
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
        delete this->contact[i];
    delete[]this->contact;
}
