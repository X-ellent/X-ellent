// ip_user.h
#ifndef IP_USER_H
#define IP_USER_H

#include <arpa/inet.h>

#define MAX_USERS 100

struct ip_user {
    char ip[INET_ADDRSTRLEN];
    char username[32];
};

extern struct ip_user ip_users[MAX_USERS];
extern int num_ip_users;

void load_ip_users(void);

#endif // IP_USER_H
