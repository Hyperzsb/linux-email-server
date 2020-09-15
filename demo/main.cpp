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
    signup_status = mysql_dao->SignUp("127.0.0.1", "111", "222", "333",
                                      "444", "555", "666", "777");
    if (signup_status == SignUpStatus::SIGN_UP_SUCCESS)
        printf("[console] Account 111@222 sign up successfully\n");
    else if (signup_status == SignUpStatus::SIGN_UP_ACCOUNT_CONFLICT)
        printf("[console] Account 111@222 sign up unsuccessfully, already exists\n");
    else
        printf("[console] Account 111@222 sign up unsuccessfully, unexpected error\n");
    // Sign in function
    SignInFeedback *feedback;
    feedback = mysql_dao->SignIn("127.0.0.1", "aaa@bbb", "ccc");
    printf("[console] Sign in successfully: status: %d, token: %s, nickname: %s, description: %s\n",
           feedback->status, feedback->token, feedback->nickname, feedback->description);
    delete feedback;
    feedback = mysql_dao->SignIn("127.0.0.1", "111@222", "333");
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
    sprintf(email.body, "Just a test email.");
    mysql_dao->SendEmail("127.0.0.1", "null", "aaa@bbb", &email);
    // Fetch email function
    EmailFeedback *email_feedback = mysql_dao->FetchEmail("127.0.0.1", "null",
                                                          "aaa@bbb", EmailType::OUT);
    printf("[console] email_status:%d, email_num:%d\n", email_feedback->status, email_feedback->email_num);
    if (email_feedback->status == EXPECTED_SUCCESS)
        for (int i = 0; i < email_feedback->email_num; i++) {
            printf("[console] Email No.%d: sender: %s, recipient: %s, time: %s, title: %s, body: %s\n", i,
                   email_feedback->email[i]->sender, email_feedback->email[i]->recipient,
                   email_feedback->email[i]->time, email_feedback->email[i]->title, email_feedback->email[i]->body);
        }
    delete email_feedback;
    email_feedback = mysql_dao->FetchEmail("127.0.0.1", "null",
                                           "111@222", EmailType::IN);
    printf("[console] email_status:%d, email_num:%d\n", email_feedback->status, email_feedback->email_num);
    if (email_feedback->status == EXPECTED_SUCCESS)
        for (int i = 0; i < email_feedback->email_num; i++) {
            printf("[console] Email No.%d: sender: %s, recipient: %s, time: %s, title: %s, body: %s\n", i,
                   email_feedback->email[i]->sender, email_feedback->email[i]->recipient,
                   email_feedback->email[i]->time, email_feedback->email[i]->title, email_feedback->email[i]->body);
        }
    delete email_feedback;
    // Save draft function
    Email draft{};
    draft.sender = new char[30];
    draft.recipient = new char[30];
    draft.title = new char[100];
    draft.body = new char[100];
    sprintf(draft.sender, "aaa@bbb");
    sprintf(draft.recipient, "111@222");
    sprintf(draft.title, "Test");
    sprintf(draft.body, "Just a test draft.");
    mysql_dao->SaveDraft("127.0.0.1", "null", "aaa@bbb", &draft);
    strcpy(draft.recipient, "");
    mysql_dao->SaveDraft("127.0.0.1", "null", "aaa@bbb", &draft);
    // Fetch draft function
    EmailFeedback *draft_feedback = mysql_dao->FetchDraft("127.0.0.1", "null", "aaa@bbb");
    printf("[console] email_status:%d, email_num:%d\n", draft_feedback->status, draft_feedback->email_num);
    if (draft_feedback->status == EXPECTED_SUCCESS)
        for (int i = 0; i < draft_feedback->email_num; i++) {
            printf("[console] Email No.%d: sender: %s, recipient: %s, time: %s, title: %s, body: %s\n", i,
                   draft_feedback->email[i]->sender, draft_feedback->email[i]->recipient,
                   draft_feedback->email[i]->time, draft_feedback->email[i]->title, draft_feedback->email[i]->body);
        }
    delete draft_feedback;
    // Get recovery question
    RecoverFeedback *recover_feedback = mysql_dao->GetRecoverQuestion("127.0.0.1", "aaa@bbb");
    if (recover_feedback->status == Status::EXPECTED_SUCCESS) {
        printf("[console] Recovery question: %s\n", recover_feedback->question);
    } else {
        printf("[console] Get recovery question unsuccessfully\n");
    }
    delete recover_feedback;
    // Recover
    RecoverStatus recover_status = mysql_dao->Recover("127.0.0.1", "null", "aaa@bbb", "ggg", "zzz");
    if (recover_status == RECOVER_SUCCESS) {
        printf("[console] Account 'aaa@bbb' recovers successfully\n");
    } else if (recover_status == RECOVER_INVALID_ANSWER) {
        printf("[console] Account 'aaa@bbb' recovers unsuccessfully: invalid answer\n");
    } else {
        printf("[console] Account 'aaa@bbb' recovers unsuccessfully\n");
    }
    // Delete
    Status delete_status = mysql_dao->Delete("127.0.0.1", "null", "aaa@bbb");
    if (delete_status == EXPECTED_SUCCESS) {
        printf("[console] Account 'aaa@bbb' deletes itself successfully\n");
    } else {
        printf("[console] Account 'aaa@bbb' deletes itself unsuccessfully\n");
    }
    // Close MySQL connection
    delete mysql_dao;

    return 0;
}