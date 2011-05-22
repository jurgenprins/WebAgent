#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "wn_config.h"
#include "wn_str.h"

char *wn_str_chop (char *str, int no_spaces) {
	int i = 0, j;

	if (!str) 	
		return NULL;

	/* replace all control chars with space */
	while (str[i] != 0) {
		if (str[i] < 32) 
			str[i] = 32;
		i++;
	}

	/* remove (extraneous) spaces in text */
	i = 0;
	while (str[i] != 0) {
		if ((str[i] == 32) && (no_spaces || ((str[i + 1] == 32) || 
		     (str[i + 1] == '\r') || (str[i + 1] == '\n') ||
		     (str[i + 1] == '\t')))) {
			for (j = i; j < strlen (str); j++) 
				str[j] = str[j + 1];
		} else i++;
	}

	/* remove leading blanks */
	while (str[0] == ' ') 
		for (j = 0; j < strlen (str); j++)
			str[j] = str[j + 1];

	/* remove trailing blanks */
	i = strlen (str) - 1;
	while ((i >= 0) && (str[i] == ' ')) {
		str[i] = 0;
		i--;
	}

	return str;
}

char *wn_str_crop (char *str, int n, char *add) {
	int i, j = n;

	if (!str)
		return NULL;

	if (add)
		j = n + strlen (add);
		
	if ((n <= 0) || (strlen (str) <= j))
		return str;

	for (i = n; i < j; i++)
		str[i] = add[i - n];
	str[i] = 0;

	return str;
}

int wn_str_cat (char **str, char *what, int *blocks) {
	if (!what)
		return 0;

	if (!(*str)) {
		*blocks = 0;
		*str = (char *) malloc (MAX_SIZE * ++(*blocks));
		(*str)[0] = 0;
	} 

	while ((strlen (*str) + strlen (what)) > (MAX_SIZE * (*blocks)))
		*str = (char *) realloc (*str, MAX_SIZE * ++(*blocks));
	
	strcat (*str, what);

	return 1;
}

int _wn_str_shift (char *str, int i, int n) {
	int len;

	if (!str || (n <= 0)) return 0;

	len = strlen (str) - n;
	while (i <= len) {
		str[i] = str[i + n];
		i++;
	}
	
	return n;
}

int wn_str_shift (char *str, int n) {
	return _wn_str_shift (str, 0, n);
}

int wn_str_replace (char *str, char *test, char repl) {
	int i;

	if (!str || !test || !repl || (strlen (str) <  strlen (test)))
		return 0;

	for (i = 0; i < strlen (test); i++) {
		if (str[i] != test[i])
			return 0;
	}

	str[0] = repl;
	return _wn_str_shift (str, 1, strlen (test) - 1);
}

char *wn_str_escape (char **str) {
	int i, j, len;

	if (!(*str) || (!(*str = (char *) realloc (*str, (strlen (*str) * 2) + 1))))
		return NULL;
	
	len = strlen (*str);

	for (i = 0; i < len; i++) {
		if ((*str)[i] == '\'') {
			for (j = len; j > i; j--) 
				(*str)[j] = (*str)[j - 1];
			len++;
			(*str)[i] = '\\';
			i++;
		}	
	}

	(*str)[len] = 0;
	return *str;
}

char *wn_str_crlf (char **str) {
	int i, j, len;

	if (!(*str) || (!(*str = (char *) realloc (*str, (strlen (*str) * 2) + 1))))
		return NULL;
	
	len = strlen (*str);

	for (i = 0; i < len; i++) {
		if ((*str)[i] == '\n') {
			if (!i || ((*str)[i - 1] != '\r')) {
				for (j = len; j > i; j--) 
					(*str)[j] = (*str)[j - 1];
				len++;
				(*str)[i] = '\r';
				i++;
			}
		}	
	}

	(*str)[len] = 0;
	return *str;
}

char *wn_str_lcase (char *str) {
	int i = 0;
	
	if (!str) 
		return NULL;

	for (i = 0; str[i] != 0; i++)
		if ((str[i] >= 'A') && (str[i] <= 'Z'))
			str[i] += 32;

	return str;
}

