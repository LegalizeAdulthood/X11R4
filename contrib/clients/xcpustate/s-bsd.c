/* System dependent file for BSD systems */
/* Chris Siebenmann, CSRI, University of Toronto <cks@white.toronto.edu> */
/* LINTLIBRARY */
#include <sys/param.h>
#include <sys/dk.h>
#include <nlist.h>

extern char *xmalloc(/* int nbytes */);

extern int open(), read();
extern long lseek();

long	cp_time[CPUSTATES];
long	cp_old[CPUSTATES];
int	kmem;			/* file descriptor of /dev/kmem. */
struct nlist	nl[] = {
#define	X_CP_TIME	0
	{ "_cp_time" },
	{ "" },
};

/* Called at the beginning to inquire how many bars are needed. */
int
num_bars()
{
    return 1;
}

/* Called after num_bars to ask for the bar names */
/* ARGSUSED */
char **
label_bars(nbars)
{
    static char *name[] = {"CPU states"};

    return name;
}

/* 
 *  Called after the bars are created to perform any machine dependent
 *  initializations.
 */
/* ARGSUSED */
void
init_bars(nbars)
int nbars;
{
    
    if ((kmem = open("/dev/kmem", 0)) < 0) {
	    perror("/dev/kmem");
	    exit(1);
    }
    (void)nlist("/vmunix", nl);
    if (lseek(kmem, (long) nl[X_CP_TIME].n_value, 0) !=
	(long) nl[X_CP_TIME].n_value)
	    perror("lseek");
    if (read(kmem, (char *) cp_old, sizeof(cp_old)) !=
	sizeof(cp_old))
	    perror("read");
}

/* 
 *  This procedure gets called every interval to compute and display the
 *  bars. It should call draw_bar() with the bar number, the array of
 *  integer values to display in the bar, and the number of values in
 *  the array.
 */
/* ARGSUSED */
void
display_bars(nbars)
{
	int	states[CPUSTATES];
	int	nstates;
	int	i;
	extern void draw_bar(/*int bar_num, int *states, int num_states*/);
	
	if (lseek(kmem, (long) nl[X_CP_TIME].n_value, 0) !=
	    (long) nl[X_CP_TIME].n_value)
		perror("lseek");
	if (read(kmem, (char *) cp_time, sizeof(cp_time)) !=
	    sizeof(cp_time))
		perror("read");
	
	
#define delta(cpustate) ((int) (cp_time[(cpustate)] - cp_old[(cpustate)]))

	nstates = 0;
	states[nstates++] = delta(CP_IDLE);
	states[nstates++] = delta(CP_USER);
	states[nstates++] = delta(CP_NICE);
	states[nstates++] = delta(CP_SYS);
	draw_bar(0, states, nstates);
	for (i = 0; i < CPUSTATES; i ++)
		cp_old[i] = cp_time[i];
}
