#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include "wn_err.h"

int err_set = 0;
char err_msg[ERR_SIZE];
int err_no;

int wn_report_error (int errno, char *format, ...) {
	va_list ap;

	if (!err_set) 
		err_set = 1;

	err_no = errno;
	va_start (ap, format);
	vsprintf (err_msg, format, ap);
	va_end (ap);

	return 1;
}

char *wn_get_error () {
	if (!err_set)
		strcpy (err_msg, "no error");
	return err_msg;
}	

int wn_get_errno () {
	if (err_set)
		return err_no;
	else
		return WE_NONE;
}

char *wn_get_errstr (int err_no) {
	int i = 0;

	while (i <= WE_UNKNOWN) {
		if (errors[i].err_no == err_no)
			return errors[i].err_str;

		 i++;
	}

	return "unknown error";
}

void wn_discard_error() {
	err_set = 0;
}

