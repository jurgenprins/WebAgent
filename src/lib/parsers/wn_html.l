%{
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../wn_err.h"
#include "../wn_fmt.h"
#include "../wn_url.h"
#include "wn_html.h"

wn_url_t *g_url;
wn_fmt_t *g_fmt;
wn_fmt_tag_t *g_tag;
char *g_str;
int g_offset = 0;

#define YY_INPUT(buf,result,max_size) \
{ \
	int n = (strlen (&(g_url->data[g_offset])) > max_size) ? \
	         max_size : strlen (&(g_url->data[g_offset])); \
		\
	result = (n > 0) ? strncpy (buf, &(g_url->data[g_offset]), n), \
                     g_offset += n, \
	                   n \
	                 : YY_NULL; \
}

%}

ws [ \t\n\r]*
text [^<]+
assign ={ws}[\"\']*
atext [^ <>\"\']+
href "<"{ws}[aA]{ws}[hH][rR][eE][fF]{ws}
img	"<"{ws}[iI][mM][gG]{ws}
frame "<"{ws}[fF][rR][aA][mM][eE]{ws}
title {ws}[Tt][Ii][Tt][Ll][eE]{ws}
script {ws}[sS][cC][rR][iI][pP][tT]{ws}
src {ws}[sS][rR][cC]{ws}
metaurl ;{ws}[uU][rR][lL]
table {ws}[tT][aA][bB][lL][eE]{ws}
par {ws}[pP]{ws}
blockquote {ws}[bB][lL][oO][cC][kK][qQ][uU][oO][tT][eE]{ws}
ul {ws}[uU][lL]{ws}
ol {ws}[oO][lL]{ws}
dl {ws}[dD][lL]{ws}
div {ws}[dD][iI][vV]{ws}
hr {ws}[hH][rR]{ws}
center {ws}[cC][eE][nN][tT][eE][rR]{ws}

%s LINK
%s LINKVAL
%s LINKIMG
%s LINKCLOSE
%s METALINK
%s METALINKCLOSE
%s IMG
%s IMGSRC
%s IMGCLOSE
%s FRAME
%s FRAMESRC
%s FRAMECLOSE
%x COMMENT
%x SCRIPT
%x TITLE
%s TEXT

%%

{href}{assign}							{ BEGIN LINK; }
<LINK>{atext}								{ BEGIN LINKVAL; 
															g_tag = wn_fmt_add (g_fmt, WN_FMT_LINK, g_str = wn_urlify (g_url, yytext), NULL); 
															free (g_str);
														}
<LINKVAL>">"{text}					{ BEGIN LINKCLOSE; 
															if (g_tag) 
																wn_fmt_expand (g_tag, NULL, yytext + 1); 
														}
<LINKVAL>">"{img}						{ BEGIN LINKIMG;
															wn_fmt_retract (g_fmt);
														}
<LINKIMG>">"								{ BEGIN LINKCLOSE; }
<LINKCLOSE>"<"							{ BEGIN 0; }

{img}												{ BEGIN IMG; }
<IMG>{src}{assign}					{ BEGIN IMGSRC; }
<IMGSRC>{atext}							{ BEGIN IMGCLOSE; 
															wn_fmt_add (g_fmt, WN_FMT_IMAGE, g_str = wn_urlify (g_url, yytext), NULL); 
															free (g_str);
														}
<IMGSRC,IMGCLOSE>">"				{ BEGIN 0; }

{metaurl}{assign}						{ BEGIN METALINK; }
<METALINK>{atext}						{ BEGIN METALINKCLOSE;
														  g_tag = wn_fmt_add (g_fmt, WN_FMT_LINK, g_str = wn_urlify (g_url, yytext), NULL);
														  free (g_str);
														}
<METALINKCLOSE>">"					{ BEGIN 0; }

{frame}											{ BEGIN FRAME; }
<FRAME>{src}{assign}				{ BEGIN FRAMESRC; }
<FRAMESRC>{atext}						{ BEGIN FRAMECLOSE; 
															wn_url_addframe (g_url, yytext); 
														}
<FRAMESRC,FRAMECLOSE>">"		{ BEGIN 0; }

"<!--"											{ BEGIN COMMENT; }
<COMMENT>"-->"							{ BEGIN 0; }

"<"{script}									{ BEGIN SCRIPT; }
<SCRIPT>"/"{script}					{ BEGIN 0; }

"<"{title}									{ BEGIN TITLE; }
<TITLE>">"{text}						{
															g_url->title = strdup (yytext + 1);
														}
<TITLE>"/"{title}						{ BEGIN 0; }

">"{text}										{  
															g_tag = wn_fmt_add (g_fmt, WN_FMT_TEXT, NULL, yytext + 1); 
														}
	
"<"{par}										|
"<"{hr}											|
"<"{table}									|
"<"{blockquote}							|
"<"{ul}											|
"<"{ol}											|
"<"{dl}											|
"<"{div}										|
"<"{center}									{ wn_fmt_setblock (g_fmt); }

"/"{table}									|
"/"{blockquote}							|
"/"{ul}											|
"/"{ol}											|
"/"{dl}											|
"/"{div}										|
"/"{center}									{ wn_fmt_unsetblock (g_fmt); }

%%
int yywrap () {
	return 1;
}

int wn_html2fmt (wn_url_t *url, wn_fmt_t *fmt)
{
	if (!url || !fmt) {
		wn_report_error (WE_PARAM, "html2fmt: too few parameters");
		return 0;
	}

	g_url = url;
	g_fmt = fmt;
	g_offset = 0;

	yyout = fopen ("/dev/null", "w");
	yylex();

	return 1;
}
