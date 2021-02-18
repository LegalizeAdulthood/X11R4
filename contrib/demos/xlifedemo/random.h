#ifdef HAVE_RANDOM
extern long random();
#define rnd(x)		(random() % (long) (x))
#define seedrnd(x)	(srandom(x))
#else /* ! HAVE_RANDOM */
extern int rand();
#define rnd(x)		(rand() % (x))
#define seedrnd(x)	(srand(x))
#endif /* HAVE_RANDOM */
