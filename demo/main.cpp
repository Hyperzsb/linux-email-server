#include "leadao.h"
#include "type.h"

int main() {
    // Init MySQL connection
    auto *mysql_dao = new MySQL_DAO("127.0.0.1", 3306,
                                    "email_admin", "email_admin_passwd", "email_system");
    // Connect to MySQL server
    mysql_dao->Connect();
    // Sign up function
    SignUpStatus signup_status;
    signup_status = mysql_dao->SignUp("127.0.0.1", "aaa", "bbb", "ccc",
                                      "ddd", "eee", "fff", "ggg");
    if (signup_status == SignUpStatus::SIGN_UP_SUCCESS)
        printf("[console] Account aaa@bbb sign up successfully\n");
    else if (signup_status == SignUpStatus::SIGN_UP_ACCOUNT_CONFLICT)
        printf("[console] Account aaa@bbb sign up unsuccessfully, already exists\n");
    else
        printf("[console] Account aaa@bbb sign up unsuccessfully, unexpected error\n");
    // Sign in function
    SignInFeedback *feedback;
    feedback = mysql_dao->SignIn("127.0.0.1", "aaa@bbb", "ccc");
    printf("[console] Sign in successfully: status: %d, token: %s, nickname: %s, description: %s\n",
           feedback->status, feedback->token, feedback->nickname, feedback->description);
    delete feedback;
    // Send email function
    Email email{};
    email.sender = new char[30];
    email.recipient = new char[30];
    email.title = new char[100];
    email.body = new char[100];
    sprintf(email.sender, "aaa@bbb");
    sprintf(email.recipient, "111@222");
    sprintf(email.title, "Test");
    sprintf(email.body, "Test email.");
    mysql_dao->SendEmail("127.0.0.1", "null", &email);
    // Fetch email function
    EmailFeedBack *email_feedback = mysql_dao->FetchEmail("127.0.0.1", "null",
                                                          "aaa@bbb", EmailType::OUT);
    printf("[console] email_status:%d, email_num:%d\n", email_feedback->status, email_feedback->email_num);
    if (email_feedback->status == EXPECTED_SUCCESS)
        for (int i = 0; i < email_feedback->email_num; i++) {
            printf("[console] Email No.%d: sender: %s, recipient: %s, time: %s, title: %s, body: %s\n", i,
                   email_feedback->email[i]->sender, email_feedback->email[i]->recipient,
                   email_feedback->email[i]->time, email_feedback->email[i]->title, email_feedback->email[i]->body);
        }
    // Close MySQL connection
    delete mysql_dao;

    return 0;
}