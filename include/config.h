/* config.h  */
#ifndef FILE_SORTER_CORE_CONFIG_H
#define FILE_SORTER_CORE_CONFIG_H

#include <memory.h>

#define FAILED_TO_PARSE	-0x1


// config options.
struct options
{
	unsigned int o_check_interval;		// check interval option.
	unsigned int o_parse_interval;		// parse interval option.
	unsigned int o_debug_log;		// debug log option.
	char *o_default_path;			// default path option.
	unsigned int o_enable_default:1;	// enable default path option.
};

// config lists.
struct lists
{
	char *(*l_check_list);			// the check list of the config.
	char *(*l_target_list);			// the targests of the config.
	char *(*l_exclude_list);		// the excludes of the config.

};

// the config file.
struct config
{
	struct options c_options; 		// the options of the config.
	struct lists c_lists;			// the lists of the config.
};


/**
	Initialize the variable that contains the config
	file.
**/
static void inline init_config(struct config *config)
{
	memset(config, 0x0, sizeof(struct config));
}

/**
	Parse the config file and save the informations	
	inside the dst variable.
	@param dst The destination of where we put the config.
	@param path The path to the config file.
	@return The coresponded error that was occured.
*/
extern int parse_config(struct config *src, const char *path);



#endif
