#define CATCH_CONFIG_MAIN

#include "catch.hpp"
#include "leadao.h"
#include "type.h"

TEST_CASE("Connect") {
    auto *mysql_dao = new MySQL_DAO("127.0.0.1", 3306,
                                    "email_admin", "email_admin_passwd", "email_system");
    REQUIRE(mysql_dao->Connect() == true);
    delete mysql_dao;
}

TEST_CASE("SignUp") {
    auto *mysql_dao = new MySQL_DAO("127.0.0.1", 3306,
                                    "email_admin", "email_admin_passwd", "email_system");
    REQUIRE(mysql_dao->Connect() == true);
    SignUpStatus sign_up_status;
    sign_up_status = mysql_dao->SignUp("127.0.0.1", "sign_up", "catch.org", "passwd",
                                       "unit-test", "Catch2 unit test example", "Question", "Answer");
    REQUIRE((sign_up_status == SIGN_UP_SUCCESS || sign_up_status == SIGN_UP_ACCOUNT_CONFLICT));
    delete mysql_dao;
}

TEST_CASE("SignIn") {
    // Init MySQL connection
    auto *mysql_dao = new MySQL_DAO("127.0.0.1", 3306,
                                    "email_admin", "email_admin_passwd", "email_system");
    REQUIRE(mysql_dao->Connect() == true);
    // Sign up a test account
    SignUpStatus sign_up_status;
    sign_up_status = mysql_dao->SignUp("127.0.0.1", "sign_in", "catch.org", "passwd",
                                       "unit-test", "Catch2 unit test example", "Question", "Answer");
    REQUIRE((sign_up_status == SIGN_UP_SUCCESS || sign_up_status == SIGN_UP_ACCOUNT_CONFLICT));
    // Sign in
    SignInFeedback *feedback = mysql_dao->SignIn("127.0.0.1", "sign_in@catch.org", "passwd");
    SignInStatus sign_in_status = feedback->status;
    REQUIRE(feedback->status == SIGN_IN_SUCCESS);
    delete feedback;
    feedback = mysql_dao->SignIn("127.0.0.1", "sign_in@catch.org", "password");
    REQUIRE(feedback->status == SIGN_IN_INVALID_PASSWD);
    delete feedback;
    feedback = mysql_dao->SignIn("127.0.0.1", "sign_in_wrong@catch.org", "passwd");
    REQUIRE(feedback->status == SIGN_IN_INVALID_ACCOUNT);
    delete feedback;
    delete mysql_dao;
}

TEST_CASE("Recover") {
    auto *mysql_dao = new MySQL_DAO("127.0.0.1", 3306,
                                    "email_admin", "email_admin_passwd", "email_system");
    REQUIRE(mysql_dao->Connect() == true);
    SignUpStatus sign_up_status;
    sign_up_status = mysql_dao->SignUp("127.0.0.1", "recover", "catch.org", "passwd",
                                       "unit-test", "Catch2 unit test example", "Question", "Answer");
    REQUIRE((sign_up_status == SIGN_UP_SUCCESS || sign_up_status == SIGN_UP_ACCOUNT_CONFLICT));
    RecoverFeedback *feedback = mysql_dao->GetRecoverQuestion("127.0.0.1", "recover@catch.org");
    REQUIRE(feedback->status == EXPECTED_SUCCESS);
    REQUIRE(strcmp(feedback->question, "Question") == 0);
    RecoverStatus status = mysql_dao->Recover("127.0.0.1", "null", "recover@catch.org", "Answer", "passwd");
    REQUIRE(status == RECOVER_SUCCESS);
    delete mysql_dao;
}

TEST_CASE("Delete") {
    auto *mysql_dao = new MySQL_DAO("127.0.0.1", 3306,
                                    "email_admin", "email_admin_passwd", "email_system");
    REQUIRE(mysql_dao->Connect() == true);
    SignUpStatus sign_up_status;
    sign_up_status = mysql_dao->SignUp("127.0.0.1", "delete", "catch.org", "passwd",
                                       "unit-test", "Catch2 unit test example", "Question", "Answer");
    REQUIRE((sign_up_status == SIGN_UP_SUCCESS || sign_up_status == SIGN_UP_ACCOUNT_CONFLICT));
    Status status = mysql_dao->Delete("127.0.0.1", "null", "delete@catch.org");
    REQUIRE(status == EXPECTED_SUCCESS);
    delete mysql_dao;
}

