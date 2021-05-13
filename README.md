# Linux Email Application Server

**An email application running on Linux for LAN environment.**

**This repository only contains server-side program of the whole Linux Email Application.**

*Course project for Basic Practice of Computer Science, BIT 2020.*

## Table of Contents

- [Overview](https://github.com/Hyperzsb/linux-email-server#overview)
- [Structure of Repository](https://github.com/Hyperzsb/linux-email-server#structure-of-repository)
  - [Branches](https://github.com/Hyperzsb/linux-email-server#branches)
  - [Contents in Master](https://github.com/Hyperzsb/linux-email-server#contents-in-master)
- [What's Next](https://github.com/Hyperzsb/linux-email-server#whats-next)
- [Call for Contribution](https://github.com/Hyperzsb/linux-email-server#call-for-contribution)
- [License](https://github.com/Hyperzsb/linux-email-server#license)

## Overview

This **server-side** program currently includes features as follows:

- Account Support
  - Sign up
  - Sign in
  - Recover account
  - Delete account 
  - Account activities log
- Email
  - Send & fetch emails
  - Save & load drafts
- Communication 
  - Save & load contacts

## Structure of Repository

### Branches

- master: Master branch for the project.
- echo: A simple demo for C Socket Programming.
- single-p2p: A simple P2P demo for C Socket communication program.

### Contents in Master

- [demo](https://github.com/Hyperzsb/linux-email-server/tree/master/demo): a simple demo for libeadao.
- [libeadao](https://github.com/Hyperzsb/linux-email-server/tree/master/libleadao): the MySQL Data Access Object (DAO) for this server. It implements basic database methods for login, logout, sending emails, fetching emails, and so forth, actions.
- [server](https://github.com/Hyperzsb/linux-email-server/tree/master/server): the main server program.
- [sql](https://github.com/Hyperzsb/linux-email-server/tree/master/sql): the SQL script for creating the necessary database for the server.
- [unit-test](https://github.com/Hyperzsb/linux-email-server/tree/master/unit-test): basic unit tests for this server using [Catch2](https://github.com/catchorg/Catch2)

## What's next

This **server-side** program is definitely not thorough due to time limitation.

There are several features needed to add as follows:

- Account
  - Profile image
  - Information modification
  - Status authentication
- Email
  - Accessory support
  - Carbon copy
  - Delete emails (Trash bin)

## Call for Contribution

As you know, the project assigned in course *Basic Practice of Computer Science* will generally repeat every two years. So, in order to help everyone learning in CS, BIT, this project needs more contributing and improving. If you are interested in this meaningful tasks, I won't be more grateful.

## License

This project is under [MIT License](https://github.com/Hyperzsb/linux-email-server/blob/master/LICENSE).