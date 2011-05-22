#ifndef WN_STR_H
#define WN_STR_H

char *wn_str_chop (char *str, int no_spaces);
char *wn_str_crop (char *str, int n, char *add);

int wn_str_cat (char **str, char *what, int *blocks);

int wn_str_shift (char *str, int n);
int _wn_str_shift (char *str, int i, int n);

int wn_str_replace (char *str, char *test, char repl);

char *wn_str_escape (char **str);

char *wn_str_lcase (char *str);

int wn_str_match (char *str, char *pattern);

char *wn_str_html2latin (char *str);

#endif
