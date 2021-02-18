	static char rcsid [] = "$Header: /u/04c/mnt/new_integ/demos/concent/RCS/app.c,v 1.1 89/10/10 20:06:13 serpent Exp $";

/*--------------------------------------------------------------------------*\
|   
|  Name:  app.c
|
|  Description:  
|     This C program implements application portion of the Concentration Game
|     demo.
|
|     Its sole responsibility is to present the dialogue with a random
|     set of prizes and a random set of board locations for those prizes.
|     The dialogue uses that information to initialize the playing board.
|     The game playing logic resides in the dialogue, not in the application.
|
|  Component of:
|     Serpent Demos.
|
|  Related files:
|     concent.sdd - Shared data descriptions used within program.
|
|  Project:
|     Serpent
|     User Interface Project
|     Software Engineering Institute
|     Carnegie Mellon University
|     Pittsburgh, PA  15213
|
|  Version: 1.0
|
|  Project Leader:  
|     Len Bass
|     ljb@sei.cmu.edu
|
|  Author:  
|     Brian M. Clapper
|     bmc@sei.cmu.edu  
|
$Log:	app.c,v $
 * Revision 1.1  89/10/10  20:06:13  serpent
 * Initial revision
 * 
 * Revision 1.2  89/10/05  13:31:05  bmc
 * Added some more prizes.  (Okay, so I was bored one night.)
 * 
 * Revision 1.1  89/09/14  14:00:22  bmc
 * Initial revision
 * 
|
\*--------------------------------------------------------------------------*\
|
| Distribution: Approved  for  public  release;  distribution  is  unlimited.
| 
| Copyright  (c)  1989  by  Carnegie  Mellon  University, Pittsburgh, PA. The
| Software Engineering Institute (SEI) is a  federally  funded  research  and
| development  center  established and operated by Carnegie Mellon University
| (CMU). Sponsored by the U.S. Department of Defense under  contract  F19628-
| 85-C-0003,  the SEI is supported by the services and defense agencies, with
| the U.S. Air Force as the executive contracting agent.
| 
| Permission to use, copy, modify, or distribute this software and its  docu-
| mentation  for any purpose and without fee is hereby granted, provided that
| such use, copying, modification  or  distribution  is  not  for  commercial
| advantage and that the above copyright notice appear in all copies and that
| both that copyright notice and this permission notice appear in  supporting
| documentation.  Further, the names Software Engineering Institute or Carne-
| gie Mellon University may not be used in advertising or publicity  pertain-
| ing to distribution of the software without specific, written prior permis-
| sion. CMU makes no claims or representations about the suitability of  this
| software  for  any  purpose.  This software is provided "as is" and no war-
| ranty, express or implied, is made by the SEI or CMU, as  to  the  accuracy
| and  functioning of the program and related program material, nor shall the
| fact of distribution constitute any such  warranty.  No  responsibility  is
| assumed by the SEI or CMU in connection herewith.
|
\*--------------------------------------------------------------------------*/

#include <stdio.h>
#include "serpent.h"
#include "concent.h"
#include "shared.h"		/* definitions shared with dialogue */

/*----------------------------- Macros -------------------------------------*/

#define PRIZES_TO_SHIP	(TOTAL_SQUARES / 2)
#define NUM_BUF_LEN	(sizeof (int))

#define set_random_number_seed() \
    srand48 (time ((long *) NULL) * getpid ())

#define random_integer(lower_bound,upper_bound) \
    (int) ( (lrand48 () % ((upper_bound) - (lower_bound) + 1)) + (lower_bound) )

	/*
	** The following macros are required by one of the header files
	** pulled in by "serpent.h".
	*/

#ifdef DEBUG
#define SERPENT_DEBUG 	(1)
#else
#define SERPENT_DEBUG 	(0)
#endif

#define DEBUG1(statement) SERPENT_DEBUG1 (SERPENT_DEBUG, statement)
#define DEBUG2(statement) SERPENT_DEBUG2 (SERPENT_DEBUG, statement)
#define DEBUG3(statement) SERPENT_DEBUG3 (SERPENT_DEBUG, statement)

/*--------------------------- External References --------------------------*/

extern int getpid ();
extern long lrand48 ();
extern void srand48 ();
extern long time ();

/*--------------------------- Static Global Data ---------------------------*/

			/*
			** The prizes array contains a list of prizes from
			** which to choose.  PRIZES_TO_SHIP prizes are chosen
			** randomly from this list and sent to the dialogue.
			** To add more prizes, simply add additional strings
			** to the array.
			*/
static char *prizes [] =
    {
    "CD Player",
    "Beat-up Chevy",
    "Kids' Toys",
    "5000 Stamps",
    "Videotapes",
    "Bicycle",
    "Hawaii Trip",
    "Prize Sheep",
    "Answering Machine",
    "Car Telephone",
    "Lawn Furniture",
    "An Ancient VAX",
    "Photo Supplies",
    "Serpent Manuals",
    "Rack of Lamb",
    "Electric Guitar",
    "Home Computer",
    "Startup Company",
    "Personal Bodyguard",
    "Halloween Masks",
    "Three Stooges Films",
    "Ton of Fertilizer",
    "Camera Outfit",
    "Expensive Lingerie",
    "Playboy for a Year",
    "New Kitchen",
    "Washer/Dryer",
    "Dog-Grooming Salon",
    "Surplus Underwear",
    "Diamond Ring",
    "Cool Sunglasses",
    "Case of Fine Wine",
    "Ferrari"
    };
#define TOTAL_PRIZES	(sizeof (prizes) / sizeof (char *))

			/*
			** We're going to put the square numbers in this
			** array.  The array will contain pointers to integers,
			** but they'll be cast as pointers to characters so
			** we can use the same shuffle() routine that we use
			** on the prizes array.  (Ugly, but necessary.)
			**
			** The get_square_number() macro makes it easier to
			** access elements of the weirdly cast array.
			*/
