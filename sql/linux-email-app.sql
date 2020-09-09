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
drop table if exists account_info;
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
    # Recovery function used to reset account password in case that user forgets his password
    # Recovery question
    recovery_question varchar(100) not null,
    # Recovery answer
    recovery_answer   varchar(100) not null,
    # Account nickname
    nickname          varchar(30)           default null,
    # Account description
    description       varchar(100)          default null,
    # Lock status, determine whether this account is available:
    #     0: NLOCKED
    #     1: LOCKED
    locked            int(1)       not null default 0,
    primary key (id)
) charset = utf8
  collate = utf8_general_ci;
# Create account activity table, which contains basic info of every activity created by every account
drop table if exists account_activity;
create table account_activity
(
    # Activity Hash ID
    id         char(16)    not null,
    # Activity creator's account ID
    account_id varchar(16) not null,
    # Client IP
    ip         varchar(15) not null,
    # Client Port
    port       int(5)      not null,
    # Activity type, currently include:
    #     0: Sign in
    #     1: Sign out
    #     2: Sing up
    #     3: Recover account
    #     4：Close account
    action     int(2)      not null default 0,
    # Activity occurring time
    time       timestamp   not null,
    # Activity status code, currently include:
    #     0: SUCCESS
    #    -1: UNEXPECTED_FAILURE
    status     int(3)      not null default 0,
    primary key (id),
    foreign key (account_id) references account_info (id)
) charset = utf8
  collate = utf8_general_ci;
# Create email details table
drop table if exists email;
create table email
(
    # Email Hash ID
    id           char(16)     not null,
    # Sender ID
    sender_id    char(16)     not null,
    # Recipient ID
    recipient_id char(16)     not null,
    # Email send time
    time         timestamp    not null,
    # Email title
    title        varchar(100) not null default '默认标题',
    # Email body
    body         varchar(10000)        default null,
    # Email receiving status, determining whether the email is delivered successfully, include:
    #     0: UNRECEIVED
    #     1: RECEIVED
    status       int(3)       not null,
    primary key (id),
    foreign key (sender_id) references account_info (id),
    foreign key (recipient_id) references account_info (id)
) charset = utf8
  collate = utf8_general_ci;
# Create draft details table
drop table if exists draft;
create table draft
(
    # Email Hash ID
    id           char(16)     not null,
    # Sender ID
    sender_id    char(16)     not null,
    # Recipient ID
    recipient_id char(16)              default null,
    # Draft save time
    time         timestamp    not null,
    # Draft title
    title        varchar(100) not null default '默认标题',
    # Draft body
    body         varchar(10000)        default null,
    primary key (id),
    foreign key (sender_id) references account_info (id),
    foreign key (recipient_id) references account_info (id)
) charset = utf8
  collate = utf8_general_ci;