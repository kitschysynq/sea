#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <jansson.h>

#include "sea.h"
#include "account.h"
#include "request.h"

struct account *account_new()
{
	struct account *a = NULL;
	a = calloc(1, sizeof(struct account));

	return a;
}

void account_free(struct account *a)
{
	free(a->email);
	free(a->status);
	free(a->uuid);
	free(a->status_message);
	free(a);
	a = NULL;
}

int account_get(struct account *a)
{
	if(a == NULL)
		return 0;

	int ret = 1;
	char url[URL_SIZE];
	char *endpoint = "/account";
	char *response = NULL;

	strncpy(url, URL_BASE, URL_SIZE - strlen(endpoint));
	strncat(url, endpoint, strlen(endpoint));

	response = request(url);
	if(!response)
		return 0;

	json_t *root;
	json_error_t error;
	root = json_loads(response, 0, &error);
	free(response);
	response = NULL;

	if(!root)
	{
		fprintf(stderr, "error: on line %d: %s\n", error.line, error.text);
		return 0;
	}

	if(!json_is_object(root))
	{
		fprintf(stderr, "error: root is not an object\n");
		ret = 0;
		goto finish;
	}

	json_t *data, *email, *email_verified, *status, *status_message, *uuid, *droplet_limit, *floating_ip_limit;

	data = json_object_get(root, "account");
	if(!json_is_object(data))
	{
		fprintf(stderr, "error: account data is not an object\n");
		ret = 0;
		goto finish;
	}

	email = json_object_get(data, "email");
	if(!json_is_string(email))
	{
		fprintf(stderr, "error: email is not a string\n");
		ret = 0;
		goto finish;
	}

	status = json_object_get(data, "status");
	if(!json_is_string(status))
	{
		fprintf(stderr, "error: status is not an string\n");
		ret = 0;
		goto finish;
	}

	status_message = json_object_get(data, "status_message");
	if(!json_is_string(status))
	{
		fprintf(stderr, "error: status_message is not an string\n");
		ret = 0;
		goto finish;
	}

	uuid = json_object_get(data, "uuid");
	if(!json_is_string(uuid))
	{
		fprintf(stderr, "error: uuid is not an string\n");
		ret = 0;
		goto finish;
	}

	email_verified = json_object_get(data, "email_verified");
	if(!json_is_boolean(email_verified))
	{
		fprintf(stderr, "error: email_verified is not a boolean\n");
		ret = 0;
		goto finish;
	}

	droplet_limit = json_object_get(data, "droplet_limit");
	if(!json_is_integer(droplet_limit))
	{
		fprintf(stderr, "error: droplet_limit is not a number\n");
		ret = 0;
		goto finish;
	}

	floating_ip_limit = json_object_get(data, "floating_ip_limit");
	if(!json_is_integer(floating_ip_limit))
	{
		fprintf(stderr, "error: floating_ip_limit is not a number\n");
		ret = 0;
		goto finish;
	}

	size_t len = 256;
	a->email = malloc(sizeof(char) * len);
	a->status = malloc(sizeof(char) * len);
	a->status_message = malloc(sizeof(char) * len);
	a->uuid = malloc(sizeof(char) * len);

	strncpy(a->email, json_string_value(email), len);
	strncpy(a->status, json_string_value(status), len);
	strncpy(a->status_message, json_string_value(status_message), len);
	strncpy(a->uuid, json_string_value(uuid), len);
	a->email_verified = (int)json_boolean_value(email_verified);
	a->droplet_limit = (int)json_integer_value(droplet_limit);
	a->floating_ip_limit = (int)json_integer_value(floating_ip_limit);

finish:
	json_decref(root);
	return ret;
}
