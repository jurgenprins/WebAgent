#ifndef WN_ERR_H
#define WN_ERR_H

#define ERR_SIZE 256

#define WE_NONE			0
#define WE_SOCKET		1
#define WE_LOOKUP		2
#define WE_CONNECT		3
#define WE_TIMEOUT 		4
#define WE_PARAM		5
#define WE_FILE			6
#define WE_REGC			7
#define WE_REGE			8
#define WE_EMPTY		9
#define WE_PARSE		10
#define WE_SMTP			11
#define WE_SEND			12
#define WE_SKIP			13
#define WE_RELOC		14
#define WE_HTTP			15
#define WE_CLEAN		16
#define WE_MEM			17
#define WE_DB			18
#define WE_ORPHAN 		19
#define WE_UNKNOWN 		99

typedef struct _err_t {
	int err_no;
	char *err_str;
} err_t;

static err_t errors[] = 
{
  {WE_NONE, "no error occurred"},
  {WE_SOCKET, "there was a problem using the network"},
  {WE_LOOKUP, "the domain didn't seem to exist anymore"},
  {WE_CONNECT, "the remote server was not accepting connections"},
  {WE_TIMEOUT, "the retrieval was aborted because it took too long"},
  {WE_PARAM, "there were internal problems handling the data"},
  {WE_FILE, "there was a problem saving the data"},
  {WE_REGC, "there was a problem analyzing the content"},
  {WE_REGE, "there was a problem analyzing the content"},
  {WE_EMPTY, "there was no content in this document"},
  {WE_PARSE, "no parser available for the mime type"},
  {WE_SMTP, "the mail server did not accept our commands"},
  {WE_SEND, "the talk to the mail server was unexpectedly aborted"},
  {WE_SKIP, "the document was not modified since last visit"},
  {WE_RELOC, "the document can be found elsewhere"},
  {WE_HTTP, "there was a problem talking to the remote server"},
  {WE_CLEAN, "the document was removed manually by the administrator"},
  {WE_MEM, "insufficient memory"},
  {WE_DB, "there was a problem communicating with the database"},
  {WE_ORPHAN, "there were no subscribers for this document"},
  {WE_UNKNOWN, "something unexpected happened"}
};

int wn_report_error (int errno, char *format, ...);
char *wn_get_error ();
int wn_get_errno ();
char *wn_get_errstr (int err_no);
void wn_discard_error ();

#endif
