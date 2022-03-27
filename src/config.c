#include <malloc.h>
#include <string.h>
#include <stdlib.h>

#include <config.h>

static char *isolate_opt(const char *conf_buff, const char *opt)
{
	// don't change the original buffer.
	char *tmp = strdup(conf_buff);
	char *opt_c = strstr(tmp, opt);
	opt_c = strtok(opt_c, " "); // separete the option and the label.
	opt_c = strtok(NULL, "\n"); // get the option.
	opt_c = strdup(opt_c);

	free(tmp);
	return opt_c; // return value must be freed.
}

static inline unsigned int parse_int_opt(const char *conf_buff, const char *opt)
{
	char *opt_r = isolate_opt(conf_buff, opt);
	// TODO: Add warning in logger if the value is zero.	
	return (opt_r == NULL)? 0:atoi(opt_r); // get the int value.
}

static inline char *parse_str_opt(const char *conf_buff, const char *opt)
{
	// [!] TODO: Verify path before return.
	return isolate_opt(conf_buff, opt); // get the str value.
}

static char **parse_list(const char *conf_buff, const char *list)
{
	// locate the list.
	char *tmp = strdup(conf_buff);
	char *loc = strstr(tmp, list);
	if (loc == NULL) return NULL; // TODO: print warning using logger.

	char *curr_el = strtok(loc, "\n");

	unsigned int list_l = 10;
	unsigned int real_l = 0;
	char *(*list_r) = (char **) malloc(list_l * sizeof(char *));
	if (list_r == NULL) return NULL; // TODO: print warning using logger.

	for (int c = 0; curr_el; c++)
	{
		curr_el = strtok(NULL, "\n");
		if (curr_el == NULL) break;
		if (!strcmp(curr_el, "[done]")) break;
		real_l = c;

		if (c == list_l)
		{	
			list += 10;
			list_r = (char **) realloc(list_r,  list_l * sizeof(char *));
			if (list_r == NULL) return NULL; // TODO: print warning using logger. 	
		}
		list_r[c] = strdup(curr_el);
	}

	// null terminate the array.
	list_r[real_l] = NULL;
	free(tmp);
	return (char **) realloc(list_r, real_l * sizeof(char *)); // adjust the size of the list.
}

static inline void free_list(char *(**list))
{
	for (int i = 0; (*list)[i]; i++)
		free((*list)[i]);
	free(*list); 
	*list = NULL;
}

int parse_config(struct config *dst, const char *conf_buff)
{
	dst->c_options.o_check_interval = parse_int_opt(conf_buff, "check_interval");
	dst->c_options.o_parse_interval = parse_int_opt(conf_buff, "parse_interval");
	dst->c_options.o_debug_log 	= parse_int_opt(conf_buff, "debug_log") & 0x1;
	dst->c_options.o_default_path 	= parse_str_opt(conf_buff, "default_dir_path");
	dst->c_options.o_enable_default = parse_int_opt(conf_buff, "enable_default_path") & 0x1;
	dst->c_lists.l_check_list 	= parse_list(conf_buff, "[check]");
	dst->c_lists.l_target_list 	= parse_list(conf_buff, "[targets]");
	dst->c_lists.l_exclude_list 	= parse_list(conf_buff, "[exclude]");
}


int reparse_config(struct config *dst, const char *conf_buff)
{
	// free the allocated space where it should.
	free(dst->c_options.o_default_path);
	dst->c_options.o_default_path = NULL;
	free_list(&dst->c_lists.l_check_list);
	free_list(&dst->c_lists.l_target_list);
	free_list(&dst->c_lists.l_exclude_list);
	// reparse.
	parse_config(dst, conf_buff);
}
