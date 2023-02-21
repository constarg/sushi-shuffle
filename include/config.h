/* config.h  */
#ifndef FILE_SORTER_CORE_CONFIG_H
#define FILE_SORTER_CORE_CONFIG_H

#include <memory.h>

#define FAILED_TO_PARSE    -0x1

/**
 * *******************
 *   		OPTIONS
 * *******************
 */
#define CHECK_INT   "check_interval"
#define PARSE_INT   "parse_interval"
#define DEBUG_LOG   "debug_log"
#define DEFAULT_DIR "default_dir_path"
#define EN_DEFAULT  "enable_default_path"
#define WITHOUT_EXT "move_files_without_ext"

/**
 * ******************
 * 			LISTS
 * ******************
 */
#define CHECK_LISTID 	  "[check]"
#define TARGET_LISTID   "[targets]"
#define EXCLUDE_LISTID  "[exclude]"


// config options.
struct options
{
    unsigned int o_check_interval;        // check interval option.
    unsigned int o_debug_log: 1;          // debug log option.
    char        *o_default_path;          // default path option.
    unsigned int o_enable_default: 1;     // enable default path option.
    unsigned int o_move_no_ext: 1;        // move files that has no extention.
};

// config lists.
struct lists
{
    char *(*l_check_list);                // the check list of the config.
    char *(*l_target_list);               // the targests of the config.
    char *(*l_exclude_list);              // the excludes of the config.
};

// the config file.
struct config
{
    struct options c_options;             // the options of the config.
    struct lists c_lists;                 // the lists of the config.
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
*/
extern void parse_config(struct config *dst);

extern void reparse_config(struct config *dst);

extern void destroy_config(struct config *src);

extern int update_config(const struct config *src);

#endif
