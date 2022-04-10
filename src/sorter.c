#include <pthread.h>
#include <dirent.h>
#include <string.h>
#include <malloc.h>
#include <unistd.h>

#include <sorter.h>

static struct config *config_file;
static pthread_mutex_t config_lock;

// pthread_mutex_lock(&lock);

// the thread that will reparse the config file.
static void *refresh_config(void *arg)
{
	while (1)
	{
		sleep(config_file->c_options.o_parse_interval);
		pthread_mutex_lock(&config_lock);
		// reparse the config. Thread safe.
		reparse_config(config_file);
		pthread_mutex_unlock(&config_lock);
	}	
}

// the thread that will organize the files.
static void *organize_files(void *arg)
{
		
}



void start_sorter(struct config *src)
{
	config_file = src;
	// initalize the mutex.
	pthread_mutex_init(&config_lock, NULL);

	pthread_t refresh_t;
	pthread_t organize_t;
	// check if the threads has been created.
	if (
		pthread_create(&refresh_t, NULL, &refresh_config, NULL) != 0
		||
		pthread_create(&organize_t, NULL, &organize_files, NULL) != 0
	   )
	{
		// TODO - Log that the threads can't be created.
		pthread_mutex_destroy(&config_lock);
		return;
	}

	pthread_join(refresh_t, NULL);
	pthread_join(organize_t, NULL);

	// destroy the mutex.	
	pthread_mutex_destroy(&config_lock);
}
