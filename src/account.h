#ifndef _SEA_ACCOUNT_H
#define _SEA_ACCOUNT_H

struct account
{
	int droplet_limit;
	int floating_ip_limit;
	int email_verified;
	char *email;
	char *uuid;
	char *status;
	char *status_message;
};

struct account *account_new(void);
void account_free(struct account *a);
int account_get(struct account *a);

#endif /* _SEA_ACCOUNT_H */
