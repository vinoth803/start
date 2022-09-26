#include "common.h"

/* Log file which redirects the logs to 
 * log.txt file
 */
void BDN_log(char* format, ...) {
	FILE *file;
	char buffer[512];
	struct tm* current_tm;
      	time_t time_now;

    	time(&time_now);
    	current_tm = localtime(&time_now);

	va_list args;
    	va_start(args, format);
    	vsprintf(buffer, format, args);
    	va_end(args);

	file = fopen("log.txt", "a");

	if(file == NULL) {
		syslog(LOG_INFO,"unable to create log file");
		exit(1);
	}

	fprintf(file, "%02d:%02d:%02d:%s\n", current_tm->tm_hour
                				 , current_tm->tm_min
                				 , current_tm->tm_sec
						 , buffer);

	fclose(file);
}	
