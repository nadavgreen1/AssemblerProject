/* error_handler.h */
/* Error handling interface */

#ifndef ERROR_HANDLER_H
#define ERROR_HANDLER_H

void report_error(const char *message, int line_number);
void report_general_error(const char *message);

#endif
