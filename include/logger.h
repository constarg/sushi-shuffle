#ifndef SORTER_LOGGER_H
#define SORTER_LOGGER_H

// log tyoes.
#define ERROR 1
#define WAR   2
#define LOG   3

/**
	Logs a message.
	@param msg The message to log.
	@param type The type of the log.
*/
extern void logger(const char *msg, int type);


#endif
