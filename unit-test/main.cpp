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
    sign_up_status = mysql_dao->SignUp("127.0.0.1", "aaa", "bbb", "ccc",
                                      "ddd", "eee", "fff", "ggg");
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
    sign_up_status = mysql_dao->SignUp("127.0.0.1", "aaa", "bbb", "ccc",
                                      "ddd", "eee", "fff", "ggg");
    REQUIRE((sign_up_status == SIGN_UP_SUCCESS || sign_up_status == SIGN_UP_ACCOUNT_CONFLICT));
    // Sign in
    SignInFeedback *feedback = mysql_dao->SignIn("127.0.0.1", "aaa@bbb", "ccc");
    SignInStatus sign_in_status = feedback->status;
    REQUIRE(feedback->status == SIGN_IN_SUCCESS);
    delete feedback;
    feedback = mysql_dao->SignIn("127.0.0.1", "aaa@bbb", "ddd");
    REQUIRE(feedback->status == SIGN_IN_INVALID_PASSWD);
    delete feedback;
    feedback = mysql_dao->SignIn("127.0.0.1", "aaa@ccc", "ccc");
    REQUIRE(feedback->status == SIGN_IN_INVALID_ACCOUNT);
    delete feedback;
    delete mysql_dao;
}
