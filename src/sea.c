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
#include "droplet.h"

int main(int argc, char *argv[])
{
	if(argc < 2)
	{
		fprintf(stderr, "usage: %s account\n\n", argv[0]);
		fprintf(stderr, "Show details of the current account.\n\n");
		return 2;
	}

	if(strncmp(argv[1], "account", ARG_SIZE) == 0) {
		struct account *a = account_new();
		if(a == NULL)
		{
			account_free(a);
			perror("account: ");
			return EXIT_FAILURE;
		}

		if(!account_get(a))
			return EXIT_FAILURE;

		printf("Email: %s (verified: %s)\nStatus: %s\nStatus Message: %s\nUUID: %s\nDroplet Limit: %d\nFloating IP Limit: %d\n",
				a->email,
				(a->email_verified) ? "true" : "false",
				a->status,
				a->status_message,
				a->uuid,
				a->droplet_limit,
				a->floating_ip_limit);

		account_free(a);
	} else if(strncmp(argv[1], "droplet", ARG_SIZE) == 0) {
		struct droplet *d = droplet_new();
		if(d == NULL)
		{
			droplet_free(d);
			perror("droplet: ");
			return EXIT_FAILURE;
		}

		int id = atoi(argv[2]);
		if(!droplet_get_by_id(d, id))
			return EXIT_FAILURE;

		printf("ID: %d\nName: %s\nMemory: %ldM\nVCPUs: %d\nDisk: %dG\nLocked: %s\n",
				d->id,
				d->name,
				d->memory,
				d->vcpus,
				d->disk,
				(d->locked) ? "true" : "false"
				);

		droplet_free(d);
	} else if(strncmp(argv[1], "ssh", ARG_SIZE) == 0) {
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
	}

	return EXIT_SUCCESS;
}
