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

	return EXIT_SUCCESS;
}
