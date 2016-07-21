#pragma once
struct write_result
{
	char *data;
	int pos;
};

size_t write_response(void *ptr, size_t size, size_t nmemb, void *stream);

char *request(const char *url);
