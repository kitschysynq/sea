#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <jansson.h>

#include "sea.h"
#include "droplet.h"
#include "request.h"

struct droplet *droplet_new()
{
	struct droplet *d = NULL;
	d = calloc(1, sizeof(struct droplet));

	return d;
}

void droplet_free(struct droplet *d)
{
	free(d->name);
	free(d);
	d = NULL;
}

int droplet_get_by_id(struct droplet *d, int id)
{
	if(d == NULL)
		return 0;

	int ret = 1;
	char url[URL_SIZE];
	char *endpoint = "%s/droplets/%d";
	char *response = NULL;

	snprintf(url, URL_SIZE, endpoint, URL_BASE, id);

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

	json_t *data,
	       *droplet_id,
	       *name,
	       *memory,
	       *vcpus,
	       *disk,
	       *locked;
//	       *created_at,
//	       *status,
//	       *backup_id,
//	       *snapshot_ids,
//	       *features,
//	       *size_slug,
//	       *tags,
//	       *volume_ids;

	data = json_object_get(root, "droplet");
	if(!json_is_object(data))
	{
		fprintf(stderr, "error: account data is not an object\n");
		ret = 0;
		goto finish;
	}

	droplet_id = json_object_get(data, "id");
	if(!json_is_integer(droplet_id))
	{
		fprintf(stderr, "error: id is not a number\n");
		ret = 0;
		goto finish;
	}

	name = json_object_get(data, "name");
	if(!json_is_string(name))
	{
		fprintf(stderr, "error: name is not an string\n");
		ret = 0;
		goto finish;
	}

	memory = json_object_get(data, "memory");
	if(!json_is_integer(memory))
	{
		fprintf(stderr, "error: memory is not a number\n");
		ret = 0;
		goto finish;
	}

	vcpus = json_object_get(data, "vcpus");
	if(!json_is_integer(vcpus))
	{
		fprintf(stderr, "error: vcpus is not an integer\n");
		ret = 0;
		goto finish;
	}

	disk = json_object_get(data, "disk");
	if(!json_is_integer(disk))
	{
		fprintf(stderr, "error: disk is not a number\n");
		ret = 0;
		goto finish;
	}

	locked = json_object_get(data, "locked");
	if(!json_is_boolean(locked))
	{
		fprintf(stderr, "error: locked is not a boolean\n");
		ret = 0;
		goto finish;
	}

	size_t len = 256;
	d->name = malloc(sizeof(char) * len);

	strncpy(d->name, json_string_value(name), len);
	d->id = (int)json_integer_value(droplet_id);
	d->memory = (int)json_integer_value(memory);
	d->vcpus = (int)json_integer_value(vcpus);
	d->disk = (int)json_integer_value(disk);
	d->locked = (int)json_boolean_value(locked);

finish:
	json_decref(root);
	return ret;
}