TEST_CASE("Email") {
    auto *mysql_dao = new MySQL_DAO("127.0.0.1", 3306,
                                    "email_admin", "email_admin_passwd", "email_system");
    REQUIRE(mysql_dao->Connect() == true);
    SignUpStatus sign_up_status;
    sign_up_status = mysql_dao->SignUp("127.0.0.1", "sender", "catch.org", "passwd",
                                       "unit-test", "Catch2 unit test example", "Question", "Answer");
    REQUIRE((sign_up_status == SIGN_UP_SUCCESS || sign_up_status == SIGN_UP_ACCOUNT_CONFLICT));
    sign_up_status = mysql_dao->SignUp("127.0.0.1", "recipient", "catch.org", "passwd",
                                       "unit-test", "Catch2 unit test example", "Question", "Answer");
    REQUIRE((sign_up_status == SIGN_UP_SUCCESS || sign_up_status == SIGN_UP_ACCOUNT_CONFLICT));
    Email email{};
    email.sender = new char[30];
    email.recipient = new char[30];
    email.title = new char[100];
    email.body = new char[100];
    sprintf(email.sender, "sender@catch.org");
    sprintf(email.recipient, "recipient@catch.org");
    sprintf(email.title, "Test");
    sprintf(email.body, "Just a test email.");
    Status status = mysql_dao->SendEmail("127.0.0.1", "null", "sender@catch.org", &email);
    REQUIRE(status == EXPECTED_SUCCESS);
    EmailFeedback *feedback = mysql_dao->FetchEmail("127.0.0.1", "null",
                                                    "sender@catch.org", EmailType::OUT);
    REQUIRE(feedback->status == EXPECTED_SUCCESS);
    REQUIRE(feedback->email_num > 0);
    delete feedback;
    feedback = mysql_dao->FetchEmail("127.0.0.1", "null",
                                     "recipient@catch.org", EmailType::IN);
    REQUIRE(feedback->status == EXPECTED_SUCCESS);
    REQUIRE(feedback->email_num > 0);
    delete feedback;
}

TEST_CASE("Draft") {
    auto *mysql_dao = new MySQL_DAO("127.0.0.1", 3306,
                                    "email_admin", "email_admin_passwd", "email_system");
    REQUIRE(mysql_dao->Connect() == true);
    SignUpStatus sign_up_status;
    sign_up_status = mysql_dao->SignUp("127.0.0.1", "sender", "catch.org", "passwd",
                                       "unit-test", "Catch2 unit test example", "Question", "Answer");
    REQUIRE((sign_up_status == SIGN_UP_SUCCESS || sign_up_status == SIGN_UP_ACCOUNT_CONFLICT));
    sign_up_status = mysql_dao->SignUp("127.0.0.1", "recipient", "catch.org", "passwd",
                                       "unit-test", "Catch2 unit test example", "Question", "Answer");
    REQUIRE((sign_up_status == SIGN_UP_SUCCESS || sign_up_status == SIGN_UP_ACCOUNT_CONFLICT));
    Email draft{};
    draft.sender = new char[30];
    draft.recipient = new char[30];
    draft.title = new char[100];
    draft.body = new char[100];
    sprintf(draft.sender, "sender@catch.org");
    sprintf(draft.recipient, "recipient@catch.org");
    sprintf(draft.title, "Test");
    sprintf(draft.body, "Just a test draft.");
    Status status = mysql_dao->SaveDraft("127.0.0.1", "null", "sender@catch.org", &draft);
    REQUIRE(status == EXPECTED_SUCCESS);
    strcpy(draft.recipient, "");
    status = mysql_dao->SaveDraft("127.0.0.1", "null", "sender@catch.org", &draft);
    REQUIRE(status == EXPECTED_SUCCESS);
    EmailFeedback *feedback = mysql_dao->FetchDraft("127.0.0.1", "null", "sender@catch.org");
    REQUIRE(feedback->status == EXPECTED_SUCCESS);
    REQUIRE(feedback->email_num > 0);
    delete feedback;
    delete mysql_dao;
}

TEST_CASE("Contact") {
    auto *mysql_dao = new MySQL_DAO("127.0.0.1", 3306,
                                    "email_admin", "email_admin_passwd", "email_system");
    REQUIRE(mysql_dao->Connect() == true);
    SignUpStatus sign_up_status;
    sign_up_status = mysql_dao->SignUp("127.0.0.1", "owner", "catch.org", "passwd",
                                       "unit-test", "Catch2 unit test example", "Question", "Answer");
    REQUIRE((sign_up_status == SIGN_UP_SUCCESS || sign_up_status == SIGN_UP_ACCOUNT_CONFLICT));
    sign_up_status = mysql_dao->SignUp("127.0.0.1", "contact", "catch.org", "passwd",
                                       "unit-test", "Catch2 unit test example", "Question", "Answer");
    REQUIRE((sign_up_status == SIGN_UP_SUCCESS || sign_up_status == SIGN_UP_ACCOUNT_CONFLICT));
    Contact contact{};
    contact.contact_name = new char[strlen("contact@catch.org")];
    strcpy(contact.contact_name, "contact@catch.org");
    contact.alias = new char[strlen("Contact")];
    strcpy(contact.alias, "Contact");
    Status status = mysql_dao->SetContact("127.0.0.1", "null", "owner@catch.org", &contact);
    REQUIRE(status == EXPECTED_SUCCESS);
    ContactFeedback *feedback = mysql_dao->GetContact("127.0.0.1", "null", "owner@catch.org");
    REQUIRE(feedback->status == EXPECTED_SUCCESS);
    REQUIRE(feedback->contact_num > 0);
    printf("[console] Get contact, total num is: %d\n", feedback->contact_num);
    for (int i = 0; i < feedback->contact_num; i++)
        printf("[console] Contact name: %s, alias: %s\n", feedback->contact[i]->contact_name,
               feedback->contact[i]->alias);
    delete feedback;
    delete mysql_dao;
}