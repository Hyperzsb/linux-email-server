#include "lesdao.h"

int main() {
    auto *mySqlDao = new MySQL_DAO("172.16.151.128", 3306,
                                        "email_admin", "email_admin_passwd", "email_system");
    mySqlDao->connect();
}
