# Create Database
drop database if exists email_system;
create database email_system;
use email_system;

# Create user and set privileges
drop user if exists 'email_admin'@'%';
create user 'email_admin'@'%' identified by 'email_admin_passwd';
grant all on email_system.* to 'email_admin'@'%';

# Create tables
# Create account info table, which contains basic info of every account
drop table if exists email_system.account_info;
create table account_info
(
    # Account Hash ID
    id                char(16)     not null,
    # A typical full account consists of `host` and `domain`, e.g. 'my_account@outlook.com'
    # Account host name, e.g. 'my_account'
    host              varchar(30)  not null,
    # Account domain name, e.g. '@outlook.com'
    domain            varchar(30)  not null,
    # Account password
    password          varchar(30)  not null,
    # Account nickname
    nickname          varchar(30)           default '',
    # Account description
    description       varchar(100)          default '',
    # Recovery function used to reset account password in case that user forgets his password
    # Recovery question
    recovery_question varchar(100) not null,
    # Recovery answer
    recovery_answer   varchar(100) not null,
    # Lock status, determine whether this account is available:
    #     0: NLOCKED
    #     1: LOCKED
    locked            int          not null default 0,
    primary key (id)
) charset = UTF8MB4;
# Create account activity table, which contains basic info of every activity created by every account
drop table if exists account_activity;
create table account_activity
(
    # Activity Hash ID
    id           char(16)    not null,
    # Activity creator's account name
    account_name varchar(70) not null,
    # Client IP
    ip           varchar(15) not null,
    # Activity type, currently include:
    #     0: Sign up
    #     1: Sign in
    #     2: Sing out
    #     3: Recover account
    #     4：Delete account
    action       int         not null default -1,
    # Activity occurring time
    time         timestamp   not null,
    # Activity status code, currently include:
    #     0: EXPECTED_SUCCESS
    #     1: UNEXPECTED_SUCCESS
    #     2: EXPECTED_ERROR
    #     3: UNEXPECTED_ERROR
    status       int         not null default 0,
    primary key (id)
) charset = UTF8MB4;
# Create account token table, which contains token of every sign-in action created by every account
drop table if exists account_token;
create table account_token
(
    # Account sign in token
    token       char(16)    not null,
    # Account ID
    account_id  varchar(16) not null,
    # Client IP
    ip          varchar(15) not null,
    # Token create time
    create_time timestamp   not null,
    # Token expire time
    expire_time timestamp   not null,
    # Token status code, currently include:
    #     0: OPEN
    #    -1: CLOSED
    status      int         not null default 0,
    primary key (token),
    foreign key (account_id) references account_info (id) on delete cascade on update cascade
) charset = UTF8MB4;
# Create email accessory table
drop table if exists accessory;
create table accessory
(
    # Accessory Hash ID
    id char(16) not null,
    # TODO: add accessory column
    # some accessory columns...
    primary key (id)
) charset = UTF8MB4;
# Create email details table
drop table if exists email;
create table email
(
    # Email Hash ID
    id                char(16)     not null,
    # Sender ID
    sender_id         char(16)     not null,
    # Recipient ID
    recipient_id      char(16)     not null,
    # Email send time
    time              timestamp    not null,
    # Email title
    title             varchar(100) not null default '默认标题',
    # Email body
    body              varchar(10000)        default '',
    # Email accessory number
    accessory_num     int                   default 0,
    # Email accessory ID list
    accessory_id_list char(160)             default '',
    # Email receiving status, determining whether the email is delivered successfully, include:
    #     0: UNRECEIVED
    #     1: RECEIVED
    status            int          not null,
    primary key (id),
    foreign key (sender_id) references account_info (id) on delete cascade on update cascade,
    foreign key (recipient_id) references account_info (id) on delete cascade on update cascade
) charset = UTF8MB4;
# Create draft details table
drop table if exists draft;
create table draft
(
    # Email Hash ID
    id                char(16)     not null,
    # Sender Name
    sender_id         char(16)     not null,
    # Recipient Name
    recipient_name    char(100)             default '',
    # Draft save time
    time              timestamp    not null,
    # Draft title
    title             varchar(100) not null default '默认标题',
    # Draft body
    body              varchar(10000)        default '',
    # Email accessory number
    accessory_num     int                   default 0,
    # Email accessory ID list
    accessory_id_list char(160)             default '',
    primary key (id),
    foreign key (sender_id) references account_info (id) on delete cascade on update cascade
) charset = UTF8MB4;
# Create contact tables
drop table if exists contact;
create table contact
(
    owner_id   char(16) not null,
    contact_id char(16) not null,
    alias      varchar(30) default '',
    primary key (owner_id),
    foreign key (owner_id) references account_info (id) on delete cascade on update cascade,
    foreign key (contact_id) references account_info (id) on delete cascade on update cascade
) charset = UTF8MB4;
