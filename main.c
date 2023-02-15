#ifdef __unix__

#include <malloc.h>

#include <config.h>
#include <sorter.h>

#else
// TODO - include logger.
#endif


int main(int argc, char *argv[])
{
#ifdef __unix__
    struct config config_p;
    // initialzize the config.
    init_config(&config_p);

    // parse config.
    parse_config(&config_p);

    // start procces
    start_sorter(&config_p);

    destroy_config(&config_p);
#else
    // TODO - call logger and write that the oparating system is not compatiple.
#endif
}
