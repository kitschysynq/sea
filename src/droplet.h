#ifndef _SEA_DROPLET_H
#define _SEA_DROPLET_H
#include <time.h>

struct droplet
{
	int id;
	char * name;
	long memory;
	int vcpus;
	int disk;
	int locked;
	time_t created_at; // should this just be a char * to match the json string?
	char * status;
	int * backup_ids;
	int * snapshot_ids;
	char ** features;
	// struct region region;
	// struct image * image;
	// struct size * size;
	char * size_slug;
	// struct network * networks;
	// struct kernel * kernel;
	// struct backup_window * next_backup_window;
	char ** tags;
	int * volume_ids;
};

struct droplet *droplet_new(void);
void droplet_free(struct droplet *d);
int droplet_get_by_id(struct droplet * d, int id);
#endif // _SEA_DROPLET_H