int wn_str_match (char *str, char *pattern) {
	char *tmp1, *tmp2;
	int res;

	if (!pattern)
		return 1;

	if (!str) 
		return 0;

	tmp1 = wn_str_lcase (strdup (str));
	tmp2 = wn_str_lcase (strdup (pattern));

	res = (strstr (tmp1, tmp2) != NULL);

	free (tmp1);
	free (tmp2);

	return res;
}

char *wn_str_html2latin (char *str) {
	int i = 0, len;

	if (!str) 
		return NULL;

	len = strlen (str);

	for (i = 0; i < len - 1; i++) {
		if (str[i] == '&') {
		if (wn_str_replace (&str[i], "&quot;", 34)) continue;
		if (wn_str_replace (&str[i], "&amp;", 38)) continue;
		if (wn_str_replace (&str[i], "&lt;", 60)) continue;
		if (wn_str_replace (&str[i], "&gt;", 62)) continue;
		if (wn_str_replace (&str[i], "&nbsp;", 160)) continue;
		if (wn_str_replace (&str[i], "&iexcl;", 161)) continue;
		if (wn_str_replace (&str[i], "&cent;", 162)) continue;
		if (wn_str_replace (&str[i], "&pound;", 163)) continue;
		if (wn_str_replace (&str[i], "&curren;", 164)) continue;
		if (wn_str_replace (&str[i], "&yen;", 165)) continue;
		if (wn_str_replace (&str[i], "&brvbar;", 166)) continue;
		if (wn_str_replace (&str[i], "&sect;", 167)) continue;
		if (wn_str_replace (&str[i], "&uml;", 168)) continue;
		if (wn_str_replace (&str[i], "&copy;", 169)) continue;
		if (wn_str_replace (&str[i], "&ordf;", 170)) continue;
		if (wn_str_replace (&str[i], "&laquo;", 171)) continue;
		if (wn_str_replace (&str[i], "&not;", 172)) continue;
		if (wn_str_replace (&str[i], "&shy;", 173)) continue;
		if (wn_str_replace (&str[i], "&reg;", 174)) continue;
		if (wn_str_replace (&str[i], "&macr;", 175)) continue;
		if (wn_str_replace (&str[i], "&deg;", 176)) continue;
		if (wn_str_replace (&str[i], "&plusmn;", 177)) continue;
		if (wn_str_replace (&str[i], "&sup2;", 178)) continue;
		if (wn_str_replace (&str[i], "&sup3;", 179)) continue;
		if (wn_str_replace (&str[i], "&acute;", 180)) continue;
		if (wn_str_replace (&str[i], "&micro;", 181)) continue;
		if (wn_str_replace (&str[i], "&para;", 182)) continue;
		if (wn_str_replace (&str[i], "&middot;", 183)) continue;
		if (wn_str_replace (&str[i], "&cedil;", 184)) continue;
		if (wn_str_replace (&str[i], "&sup1;", 185)) continue;
		if (wn_str_replace (&str[i], "&ordm;", 186)) continue;
		if (wn_str_replace (&str[i], "&raquo;", 187)) continue;
		if (wn_str_replace (&str[i], "&frac14;", 188)) continue;
		if (wn_str_replace (&str[i], "&frac12;", 189)) continue;
		if (wn_str_replace (&str[i], "&frac34;", 190)) continue;
		if (wn_str_replace (&str[i], "&iquest;", 191)) continue;
		if (wn_str_replace (&str[i], "&Agrave;", 192)) continue;
		if (wn_str_replace (&str[i], "&Aacute;", 193)) continue;
		if (wn_str_replace (&str[i], "&Acirc;", 194)) continue;
		if (wn_str_replace (&str[i], "&Atilde;", 195)) continue;
		if (wn_str_replace (&str[i], "&Auml;", 196)) continue;
		if (wn_str_replace (&str[i], "&Aring;", 197)) continue;
		if (wn_str_replace (&str[i], "&AElig;", 198)) continue;
		if (wn_str_replace (&str[i], "&Ccedil;", 199)) continue;
		if (wn_str_replace (&str[i], "&Egrave;", 200)) continue;
		if (wn_str_replace (&str[i], "&Eacute;", 201)) continue;
		if (wn_str_replace (&str[i], "&Ecirc;", 202)) continue;
		if (wn_str_replace (&str[i], "&Euml;", 203)) continue;
		if (wn_str_replace (&str[i], "&Igrave;", 204)) continue;
		if (wn_str_replace (&str[i], "&Iacute;", 205)) continue;
		if (wn_str_replace (&str[i], "&Icirc;", 206)) continue;
		if (wn_str_replace (&str[i], "&Iuml;", 207)) continue;
		if (wn_str_replace (&str[i], "&ETH;", 208)) continue;
		if (wn_str_replace (&str[i], "&Ntilde;", 209)) continue;
		if (wn_str_replace (&str[i], "&Ograve;", 210)) continue;
		if (wn_str_replace (&str[i], "&Oacute;", 211)) continue;
		if (wn_str_replace (&str[i], "&Ocirc;", 212)) continue;
		if (wn_str_replace (&str[i], "&Otilde;", 213)) continue;
		if (wn_str_replace (&str[i], "&Ouml;", 214)) continue;
		if (wn_str_replace (&str[i], "&times;", 215)) continue;
		if (wn_str_replace (&str[i], "&Oslash;", 216)) continue;
		if (wn_str_replace (&str[i], "&Ugrave;", 217)) continue;
		if (wn_str_replace (&str[i], "&Uacute;", 218)) continue;
		if (wn_str_replace (&str[i], "&Ucirc;", 219)) continue;
		if (wn_str_replace (&str[i], "&Uuml;", 220)) continue;
		if (wn_str_replace (&str[i], "&Yacute;", 221)) continue;
		if (wn_str_replace (&str[i], "&THORN;", 222)) continue;
		if (wn_str_replace (&str[i], "&szlig;", 223)) continue;
		if (wn_str_replace (&str[i], "&agrave;", 224)) continue;
		if (wn_str_replace (&str[i], "&aacute;", 225)) continue;
		if (wn_str_replace (&str[i], "&acirc;", 226)) continue;
		if (wn_str_replace (&str[i], "&atilde;", 227)) continue;
		if (wn_str_replace (&str[i], "&auml;", 228)) continue;
		if (wn_str_replace (&str[i], "&aring;", 229)) continue;
		if (wn_str_replace (&str[i], "&aelig;", 230)) continue;
		if (wn_str_replace (&str[i], "&ccedil;", 231)) continue;
		if (wn_str_replace (&str[i], "&egrave;", 232)) continue;
		if (wn_str_replace (&str[i], "&eacute;", 233)) continue;
		if (wn_str_replace (&str[i], "&ecirc;", 234)) continue;
		if (wn_str_replace (&str[i], "&euml;", 235)) continue;
		if (wn_str_replace (&str[i], "&igrave;", 236)) continue;
		if (wn_str_replace (&str[i], "&iacute;", 237)) continue;
		if (wn_str_replace (&str[i], "&icirc;", 238)) continue;
		if (wn_str_replace (&str[i], "&iuml;", 239)) continue;
		if (wn_str_replace (&str[i], "&eth;", 240)) continue;
		if (wn_str_replace (&str[i], "&ntilde;", 241)) continue;
		if (wn_str_replace (&str[i], "&ograve;", 242)) continue;
		if (wn_str_replace (&str[i], "&oacute;", 243)) continue;
		if (wn_str_replace (&str[i], "&ocirc;", 244)) continue;
		if (wn_str_replace (&str[i], "&otilde;", 245)) continue;
		if (wn_str_replace (&str[i], "&ouml;", 246)) continue;
		if (wn_str_replace (&str[i], "&divide;", 247)) continue;
		if (wn_str_replace (&str[i], "&oslash;", 248)) continue;
		if (wn_str_replace (&str[i], "&ugrave;", 249)) continue;
		if (wn_str_replace (&str[i], "&uacute;", 250)) continue;
		if (wn_str_replace (&str[i], "&ucirc;", 251)) continue;
		if (wn_str_replace (&str[i], "&uuml;", 252)) continue;
		if (wn_str_replace (&str[i], "&yacute;", 253)) continue;
		if (wn_str_replace (&str[i], "&thorn;", 254)) continue;
		if (wn_str_replace (&str[i], "&yuml;", 255)) continue;
		}
	}

	return str;
}
