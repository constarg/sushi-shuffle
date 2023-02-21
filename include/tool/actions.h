#ifndef ACTIONS_H
#define ACTIONS_H

// option manipulation commands.
/**
 * Change the value of the check interval option
 * in config file.
 */
extern int set_check_interval(const char *n_value);

/**
 * Enable or disable the debug log
 * feature. This works by changing
 * the value of the corresponded value
 * in config file.
 */
extern int set_debug_log(const char *n_state);

/**
 * Change the default dir path, where the
 * files sent when no rule has been set.
 */
extern int set_default_dir_path(const char *n_path);

/**
 * Enable or disable the default dir
 * feature. This works by changing
 * the value of the corresponded value
 * in config file.
 */
extern int set_enable_default_dir(const char *n_state);

/**
 * Enable or disable the feature to
 * move the file without extention.
 * @param n_state The new state 0 or 1.
 */
extern int set_mv_without_ext(const char *n_state);


  // list manipulation commands.
/**
 * Add a new path on the check list that the core
 * will scan for files.
 */
extern int add_check(const char *path);

/**
 * Add a new rule on the target list that the core
 * checks in order to organize the files.
 * @param args The information that the builder pass to the callback.
 */
extern int add_target(const char *ext, const char *path);

/**
 * Add a new rule on the exlude list that the core
 * checks in order to exclude files.
 */
extern int add_exclude(const char *ext, const char *path);

/**
 * Remove a path from the check list that the core
 * will scan for files.
 */
extern int remove_check(const char *row);

/**
 * Remove a rule from the target list that the core
 * checks in order to organize the files.
 */
extern int remove_target(const char *row);

/**
 * Remove a rule from the exlude list that the core
 * checks in order to exclude files.
 */
extern int remove_exclude(const char *row);

// Show commands.
/**
 * Show the available options.
 */
extern int list_options();

/**
 * Show the list of path's in check list.
 * @param args The information that the builder pass to the callback.
 */
extern int list_checks();

/**
 * Show the list of rules in target list.
 */
extern int list_targets();

/**
 * Show the list of rules in exclude list.
 */
extern int list_excludes();

#endif
