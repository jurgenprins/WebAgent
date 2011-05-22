/* WN-Format data storage (memory or disk). JP
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <regex.h>
#include "wn_fmt.h"
#include "wn_net.h"
#include "wn_err.h"
#include "wn_str.h"
#include "wn_config.h"
#include "wn_parsers.h"

regex_t reg_wn_fmt_text;
int reg_compiled = 0;

wn_fmt_t *wn_fmt_init () {
	wn_fmt_t *wn_fmt = (wn_fmt_t *) malloc (sizeof (wn_fmt_t));
	if (!wn_fmt) return NULL;

	wn_fmt->first = NULL;
	wn_fmt->last = NULL;
	wn_fmt->size = 0;
	wn_fmt->blocknr = 1;
	wn_fmt->in_block = 0;
	wn_fmt->text_count = 0;
	wn_fmt->link_count = 0;
	wn_fmt->image_count = 0;

	return wn_fmt;
}

wn_fmt_tag_t *wn_fmt_add (wn_fmt_t *wn_fmt, int type, char *param, char *content) {
	wn_fmt_tag_t *wn_fmt_tag;

	if (!wn_fmt || (!param && !content)) {
		wn_report_error (WE_PARAM, "wn_fmt_add: insufficient parameters");
		return NULL;
	}

	if ((param && (!strlen ((type == WN_FMT_RAW) ? param : (param = wn_str_chop (param, 1))))) ||
	    (content && (!strlen ((type == WN_FMT_RAW) ? content : (content = wn_str_chop (content, 0))))))
		return NULL;

	wn_fmt_tag = (wn_fmt_tag_t *) malloc (sizeof (wn_fmt_tag_t));
	wn_fmt_tag->type = type;
	wn_fmt_tag->param = param ? strdup (param) : NULL;
	wn_fmt_tag->content = content ? strdup (content) : NULL;
	if (wn_fmt->in_block)
		wn_fmt_tag->blocknr = wn_fmt->blocknr;
	else
		wn_fmt_tag->blocknr = 1;
	switch (type) {
	case WN_FMT_TEXT:
		wn_fmt->text_count++;
		break;
	case WN_FMT_LINK:
		wn_fmt->link_count++;
		break;
	case WN_FMT_IMAGE:
		wn_fmt->image_count++;
		break;
	default:
	}
	wn_fmt_tag->next = NULL;

	if (!wn_fmt->first || !wn_fmt->last)
		wn_fmt->first = wn_fmt_tag;
	else
		wn_fmt->last->next = wn_fmt_tag;
	wn_fmt->last = wn_fmt_tag;
	wn_fmt->size++;

	return wn_fmt->last;
}

int wn_fmt_update (wn_fmt_tag_t *wn_fmt_tag, char *param, char *content) {
	if (!wn_fmt_tag) {
		wn_report_error (WE_PARAM, "wn_fmt_update: insufficient parameters");
		return 0;
	}

	if (param && (strlen ((wn_fmt_tag->type == WN_FMT_RAW) ? param : (param = wn_str_chop (param, 1))))) {
		if (wn_fmt_tag->param) 
			free (wn_fmt_tag->param);
		wn_fmt_tag->param = strdup (param);
	}

	if (content && (strlen ((wn_fmt_tag->type == WN_FMT_RAW) ? content : (content = wn_str_chop (content, 0))))) {
		if (wn_fmt_tag->content) 
			free (wn_fmt_tag->content);
		wn_fmt_tag->content = strdup (content);
	}

	return 1;
}

int wn_fmt_expand (wn_fmt_tag_t *wn_fmt_tag, char *param, char *content) {
	if (!wn_fmt_tag) {
		wn_report_error (WE_PARAM, "wn_fmt_expand: insufficient parameters");
		return 0;
	}

	if (param && (strlen ((wn_fmt_tag->type == WN_FMT_RAW) ? param : (param = wn_str_chop (param, 1))))) {
		if (wn_fmt_tag->param) {
			if (!(wn_fmt_tag->param = (char *) realloc (wn_fmt_tag->param, strlen (wn_fmt_tag->param) + strlen (param) + 1)))
				return 0;
			strcat (wn_fmt_tag->param, param);
		} else 
			wn_fmt_tag->param = strdup (param);
	}

	if (content && (strlen ((wn_fmt_tag->type == WN_FMT_RAW) ? content : (content = wn_str_chop (content, 0))))) {
		if (wn_fmt_tag->content) {
			if (!(wn_fmt_tag->content = (char *) realloc (wn_fmt_tag->content, strlen (wn_fmt_tag->content) + strlen (content) + 1)))
				return 0;
			strcat (wn_fmt_tag->content, content);
		} else 
			wn_fmt_tag->content = strdup (content);
	}

	return 1;
}

wn_fmt_tag_t *wn_fmt_find_by_param (wn_fmt_t *wn_fmt, char *param) {
	wn_fmt_tag_t *wn_fmt_tag;

	if (!wn_fmt || !param) {
		wn_report_error (WE_PARAM, "wn_fmt_find_by_param: insufficient parameters");
		return NULL;
	}

	for (wn_fmt_tag = wn_fmt->first; wn_fmt_tag; wn_fmt_tag = wn_fmt_tag->next) {
		if (wn_fmt_tag->param && (!strcasecmp (wn_fmt_tag->param, param)))
			return wn_fmt_tag;
	}

	return NULL;
}

void wn_fmt_retract (wn_fmt_t *wn_fmt) {
	wn_fmt_tag_t *tag;

	if (!wn_fmt->first || !wn_fmt->last) 
		return;

	if (wn_fmt->size == 1) {
		if (wn_fmt->first->content) free (wn_fmt->first->content);
		if (wn_fmt->first->param) free (wn_fmt->first->param);
		switch (wn_fmt->first->type) {
		case WN_FMT_TEXT:
			wn_fmt->text_count--;
			break;
		case WN_FMT_LINK:
			wn_fmt->link_count--;
			break;
		case WN_FMT_IMAGE:
			wn_fmt->image_count--;
			break;
		default:
		}
		free (wn_fmt->first);
		wn_fmt->first = NULL;
		wn_fmt->last = NULL;
		return;
	}

	for (tag = wn_fmt->first; tag; tag = tag->next) {
		if (tag->next && (tag->next->type == wn_fmt->last->type) &&
		    (!tag->next->param || (wn_fmt->last->param && !strcmp (tag->next->param, wn_fmt->last->param))) &&
		    (!tag->next->content || (wn_fmt->last->content && !strcmp (tag->next->content, wn_fmt->last->content)))) {

			if (wn_fmt->last->content) free (wn_fmt->last->content);
			if (wn_fmt->last->param) free (wn_fmt->last->param);
			switch (wn_fmt->last->type) {
			case WN_FMT_TEXT:
				wn_fmt->text_count--;
				break;
			case WN_FMT_LINK:
				wn_fmt->link_count--;
				break;
			case WN_FMT_IMAGE:
				wn_fmt->image_count--;
				break;
			default:
			}
			free (wn_fmt->last);
			tag->next = NULL;	
			wn_fmt->last = tag;
			break;
		}
	}
}

int wn_fmt_setblock (wn_fmt_t *wn_fmt) {
	wn_fmt->blocknr++;
	wn_fmt->in_block = 1;

	return 1;
}

int wn_fmt_unsetblock (wn_fmt_t *wn_fmt) {
	wn_fmt->in_block = 0;

	return 1;
}

int wn_fmt_free (wn_fmt_t *wn_fmt) {
	wn_fmt_tag_t *wn_fmt_tag1, *wn_fmt_tag2;

	if (!wn_fmt) return 1;

	wn_fmt_tag1 = wn_fmt->first;
	while (wn_fmt_tag1) {
		wn_fmt_tag2 = wn_fmt_tag1->next;
		if (wn_fmt_tag1->param) free (wn_fmt_tag1->param);
		if (wn_fmt_tag1->content) free (wn_fmt_tag1->content);
		free (wn_fmt_tag1);
		wn_fmt_tag1 = wn_fmt_tag2;
	}

	free (wn_fmt);
	wn_fmt = NULL;

	return 1;
}

wn_fmt_t *_wn_fmt_parse (wn_fmt_t *fmt, wn_url_t *url) {
	int idx = 0;
	int (*parser_fun)() = NULL;
	wn_fmt_t *new_fmt = NULL;

	if (!url) {
		wn_report_error (WE_PARAM, "wn_fmt_parse: no url retrieved");
		return fmt;
	}

	if (!url->mime_type) {
		url->mime_type = (char *) malloc (10);
		strcpy (url->mime_type, "text/html");
	}

	while (parsers[idx]) {
		if (!strcmp ((char *)parsers[idx], url->mime_type)) {
			parser_fun = parsers[idx + 1];
			break;
		}
		idx += 2;
	}

	if (!parser_fun) {
		wn_report_error (WE_PARSE, "wn_fmt_parse: no parser available for mimetype %s", url->mime_type);
		return fmt;
	}

	if (!fmt) {
		if (!(new_fmt = wn_fmt_init ()))
			return NULL;
	} else
		new_fmt = fmt;

	if (!parser_fun (url, new_fmt)) {
		wn_fmt_free (new_fmt);
		return NULL;	
	}

	return new_fmt;
}

wn_fmt_t *wn_fmt_parse (wn_url_t *url) {
	return _wn_fmt_parse (NULL, url);
}

wn_fmt_t *wn_fmt_fetch (wn_url_t *url, int timeout, int recurse_frames) {
	wn_fmt_t *fmt, *tmp_fmt;
	wn_url_t *frame, *new_url;
	char *url_str = NULL;
	char *host = NULL;
	int count = 0;

	if (!url) {
		wn_report_error (WE_PARAM, "wn_fmt_fetch: no url given");
		return NULL;
	}

	if (timeout < 0) 
		timeout = 0;

	if (!wn_net_fetch (url, timeout))
		return NULL;

	if (!(fmt = _wn_fmt_parse (NULL, url))) 
		return NULL;
	

	if (recurse_frames) {
		if ((frame = url->frame) && (url->last_modified)) {
			free (url->last_modified);
			url->last_modified = NULL;
		}
	
		while (frame) {
			if (!wn_net_fetch (frame, timeout))  {
				wn_fmt_free (fmt);
				return NULL;
			}

			if (!(fmt = _wn_fmt_parse (fmt, frame))) 
				return NULL;  // fmt is already freed

			frame = frame->frame;
		}
	}	

	while ((fmt->link_count == 1) && (++count < 5)) {	
		if (!(tmp_fmt = wn_fmt_filter (fmt, WN_FMT_LINK, NULL, NULL))) {
			wn_fmt_free (fmt);
			return NULL;
		}

		wn_fmt_free (fmt);

		if (!tmp_fmt->first || !tmp_fmt->first->param ||
		    !(url_str = wn_urlify (url, tmp_fmt->first->param))) {
			wn_fmt_free (tmp_fmt);
			return NULL;
		}

		wn_fmt_free (tmp_fmt);

		if (!(host = strtok ((host = strdup (&url_str[7])), "/"))) {
			free (url_str);
			return NULL;
		}

		if (!(new_url = wn_url_init (0, host, &url_str[strlen (host) + 7], NULL))) {
			free (url_str);
			free (host);
			return NULL;
		}

		free (url_str);
		free (host);	

		if (!wn_net_fetch (new_url, timeout)) {
			return NULL;
		}

		if (!(fmt = _wn_fmt_parse (NULL, new_url))) {
			wn_url_free (new_url);
			return NULL;
		}

		if (recurse_frames) {
			if ((frame = new_url->frame) && (new_url->last_modified)) {
				free (new_url->last_modified);
				new_url->last_modified = NULL;
			}
		
			while (frame) {
				if (!wn_net_fetch (frame, timeout))  {
					wn_fmt_free (fmt);
					return NULL;
				}
	
				if (!(fmt = _wn_fmt_parse (fmt, frame))) 
					return NULL;  // fmt is already freed

				frame = frame->frame;
			}
		}	

		wn_url_free (new_url);
	}

	return fmt;
}

wn_fmt_t *wn_fmt_diff (wn_fmt_t *wn_fmt_old, wn_fmt_t *wn_fmt_new) {
	int found;
	wn_fmt_t *wn_fmtdiff; 
	wn_fmt_tag_t *wn_fmt_tag, *wn_fmt_new_tag, *wn_fmt_old_tag;

	if (!wn_fmt_old || !wn_fmt_new) {
		wn_report_error (WE_PARAM, "wn_fmt_diff: either old or new format is absent");
		return NULL;
	}

	wn_fmtdiff = wn_fmt_init();

	for (wn_fmt_new_tag = wn_fmt_new->first; wn_fmt_new_tag; wn_fmt_new_tag = wn_fmt_new_tag->next) {
		found = 0;
		for (wn_fmt_old_tag = wn_fmt_old->first; wn_fmt_old_tag; wn_fmt_old_tag = wn_fmt_old_tag->next) {
			if ((wn_fmt_old_tag->type == wn_fmt_new_tag->type) &&
			    ((wn_fmt_new_tag->type == WN_FMT_LINK) ||
			     ((!wn_fmt_old_tag->param || 
			       (wn_fmt_old_tag->param && !strcmp (wn_fmt_old_tag->param, WN_FMT_CHAR_NULL))) && 
			      (!wn_fmt_new_tag->param || 
			       (wn_fmt_new_tag->param && !strcmp (wn_fmt_new_tag->param, WN_FMT_CHAR_NULL)))) ||
			     ((wn_fmt_old_tag->param && wn_fmt_new_tag->param) && 
			      (!strcasecmp (wn_fmt_old_tag->param, wn_fmt_new_tag->param)))) &&
			    ((((!wn_fmt_old_tag->content ||
			       (wn_fmt_old_tag->content && !strcmp (wn_fmt_old_tag->content, WN_FMT_CHAR_NULL))) &&
			      (!wn_fmt_new_tag->content ||
			       (wn_fmt_new_tag->content && !strcmp (wn_fmt_new_tag->content, WN_FMT_CHAR_NULL)))) || 
			     ((wn_fmt_old_tag->content && wn_fmt_new_tag->content) && 
			      (!strcasecmp (wn_fmt_old_tag->content, wn_fmt_new_tag->content)))))) {
				found = 1;
				break;
			}
		}
		if (!found) {
			wn_fmt_tag = wn_fmt_add (wn_fmtdiff, wn_fmt_new_tag->type, wn_fmt_new_tag->param, wn_fmt_new_tag->content);
			wn_fmt_tag->blocknr = wn_fmt_new_tag->blocknr;
		}
	}
	return wn_fmtdiff;
}

wn_fmt_t *wn_fmt_filter (wn_fmt_t *wn_fmt, int type, char *pattern, char *blocks) {
	wn_fmt_t *wn_fmtfiltered;
	wn_fmt_tag_t *wn_fmt_tag, *wn_fmtfiltered_tag;

	if (!wn_fmt) {
		wn_report_error (WE_PARAM, "wn_fmt_filter: format absent");
		return NULL;
	}

	wn_fmtfiltered = wn_fmt_init();

	for (wn_fmt_tag = wn_fmt->first; wn_fmt_tag; wn_fmt_tag = wn_fmt_tag->next) {
		if ((type & wn_fmt_tag->type) && 
		    (wn_str_match ((wn_fmt_tag->type == WN_FMT_IMAGE) ? wn_fmt_tag->param : wn_fmt_tag->content, pattern)) &&
		    (!blocks || (wn_fmt_tag->blocknr > 255) ||
		     !strchr(blocks,(char)wn_fmt_tag->blocknr))) {
			if ((wn_fmtfiltered_tag = wn_fmt_add (wn_fmtfiltered, wn_fmt_tag->type, wn_fmt_tag->param, wn_fmt_tag->content)))
				wn_fmtfiltered_tag->blocknr = wn_fmt_tag->blocknr;
		}
	}

	return wn_fmtfiltered;
}

char *wn_fmt_type2str (int type) {
	switch (type) {
	case WN_FMT_ERROR: return "ERROR";
	case WN_FMT_LINK: return "LINK";
	case WN_FMT_TEXT: return "TEXT";
	case WN_FMT_IMAGE: return "IMG";
	default: return "??";
	}
}

int wn_fmt_type2int (char *type) {
	if (!type) return WN_FMT_RAW;
	if (!strncasecmp (type, "ER", 2)) return WN_FMT_ERROR;
	if (!strncasecmp (type, "LI", 2)) return WN_FMT_LINK;
	if (!strncasecmp (type, "TE", 2)) return WN_FMT_TEXT;
	if (!strncasecmp (type, "IM", 2)) return WN_FMT_IMAGE;
	return -1;
}

int wn_fmt_typecount (wn_fmt_t *wn_fmt, int type) {
	int count = 0;

	if (!wn_fmt)
		return 0;

	if (type & WN_FMT_TEXT)
		count += wn_fmt->text_count;
	if (type & WN_FMT_LINK)
		count += wn_fmt->link_count;
	if (type & WN_FMT_IMAGE)
		count += wn_fmt->image_count;

	return count;
}

int wn_fmt_save (wn_fmt_t *wn_fmt, char *fname) {
	FILE *f;
	wn_fmt_tag_t *wn_fmt_tag;

	if (!wn_fmt || !fname) {
		wn_report_error (WE_PARAM, "wn_fmt_save: Parameter(s) not set");
		return 0;
	}

	if (!(f = fopen (fname, "w"))) {
		wn_report_error (WE_FILE, "wn_fmt_save: %s cannot be opened for writing", fname);
		return 0;
	}

	for (wn_fmt_tag = wn_fmt->first; wn_fmt_tag; wn_fmt_tag = wn_fmt_tag->next) {
		if (wn_fmt_tag->type && (wn_fmt_tag->param || wn_fmt_tag->content))
			fprintf (f, "%i %s %s %s\n", wn_fmt_tag->blocknr, wn_fmt_type2str (wn_fmt_tag->type), wn_fmt_tag->param ? wn_fmt_tag->param : WN_FMT_CHAR_NULL, wn_fmt_tag->content ? wn_fmt_tag->content : WN_FMT_CHAR_NULL);
	}

	fclose (f);
	return 1;
}

wn_fmt_t *_wn_fmt_load (char *fname, int max_lines) {
	FILE *f;
	int reg_status = 0, count = 0, i, start, end, str_len;
	regmatch_t regs[10];
	wn_fmt_t *wn_fmt = wn_fmt_init();
	wn_fmt_tag_t *wn_fmt_tag;
	char *str = (char *) malloc (MAX_SIZE);
	char *type = NULL, *param = NULL, *content = NULL, *id = NULL;

	if (!fname) {
		wn_report_error (WE_PARAM, "wn_fmt_load: Parameter(s) not set");
		wn_fmt_free (wn_fmt);
		return NULL;
	}

	if (!(f = fopen (fname, "r"))) {
		wn_report_error (WE_FILE, "wn_fmt_load: %s cannot be opened for reading", fname);
		wn_fmt_free (wn_fmt);
		return NULL;
	}

	if (!reg_compiled) {
		if ((reg_status = regcomp (&reg_wn_fmt_text, "([0-9]+) ([A-Z]+) ([^ ]+) ([^\n\r]+)", REG_EXTENDED | REG_ICASE))) {
			wn_report_error (WE_REGC, "wn_fmt_load: XML format compilation failed (%i)", reg_status);
			fclose (f);
			wn_fmt_free (wn_fmt);
			return NULL;
		} else
			reg_compiled = 1;
	}

	while (fgets (str, MAX_SIZE - 1, f)) {
		if (max_lines && (count >= max_lines)) break;
		count++;
		reg_status = regexec (&reg_wn_fmt_text, str, 10, regs, 0);
		if (reg_status) {
			if (reg_status == REG_NOMATCH) 
				wn_report_error (WE_REGE, "wn_fmt_load: Unknown XML format at line %i", count);
			else 
				wn_report_error (WE_REGE, "wn_fmt_load: XML format matching failed (%i)", reg_status);
			fclose (f);
			wn_fmt_free (wn_fmt);
			return NULL;
		}

		str_len = strlen (str);
		for (i = 1; i < 10; i++) {
			start = regs[i].rm_so;
			end = regs[i].rm_eo;
			if ((start != -1) && (end > 0) && (start < str_len) && (end < str_len) && (start <= end)) {
				switch (i) {
				case 1: id = (char *) malloc (end - start + 1);
				        strncpy (id, &str[start], end - start);
				        id[end - start] = 0;
				        break;
				case 2: type = (char *) malloc (end - start + 1);
				        strncpy (type, &str[start], end - start);
				        type[end - start] = 0;
				        break;
				case 3: param = (char *) malloc (end - start + 1);
				        strncpy (param, &str[start], end - start);
				        param[end - start] = 0;
				        break;
				case 4: content = (char *) malloc (end - start + 1);
				        strncpy (content, &str[start], end - start);
				        content[end - start] = 0;
				default:
				}
			} else break;
		}

		if (type && (param || content)) {
			if ((wn_fmt_tag = wn_fmt_add (wn_fmt, wn_fmt_type2int (type), (param && strcmp (param, WN_FMT_CHAR_NULL)) ? param : NULL, (content && strcmp (content, WN_FMT_CHAR_NULL)) ? content : NULL)) && id)
				wn_fmt_tag->blocknr = atoi (id);
			if (type) { free (type); type = NULL; }
			if (param) { free (param); param = NULL; }
			if (content) { free (content); content = NULL; }
			if (id) { free (id); id = NULL; }	
		} else {
			wn_report_error (WE_REGE, "wn_fmt_load: XML definition too small at line %i", count);
			fclose (f);
			wn_fmt_free (wn_fmt);
			return NULL;
		}
	}


	fclose (f);
	return wn_fmt;
}

wn_fmt_t *wn_fmt_load (char *fname) {
	return _wn_fmt_load (fname, 0);
}
