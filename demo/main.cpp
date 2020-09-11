#include "leadao.h"

int main() {
    auto *mySqlDao = new MySQL_DAO("127.0.0.1", 3306,
                                   "email_admin", "email_admin_passwd", "email_system");
    mySqlDao->Connect();
    mySqlDao->SignIn("aaa@bbb", "ccc");
    mySqlDao->SignUp("aaa", "bbb", "ccc", "ddd", "eee",
                     "fff", "ggg");

    delete mySqlDao;

    return 0;
}