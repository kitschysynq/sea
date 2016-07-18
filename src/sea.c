/*
 * Copyright (c) 2016 Sean Swehla <sean@sweh.la>
 */

#include <stdlib.h>
#include <string.h>
#include "sea.h"

#include <jansson.h>
#include <curl/curl.h>

#define URL_FORMAT   "https://api.digitalocean.com/v2/account"
#define URL_SIZE     256

int main(int argc, char *argv[])
{
	char *text;
	char url[URL_SIZE];
	int ret;

	json_t *root;
	json_error_t error;

	if(argc != 2)
	{
		fprintf(stderr, "usage: %s account\n\n", argv[0]);
		fprintf(stderr, "Show details of the current account.\n\n");
		return 2;
	}

	ret = 0;

	//snprintf(url, URL_SIZE, URL_FORMAT, argv[1], argv[2]);
	snprintf(url, URL_SIZE, URL_FORMAT);

	text = request(url);
	if(!text)
		return 1;

	root = json_loads(text, 0, &error);
	free(text);

	if(!root)
	{
		fprintf(stderr, "error: on line %d: %s\n", error.line, error.text);
		return 1;
	}

	if(!json_is_object(root))
	{
		fprintf(stderr, "error: root is not an object\n");
		ret = 1;
		goto finish;
	}

	json_t *data, *email, *status, *droplet_limit;

	data = json_object_get(root, "account");
	if(!json_is_object(data))
	{
		fprintf(stderr, "error: account data is not an object\n");
		ret = 1;
		goto finish;
	}

	email = json_object_get(data, "email");
	if(!json_is_string(email))
	{
		fprintf(stderr, "error: email is not a string\n");
		ret = 1;
		goto finish;
	}

	status = json_object_get(data, "status");
	if(!json_is_string(status))
	{
		fprintf(stderr, "error: status is not an string\n");
		ret = 1;
		goto finish;
	}

	droplet_limit = json_object_get(data, "droplet_limit");
	if(!json_is_integer(droplet_limit))
	{
		fprintf(stderr, "error: droplet_limit is not a number\n");
		ret = 1;
		goto finish;
	}

	printf("Email: %s\nStatus: %s\nDroplet Limit: %d\n",
			json_string_value(email),
			json_string_value(status),
			(int)json_integer_value(droplet_limit));

finish:
	json_decref(root);
	return ret;
}
