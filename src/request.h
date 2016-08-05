#ifndef _SEA_REQUEST_H
#define _SEA_REQUEST_H

struct write_result
{
	char *data;
	size_t size;
};

char *auth_header(char *token);
char *request(const char *url);

#endif /* _SEA_REQUEST_H */
