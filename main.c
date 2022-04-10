#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include <malloc.h>

#include <config.h>
#include <sorter.h>

#define CONF_PATH "/.local/share/file_sorter/config/config.conf"


int main(int argc, char *argv[])
{
	struct config config_p;
	// initialzize the config.
	init_config(&config_p);

	// determine the absolute path of the config.
	char *username = getlogin();
	char *absolute = (char *) malloc((strlen(CONF_PATH) + strlen(username) + 7) * 
					 sizeof(char));
	strcpy(absolute, "/home/");
	strcat(absolute, username);
	strcat(absolute, CONF_PATH);	

	// read the size of the config.
	struct stat conf_stat;
	if (lstat(absolute, &conf_stat) != 0) return -1; // TODO: Call the logger here.

	char *conf_buff = (char *) malloc((conf_stat.st_size + 1) * sizeof(char));
	int conf_fd = open(absolute, O_RDONLY);
	if (conf_fd == -1) return -1; // TODO: Call the logger here. 

	if (read(conf_fd, conf_buff, conf_stat.st_size) == -1) return -1; // TODO: Call the logger here.
	close(conf_fd);
	conf_buff[conf_stat.st_size] = '\0';

	free(absolute);
	// parse config.
	parse_config(&config_p, conf_buff);
	free(conf_buff);

	// start procces
	start_sorter(&config_p);

	destroy_config(&config_p);
}
