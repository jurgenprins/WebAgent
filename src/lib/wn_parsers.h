#ifndef WN_PARSERS_H
#define WN_PARSERS_H

#include "parsers/wn_html.h"

static void *parsers[] = 
{
	"text/html", wn_html2fmt,
	NULL
};

#endif
