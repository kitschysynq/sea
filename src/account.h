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

struct ssh_keys
{
	int count;
	struct ssh_key **keys;
};

struct ssh_key
{
	int id;
	char *name;
	char *fingerprint;
	char *public_key;
};

struct account *account_new(void);
void account_free(struct account *a);
int account_get(struct account *a);
struct ssh_keys *account_ssh_keys(void);

struct ssh_key *ssh_key_new(void);
void ssh_key_free(struct ssh_key *key);
void ssh_keys_free(struct ssh_keys *keys);

#endif /* _SEA_ACCOUNT_H */
