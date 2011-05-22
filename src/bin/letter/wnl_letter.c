#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>
#include <wn.h>
#include "wnl_letter.h"

wnl_letter_t *first_letter = NULL;
wnl_letter_t *last_letter = NULL;
int num_letters = 0;

wnl_letter_t *wnl_letter_init (int uid, char *email) {
	wnl_letter_t *the_letter;

	if (!uid || !email) {
		wn_report_error (WE_PARAM, "wnl_letter_init: No uid or email given");
		return NULL;
	}

	if ((the_letter = wnl_letter_by_uid (uid))) 
		return the_letter;
		
	the_letter = (wnl_letter_t *) malloc (sizeof (wnl_letter_t));
	the_letter->uid = uid;
	the_letter->email = strdup (email);
	the_letter->first_body = NULL;
	the_letter->last_body = NULL;
	the_letter->next = NULL;

	if (!first_letter || !last_letter)
		first_letter = the_letter;
	else
		last_letter->next = the_letter;
	last_letter = the_letter;

	num_letters++;
	return last_letter;
}

int wnl_letter_delete (int uid) {
	wnl_letter_t *letter;
	wnl_letter_t *doomed;
	wnl_body_t *body1, *body2;

	if (first_letter && (first_letter->uid == uid)) {
		doomed = first_letter;

		if (!first_letter->next)
			last_letter = NULL;

		first_letter = first_letter->next;

		free (doomed->email);
		body1 = doomed->first_body;
		while (body1) {
			body2 = body1->next;
			if (body1->title) free (body1->title);
			if (body1->url) free (body1->url);
			if (body1->data) free (body1->data);
			if (body1->words) free (body1->words);
			if (body1->bfilter) free (body1->bfilter);
			free (body1);
			body1 = body2;
		}

		free (doomed);
		doomed = NULL;

		num_letters--;
		return 1;
	}

	for (letter = first_letter; letter->next; letter = letter->next) {
		if (letter->next->uid == uid) {
			doomed = letter->next;
			if (last_letter && (last_letter->uid == uid))
				last_letter = letter;

			letter->next = letter->next->next;

			free (doomed->email);
			body1 = doomed->first_body;
			while (body1) {
				body2 = body1->next;
				if (body1->title) free (body1->title);
				if (body1->url) free (body1->url);
				if (body1->data) free (body1->data);
				if (body1->words) free (body1->words);
				if (body1->bfilter) free (body1->bfilter);
				free (body1);
				body1 = body2;
			}
	
			free (doomed);
			doomed = NULL;

			num_letters--;
			return 1;
		}
	}

	return 1;
}

wnl_body_t *wnl_letter_add (wnl_letter_t *letter, int did, char *title, char *url, int filter, char *words, char *bfilter) {
	wnl_body_t *the_body;

	if (!letter || !did) {
		wn_report_error (WE_PARAM, "wnl_letter_add: No letter or did given");
		return NULL;
	}

	if ((the_body = wnl_letter_body_by_did (letter, did))) 
		return the_body;
		
	the_body = (wnl_body_t *) malloc (sizeof (wnl_body_t));
	the_body->did = did;
	the_body->title = strdup (title);
	the_body->url = strdup (url);
	the_body->filter = filter;
	the_body->words = strdup (words);
	the_body->bfilter = strdup (bfilter);
	the_body->data = NULL;
	the_body->next = NULL;

	if (!letter->first_body || !letter->last_body)
		letter->first_body = the_body;
	else
		letter->last_body->next = the_body;
	letter->last_body = the_body;

	return letter->last_body;
}

wnl_body_t *wnl_letter_body_by_did (wnl_letter_t *letter, int did) {
	wnl_body_t *body;

	if (!letter) return NULL;

	for (body = letter->first_body; body; body = body->next) {
		if (body->did == did)
			return body;
	}
	
	return NULL;
}

wnl_letter_t *wnl_letter_by_uid (int uid) {
	wnl_letter_t *letter;

	for (letter = first_letter; letter; letter = letter->next) {
		if (letter->uid == uid)
			return letter;
	}

	return NULL;
}

wnl_letter_t *wnl_letter_first () {
	return first_letter;
}

int wnl_letter_count () {
	return num_letters;
}
