// ip_user.h
#ifndef IP_USER_H
#define IP_USER_H

#define MAX_USERS 100
#define MAX_DISPLAY_LENGTH 256

struct ip_user {
    char ip[16];
    char username[32];
};

extern struct ip_user ip_users[MAX_USERS];
extern int num_ip_users;

void load_ip_users(void);

#endif // IP_USER_H
