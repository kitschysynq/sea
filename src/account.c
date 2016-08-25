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

struct ssh_key *
_ssh_keys_next(struct ssh_keys *keys)
{
	json_t *container;
	container = json_object_get(keys->data, "ssh_keys");
	if(!json_is_array(container)) {
		fprintf(stderr, "error: ssh_keys is not an array\n");
		return NULL;
	}

	struct ssh_key *key = NULL;
	json_t *elem, *id, *name, *fingerprint, *public_key;

	elem = json_array_get(container, keys->pos);
	if(!json_is_object(elem)) {
		goto done;
	}

	id = json_object_get(elem, "id");
	if(!json_is_integer(id)) {
		fprintf(stderr, "error: ssh key name is not a string\n");
		goto done;
	}

	name = json_object_get(elem, "name");
	if(!json_is_string(name)) {
		fprintf(stderr, "error: ssh key name is not a string\n");
		goto done;
	}

	fingerprint = json_object_get(elem, "fingerprint");
	if(!json_is_string(name)) {
		fprintf(stderr, "error: ssh key fingerprint is not a string\n");
		goto done;
	}

	public_key = json_object_get(elem, "public_key");
	if(!json_is_string(name)) {
		fprintf(stderr, "error: ssh public_key is not a string\n");
		goto done;
	}

	key = malloc(sizeof(struct ssh_key));
	const char *tmp_n  = json_string_value(name);
	const char *tmp_fp = json_string_value(fingerprint);
	const char *tmp_pk = json_string_value(public_key);

	key->id = (int)json_integer_value(id);
	key->name = malloc(sizeof(char) * strlen(tmp_n) + 1);
	key->fingerprint = malloc(sizeof(char) * strlen(tmp_fp) + 1);
	key->public_key = malloc(sizeof(char) * strlen(tmp_pk) + 1);

	strncpy(key->name, tmp_n, strlen(tmp_n) + 1);
	strncpy(key->fingerprint, tmp_fp, strlen(tmp_fp) + 1);
	strncpy(key->public_key, tmp_pk, strlen(tmp_pk) + 1);

	keys->pos++;

done:
	return key;
}

// account_ssh_keys retrieves a users ssh keys.
//
// On success, a list of ssh keys is returned.
// It is the responsibility of the caller to free each key with
// a call to account_ssh_key_free().
//
// On failure NULL is returned.
struct ssh_keys *
account_ssh_keys()
{
	struct ssh_keys *keys = NULL;
	char url[URL_SIZE];
	char *endpoint = "/account/keys";
	char *response = NULL;

	strncpy(url, URL_BASE, URL_SIZE - strlen(endpoint));
	strncat(url, endpoint, strlen(endpoint));

	response = request(url);
	if(!response)
		return NULL;

	keys = malloc(sizeof(struct ssh_keys));
	if(keys == NULL)
		goto error;

	json_error_t error;
	keys->data = json_loads(response, 0, &error);
	free(response);
	response = NULL;

	if(!keys->data) {
		fprintf(stderr, "json error: on line %d: %s\n", error.line, error.text);
		goto error;
	}

	if(!json_is_object(keys->data)) {
		fprintf(stderr, "json error: root is not an object\n");
		goto error;
	}

	json_t *container;
	container = json_object_get(keys->data, "ssh_keys");
	if(!json_is_array(container)) {
		fprintf(stderr, "json error: 'ssh_keys' is not an array\n");
		goto error;
	}

	keys->pos = 0;
	keys->count = json_array_size(container);
	keys->next = _ssh_keys_next;

	return keys;
error:
	ssh_keys_free(keys);
	return NULL;
}

void ssh_key_free(struct ssh_key *key)
{
	free(key->name);
	free(key->fingerprint);
	free(key->public_key);
	key = NULL;
}

void ssh_keys_free(struct ssh_keys *keys)
{
	if(keys->data)
		json_decref(keys->data);
	if(keys)
		free(keys);
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
	a->email_verified = (int)json_is_true(email_verified);
	a->droplet_limit = (int)json_integer_value(droplet_limit);
	a->floating_ip_limit = (int)json_integer_value(floating_ip_limit);

finish:
	json_decref(root);
	return ret;
}
