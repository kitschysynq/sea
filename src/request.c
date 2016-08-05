#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <curl/curl.h>

#include "request.h"

char *
auth_header(char *token)
{
	const char *fmt = "Authorization: Bearer %s";
	/* (fmt - '%s') + (token + \0) */
	size_t len = (strlen(fmt) - 2) + strlen(token) + 1;
	char *header = malloc(sizeof(char) * len);
	if(!header) {
		perror("malloc");
		return NULL;
	}

	snprintf(header, len, fmt, token);
	return header;
}

static size_t
write_response(void *ptr, size_t size, size_t nmemb, void *stream)
{
	size_t realsize = size * nmemb;
	struct write_result *result = (struct write_result *)stream;

	result->data = realloc(result->data, result->size + realsize + 1);
	if(!result->data) {
		perror("realloc");
		return 0;
	}

	memcpy(&(result->data[result->size]), ptr, realsize);
	result->size += realsize;
	result->data[result->size] = 0;

	return realsize;
}

char *request(const char *url)
{
	CURL *curl = NULL;
	CURLcode status;
	struct curl_slist *headers = NULL;
	struct write_result result;
	long code;
	char *auth = NULL;

	curl_global_init(CURL_GLOBAL_ALL);
	curl = curl_easy_init();
	if(!curl)
		goto error;

	result.size = 0;
	result.data = malloc(1);
	if(!result.data) {
		perror("malloc");
		goto error;
	}

	auth = auth_header(getenv("DIGITALOCEAN_API_TOKEN"));
	if(!auth)
		goto error;

	curl_easy_setopt(curl, CURLOPT_URL, url);

	headers = curl_slist_append(headers, "User-Agent: sea/0.1");
	headers = curl_slist_append(headers, "Content-Type: application/json");
	headers = curl_slist_append(headers, auth);

	curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
	curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_response);
	curl_easy_setopt(curl, CURLOPT_WRITEDATA, &result);

	if((status = curl_easy_perform(curl)) != CURLE_OK) {
		fprintf(stderr, "error: unable to request data from %s:\n", url);
		fprintf(stderr, "%s\n", curl_easy_strerror(status));
		goto error;
	}

	if((status = curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &code)) != CURLE_OK) {
		fprintf(stderr, "%s\n", curl_easy_strerror(status));
		goto error;
	}

	if(code >= 400) {
		fprintf(stderr, "error: server responded with code %ld\n", code);
		goto error;
	}

	curl_easy_cleanup(curl);
	curl_slist_free_all(headers);
	curl_global_cleanup();

	/* zero-terminate the result */
	result.data[result.size] = '\0';

	return result.data;

error:
	if(result.data)
		free(result.data);
	if(curl)
		curl_easy_cleanup(curl);
	if(headers)
		curl_slist_free_all(headers);
	if(auth)
		free(auth);
	curl_global_cleanup();
	return NULL;
}
