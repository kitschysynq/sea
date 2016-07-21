/*
 * Copyright (c) 2016 Sean Swehla <sean@sweh.la>
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

#include "sea.h"
#include "request.h"
#include "account.h"

int main(int argc, char *argv[])
{
	if(argc != 2)
	{
		fprintf(stderr, "usage: %s account\n\n", argv[0]);
		fprintf(stderr, "Show details of the current account.\n\n");
		return 2;
	}

	struct account *a = account_new();
	if(a == NULL)
	{
		account_free(a);
		perror("account: ");
		return EXIT_FAILURE;
	}

	if(!account_get(a)) {
		account_free(a);
		return EXIT_FAILURE;
	}

	printf("Email: %s (verified: %s)\nStatus: %s\nStatus Message: %s\nUUID: %s\nDroplet Limit: %d\nFloating IP Limit: %d\n",
			a->email,
			(a->email_verified) ? "true" : "false",
			a->status,
			a->status_message,
			a->uuid,
			a->droplet_limit,
			a->floating_ip_limit);
	account_free(a);

	struct ssh_keys *keys = NULL;
	keys = account_ssh_keys();

	printf("SSH Keys (%d):\n", keys->count);
	for(int i = 0; i < keys->count; i++) {
		printf("\tID: %d\n\tName: %s\n\tFingerprint: %s\n\tPublic Key: %s\n",
				keys->keys[i]->id,
				keys->keys[i]->name,
				keys->keys[i]->fingerprint,
				keys->keys[i]->public_key);
	}
	ssh_keys_free(keys);

	return EXIT_SUCCESS;
}
