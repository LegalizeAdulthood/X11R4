/* 
 *  A simple insertion-only hash routine for managing a single hash
 *  table. Heavily modified and tuned from a piece of code by Ron Unrau
 *  (ron@godzilla.ele.toronto.edu). Thanks Ron! It could be easily
 *  generalized to do multiple hashtables simply by having InitHash
 *  return a pointer to HashTable rather than maintain state in static
 *  variables, and have the Insert and Search routines take that
 *  pointer. Ron's hash function is pretty good for strings - it seems
 *  to stand up well to small dense tables. 
 */
/*
	Compile with -DTEST to make it a standalone test program.
	Compile with -DPRINTHASH for information on the hash function
	Compile with -DHSTATS for information on collisions
 */
#ifndef lint
static char *rcsid = "$Header: hash.c,v 1.3 89/07/02 15:58:22 moraes Exp $";
#endif

#include <stdio.h>
#ifdef XPIC
#include <X11/Xos.h>
#endif

#define STREQ(s1, s2)	((*s1)==(*s2) && strcmp(s1, s2) == 0)
/* A hash table smaller than this is pretty worthless */
#define MINSIZE 17
#define MAXSTR 128

extern char *malloc();
extern char *realloc();
extern char *calloc();

/*
 *  The calling routines must malloc space for the key and datum - the
 *  hash table just stores pointers
 */
typedef struct {
	char *key;
	char *datum;
} HashRec;

typedef struct {
	HashRec *hashtable;
	int hashsize;
	int hashprobe;
} HashTable;

static HashRec *hashtable = NULL; 
static int hashsize;
static int hashprobe;
extern char *calloc();
extern char *malloc();

static hashindex (s)
register char *s;
{
	register int shift = 0;
	register int index = 0;

#ifdef PRINTHASH
	(void) fprintf(stderr, "\"%s\" hashes to ", s);
#endif
	while (*s != '\0') {
		index ^= *(s++) << shift;
		shift = (shift == 0) ? 8 : 0;
	}

#ifdef PRINTHASH
	(void) fprintf(stderr, "%d    ", index);
#endif
	return index;
}


/* size should be a prime number */
HashInit(size)
{
	hashsize = (size < MINSIZE) ? MINSIZE : size;
	hashprobe = hashsize / 3;
	if (hashprobe * 3 == hashsize)
		hashprobe--;
	if (hashprobe % 2 == 0)
		hashprobe--;
#ifdef HSTATS
	(void) fprintf(stderr, "size = %d      probe = %d\n", hashsize, hashprobe);
#endif
	if (hashtable)
		hashtable = (HashRec *) realloc((char *) hashtable, 
		 (unsigned) (hashsize * sizeof(HashRec)));
	else 
		hashtable = (HashRec *) calloc((unsigned) size, sizeof(HashRec));
	if (hashtable)
		return;
	else {
		(void) fprintf(stderr, "Couldn't allocate %d bytes for hash table\n",
		 size * sizeof(HashRec));
		exit(-1);
	}
}

/*
 *  Will insert datum keyed by key in the table if key isn't already in
 *  the table - otherwise, it will replace teh datum already in the
 *  table for key with the new datum. It doesn't bother to free the old
 *  datum, so this is a search of garbage, if the stuff was malloced.
 *  Maybe we should return the old datum, so the caller can free it?
 */
HashInsert (key, datum)
char *key;
char *datum;
{
	register int hashval;
	register int index;
	register HashRec *hashptr = hashtable;
	register int countdown = hashsize;

	hashval = hashindex (key);
	index = hashval % hashsize;
	hashptr += index;
	while (hashptr->key && !STREQ(hashptr->key, key) && --countdown) {
		index = (index + hashprobe) % hashsize;
		hashptr = hashtable + index;
	}
	if (!hashptr->key) {
		hashptr->key = key;
		hashptr->datum = datum;
#ifdef HSTATS
		(void) fprintf(stderr, "%d probes\n", hashsize - countdown);
#endif
	} else if (countdown) {
		/* replace current value with new one */
		hashptr->datum = datum;
#ifdef HSTATS
		(void) fprintf(stderr, "%d probes - replacing\n", hashsize-countdown);
#endif
	} else {
		(void) fprintf(stderr, "Can't insert in hash table - help\n");
		exit(1);
	}
}

char *HashSearch (key)
char *key;
{
	register int hashval;
 	register int index;
	register HashRec *hashptr = hashtable;
	register int countdown = hashsize;

	hashval = hashindex (key);
	index = hashval % hashsize;

	hashptr += index;
	while (hashptr->key && !STREQ (hashptr->key, key) && --countdown) {
		index = (index + hashprobe) % hashsize;
		hashptr = hashtable + index;
	}
#ifdef HSTATS
	(void) fprintf(stderr, "%d probes - %s\n", hashsize - countdown, 
	 (hashptr->key && countdown) ? "found" : "not found");
#endif
	if (hashptr->key && countdown)
		return (hashptr->datum);
	else
		return((char *) 0);
}

char *strsave(s)
char *s;
{
	char *s1 = malloc((unsigned) (strlen(s) + 1));

	if (s1)
		(void) strcpy(s1, s);
	return(s1);
}


#ifdef TEST
#define prompt() if(prompt_p) printf(":"); else ;
main()
{
 	char s1[8], s2[MAXSTR], s3[MAXSTR];
	char *result;
	int prompt_p;

	HashInit(128);
	prompt_p = isatty(fileno(stdin));
	prompt();
	while(scanf(" %s %s", s1, s2) == 2) {
		switch (*s1) {
		case 'a':
			scanf(" %s", s3);
			HashInsert(strsave(s2), strsave(s3));
			break;
		case 's':
			if (result = HashSearch(s2))
				printf("found %s\n", result);
			else
				printf("no match\n");
			break;
		case 'i':
			HashInit(atoi(s2));
			break;
		default:
			printf("'a string1 string2' inserts datum (string2) keyed by string1\n");
			printf("'s string1' prints the datum associated with string1 if any\n");
			break;
		}
		prompt();
	}
}
#endif
