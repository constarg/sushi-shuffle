#include <malloc.h>

#include <config.h>


static char *isolate_opt(const char *conf_buff)
{

}

static unsigned int parse_int_opt(const char *conf_buff, const char *opt)
{

}

static char *parse_str_opt(const char *conf_buff, const char *opt)
{

}

static char **parse_list(const char *conf_buff, const char *list)
{

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
	dst->c_options.o_debug_log = parse_int_opt(conf_buff, "debug_log") & 0x1;
	dst->c_options.o_default_path = parse_str_opt(conf_buff, "default_path");
	dst->c_options.o_enable_default = parse_int_opt(conf_buff, "enable_default") & 0x1;
	dst->c_lists.l_check_list = parse_list(conf_buff, "[check_list]");
	dst->c_lists.l_target_list = parse_list(conf_buff, "[target_list]");
	dst->c_lists.l_exclude_list = parse_list(conf_buff, "[exclude_list]");
}


int reparse_config(struct config *dst, const char *conf_buff)
{
	// free the allocated space where it should.
	free(dst->c_options.o_default_path);
	free_list(&dst->c_lists.l_check_list);
	free_list(&dst->c_lists.l_target_list);
	free_list(&dst->c_lists.l_exclude_list);
	// reparse.
	parse_config(dst, conf_buff);
}
