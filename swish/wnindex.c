#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_SIZE 10240

typedef struct word_t {
	int count;
	char *word;
	struct word_t *next;
} word_t;

word_t *firstword = NULL;
word_t *lastword = NULL;

void main (int argc, char *argv[]) {
	FILE *f;
	int count, i, num = 0;
	char *str = (char *) malloc (MAX_SIZE + 1);
	char *wordstr, *rest;
	word_t *word, *newword;

	if (argc < 3) {
		printf ("usage: %s <file> <num> [<stopstring>]\n", argv[0]);
		exit (1);
	}

	if (!(f = fopen (argv[1], "r"))) {
		printf ("could not open %s\n", argv[1]);
		exit (2);
	}

	if ((num = atoi (argv[2])) < 1)
		num = 1;

	for (i = 0; i < num; i++) {
		word = (word_t *) malloc (sizeof (word_t));
		word->count = 0;
		word->word = NULL;
		word->next = NULL;
		if (!firstword || !lastword)
			firstword = word;
		else
			lastword->next = word;
		lastword = word;
	}

	while (fgets (str, MAX_SIZE, f)) {
		if ((str[0] >= 'a') && (str[0] <= 'z')) {
			if ((wordstr = strtok (str, ":")) && (rest = strtok (NULL, ":"))) {
				if ((strlen (wordstr) < 5) || ((argc >= 4) && (strstr (argv[3], wordstr))))
					continue;

				count = 0;
				for (i = 0; rest[i]; i++) {
					if (rest[i] == 32) 
						++count;
				}

				if (count) {
					newword = (word_t *) malloc (sizeof (word_t));
					newword->count = count;
					newword->word = strdup (wordstr);
					newword->next = NULL;

					if (count > firstword->count) {
						newword->next = firstword;
						firstword = newword;
					} else {
						i = 0;
						word = firstword;
						while ((i < num) && (word)) {
							if (word->next) {
								if (count > word->next->count) {
									newword->next = word->next;
									word->next = newword;	
									break;
								}
							} else break;
							i++;
							word = word->next;
						} 
					}
				}
			}
		}
	}
	
	fclose (f);

	i = 0;
	word = firstword;
	while ((i < num) && (word)) {
		printf ("%s\n", word->word);
		i++;
		word = word->next;
	}
}
