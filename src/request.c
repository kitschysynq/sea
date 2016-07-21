#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <curl/curl.h>

#include "request.h"

#define BUFFER_SIZE  (256 * 1024)  /* 256 KB */

void get_auth_token(char * token, int size) {
	snprintf(token, size, "Authorization: Bearer %s", getenv("DIGITALOCEAN_API_TOKEN"));
}

size_t write_response(void *ptr, size_t size, size_t nmemb, void *stream)
{
	struct write_result *result = (struct write_result *)stream;

	if(result->pos + size * nmemb >= BUFFER_SIZE - 1)
	{
		fprintf(stderr, "error: too small buffer\n");
		return 0;
	}

	memcpy(result->data + result->pos, ptr, size * nmemb);
	result->pos += size * nmemb;

	return size * nmemb;
}

char *request(const char *url)
{
	CURL *curl = NULL;
	CURLcode status;
	struct curl_slist *headers = NULL;
	char *data = NULL;
	long code;

	curl_global_init(CURL_GLOBAL_ALL);
	curl = curl_easy_init();
	if(!curl)
		goto error;

	data = malloc(BUFFER_SIZE);
	if(!data)
		goto error;

	struct write_result write_result = {
		.data = data,
		.pos = 0
	};

	curl_easy_setopt(curl, CURLOPT_URL, url);

	headers = curl_slist_append(headers, "User-Agent: sea/0.1");
	headers = curl_slist_append(headers, "Content-Type: application/json");

	size_t token_size = 255;
	char *token = (char *)calloc(sizeof(char), token_size);
	get_auth_token(token, token_size);
	headers = curl_slist_append(headers, token);
	curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);

	// headers = curl_slist_append(headers, sprintf("Authorization: Bearer %s", token));
	// curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);

	curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_response);
	curl_easy_setopt(curl, CURLOPT_WRITEDATA, &write_result);

	status = curl_easy_perform(curl);
	if(status != 0)
	{
		fprintf(stderr, "error: unable to request data from %s:\n", url);
		fprintf(stderr, "%s\n", curl_easy_strerror(status));
		goto error;
	}

	curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &code);
	if(code != 200)
	{
		fprintf(stderr, "error: server responded with code %ld\n", code);
		goto error;
	}

	curl_easy_cleanup(curl);
	curl_slist_free_all(headers);
	curl_global_cleanup();

	/* zero-terminate the result */
	data[write_result.pos] = '\0';

	return data;

error:
	if(data)
		free(data);
	if(curl)
		curl_easy_cleanup(curl);
	if(headers)
		curl_slist_free_all(headers);
	if(token)
		free(token);
	curl_global_cleanup();
	return NULL;
}