static char *square_nums [TOTAL_SQUARES];
static int real_square_nums [TOTAL_SQUARES];

#define get_square_number(number) 	( * ((int *) square_nums [number]) )

/*--------------------------- Local Functions ------------------------------*/

/*--------------------------------------------------------------------------*\
|
| Routine: shuffle
|
| Description: Shuffles (i.e., unsorts) an array of pointers.
|
| Parameters:
|   array  - The array of pointers to shuffle.  The pointers can refer to
|            any type of data, but they should be cast as "char *".
|   length - The number of elements in the array.
\*                                                                          */

static void shuffle (array, length)
char *array [];
int length;
{
#   define SHUFFLE_COUNT  (20)

    int i;
    int current_pos;
    int swap_pos;
    char *temp;


    for (i = 0; i < SHUFFLE_COUNT; i++)
	{
	for (current_pos = 0; current_pos < length; current_pos++)
	    {
			/*
			** Swap the item at this position with
			** another randomly chosen one.
			*/
	    swap_pos            = random_integer (0, length - 1);
	    temp                = array [swap_pos];
	    array [swap_pos]    = array [current_pos];
	    array [current_pos] = temp;
	    }
	}
}

/*--------------------------------------------------------------------------*\
|
| Routine: generate_square_list
|
| Description: Initializes and shuffles the square_nums array of integers.
|   The contents of the array should be accessed via the get_square_number()
|   macro.
|
| Parameters:
|   None.
\*                                                                          */

static void generate_square_list ()
{
    int i;


			/*
			** First generate a list of integers from 1 to
			** TOTAL_SQUARES, then shuffle it.
			*/
    for (i = 0; i < TOTAL_SQUARES; i++)
	{
			/*
			** Initialize the real array of integers, and create
			** the character pointer array from it.
			*/
	real_square_nums [i] = i + 1;
	square_nums [i] = (char *) & real_square_nums [i];
	}

    shuffle (square_nums, TOTAL_SQUARES);
}

/*--------------------------------------------------------------------------*\
|
| Routine: ship_a_square
|
| Description: Ships the data for a single square across the interface to
|   the dialogue.
|
| Parameters:
|   square_num  - The square number to be sent to the dialogue.
|   prize       - The prize name to be associated with square_number.
|   transaction - The transaction currently open to the dialogue.
\*                                                                          */

static void ship_a_square (square_num, prize, transaction)
int square_num;
char *prize;
transaction_type transaction;
{
    square_sdd *square_data;
    id_type id;


			/* Allocate memory for the square data */

    square_data = (square_sdd *) make_node (sizeof (square_sdd));
    if (square_data == (square_sdd *) NULL)
	{
	(void) fprintf (stderr,
	    "application: Unable to make square SDD for prize \"%s\"\n", prize);
	exit (1);
	}

			/* Store the values in the allocated buffer. */

    (void) strncpy (square_data->prize_name,
		    prize,
		    sizeof (square_data->prize_name) - 1);
    square_data->number      = square_num;

			/*
			** Create an element in shared data and initialize
			** it with the contents of the allocated buffer.
			*/
    id = add_shared_data (transaction,
			  "square_sdd",
			  NULL,
			  (caddr_t) square_data);

    check_status ("application: Can't add square to shared data");
}

/*--------------------------------------------------------------------------*\
|
| Routine: ship_prize
|
| Description: Ships the data for a prize across the interface to the
|   the dialogue.
|
| Parameters:
|   prize       - The prize name.
|   square_1    - The first square number to be associated with the prize.
|   square_2    - The second square number to be associated with the prize.
|   transaction - The transaction currently open to the dialogue.
\*                                                                          */

static void ship_prize (prize, square_1, square_2, transaction)
char *prize;
int square_1;
int square_2;
transaction_type transaction;
{
#ifdef DEBUG
    (void) fprintf (stderr,
	"application: Installing prize \"%s\" at square %d and square %d\n",
	prize, square_1, square_2);
#endif DEBUG

    ship_a_square (square_1, prize, transaction);
    ship_a_square (square_2, prize, transaction);
}

/*--------------------------------------------------------------------------*\
|
| Main Routine
|
\*                                                                          */

main ()
{
    transaction_type transaction;
    int prize_count = PRIZES_TO_SHIP - 1;
    int square_num_count = 0;


			/*
			** Initialize Serpent interface and start a 
			** transaction with the dialogue.
			*/
    serpent_init (MAIL_BOX, ILL_FILE);
    check_status ("application:  Unable to initialize Serpent interface.");

    transaction = start_transaction ();
    check_status ("application:  Unable to open transaction with dialogue.");

			/* Generate the prize and square number data. */

    set_random_number_seed ();
    generate_square_list ();
    shuffle (prizes, TOTAL_PRIZES);

			/*
			** Send the prizes over the interface.   Send a
			** wild card pair first.
			*/
    ship_prize (WILD_CARD,
		get_square_number (square_num_count),
		get_square_number (square_num_count + 1),
		transaction);
    square_num_count += 2;

    while (--prize_count >= 0)
	{
	ship_prize (prizes [prize_count],
		    get_square_number (square_num_count),
		    get_square_number (square_num_count + 1),
		    transaction);
	square_num_count += 2;
	}

			/*
			** Commit (and close) the transaction 
			** with the dialogue.
			*/
    commit_transaction (transaction);
    check_status ("application:  Cannot commit transaction.");

    (void) fprintf (stderr, 
	"application: Shipped prizes to dialogue; exiting normally.\n");

    exit (0);
}
