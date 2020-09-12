#include "leadao.h"
#include "type.h"

int main() {
    auto *mySqlDao = new MySQL_DAO("127.0.0.1", 3306,
                                   "email_admin", "email_admin_passwd", "email_system");
    mySqlDao->Connect();
    SignInFeedback *feedback;
    feedback = mySqlDao->SignIn("127.0.0.1", "aaa@bbb", "ccc");
    printf("status: %d, token: %s, nickname: %s, description: %s\n",
           feedback->status, feedback->token, feedback->nickname, feedback->description);
    delete feedback;
    feedback = mySqlDao->SignIn("127.0.0.1", "111@222", "333");
    printf("status: %d, token: %s, nickname: %s, description: %s\n",
           feedback->status, feedback->token, feedback->nickname, feedback->description);
    delete feedback;

    Email email{};
    email.sender = new char[30];
    email.recipient = new char[30];
    email.title = new char[100];
    email.body = new char[100];
    sprintf(email.sender, "aaa@bbb");
    sprintf(email.recipient, "111@222");
    sprintf(email.title, "Test");
    sprintf(email.body, "Test email.");
    mySqlDao->SendEmail("127.0.0.1", "null", &email);

//    EmailFeedBack *email_feedback = mySqlDao->FetchEmail("127.0.0.1", "null",
//                                                         "111@222", EmailType::OUT);
//    printf("email_status:%d, email_num:%d\n", email_feedback->status, email_feedback->email_num);
//    if (email_feedback->status == EXPECTED_SUCCESS)
//        for (int i = 0; i < email_feedback->email_num; i++) {
//            printf("sender: %s, recipient: %s, time: %s, title: %s, body: %s\n",
//                   email_feedback->email[i]->sender, email_feedback->email[i]->recipient,
//                   email_feedback->email[i]->time, email_feedback->email[i]->title, email_feedback->email[i]->body);
//        }

    delete mySqlDao;

    return 0;
}