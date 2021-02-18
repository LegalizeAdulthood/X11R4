/*
 *  Any system dependent file must implement these routines. See s-bsd.c
 *  and s-iris4d.c for examples.
 */
extern int num_bars();
extern char **label_bars(/* int nbars */);
extern void init_bars(/* int nbars */);
extern void display_bars(/* int nbars */);
