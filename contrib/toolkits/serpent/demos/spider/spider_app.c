/*
$Header: /u/04c/mnt/new_integ/demos/spider/RCS/spider_app.c,v 1.2 89/10/11 00:13:04 serpent Exp $
 */

#ifndef LINT
static char rcsid [] = "$Header: /u/04c/mnt/new_integ/demos/spider/RCS/spider_app.c,v 1.2 89/10/11 00:13:04 serpent Exp $";
#endif  LINT

/*--------------------------------------------------------------------------*\
|   
|  Name:  Sensor Site Status Application (sss)
|
|  Description:  
|     Creates a spider chart for two well known correlation centers and n
|     sensors defined in an external data file.
|
$Log:	spider_app.c,v $
 * Revision 1.2  89/10/11  00:13:04  serpent
 * short names: shared_defs.h => shared.h
 * 
 * Revision 1.1  89/08/01  14:58:05  serpent
 * initial_checkin
 * 
 * Revision 1.3  89/03/15  17:28:57  bmc
 * Fixed most of the things that lint(1) complains about.
 * 
 * Revision 1.2  89/03/08  13:20:31  bmc
 * Now flushes transactions every SENSOR_TRANSACTION_THRESHOLD times when
 * adding items to the shared data area.  Also handles user-generated
 * interrupts properly.
 * 
 * Revision 1.1  89/03/08  10:26:53  bmc
 * Initial revision
 *
\*---------------------- Copyright 1987 CMU --------------------------------*/

/*-------------------- Includes --------------------------------------------*/

#include <signal.h>                     /* UNIX signal handling             */
#include <string.h>                     /* C string functions               */
#include "serpent.h"			/* serpent interface definition	    */
#include "spider.h"			/* sapplication data structures	    */
#include "shared.h"		/* defs shared with dialogue        */

/*-------------------- DEBUG VARS --------------------------------------*/

extern int isd_debug;
extern int idd_debug;
extern int iid_debug;
extern int ipc_debug;
extern int ids_debug;
extern int ism_debug;
extern int isc_debug;
extern int ubs_debug;

/*-------------------- Constants -------------------------------------------*/

#define DEBUG1(statement) SERPENT_DEBUG1(sss_debug, statement)
#define DEBUG2(statement) SERPENT_DEBUG2(sss_debug, statement)
#define DEBUG3(statement) SERPENT_DEBUG3(sss_debug, statement)

#define GREEN_STATUS 0
#define YELLOW_STATUS 1
#define RED_STATUS 2

#define SDATA "sdata"
#define MAX_HASH 257
#define SENSOR_TRANSACTION_THRESHOLD 3  /* how many sensors to buffer up    */
                                        /* before committing transaction    */

/*-------------------- Type Definitions ------------------------------------*/

typedef struct {
  id_type self;
} generic_sdd;

/*-------------------- Retained Data ---------------------------------------*/

int sss_debug = 0;              /* debug variable                          */
boolean done = false;		/* main loop condition			    */

/*--------------------------------------------------------------------------*\
|  Routine: sss_handle_interrupt
| 
|  Description:  
|     Routine to handle the interrupt signal.  Very UNIX specific.
\*--------------------------------------------------------------------------*/
void sss_handle_interrupt ()
{
   (void) fprintf (stderr, "sapp.main interrupted.  Exiting.\n");
   (void) fflush (stderr);
   done = true;
}

/*--------------------------------------------------------------------------*\
|  Routine: sss_match_id
| 
|  Description:  
|     Routine to compare an identifier with the id in a shared data record.
\*--------------------------------------------------------------------------*/
int sss_match_id(id, shared_data)
iid_id_type id;
generic_sdd *shared_data;
{ 					/* local type definitions	    */
  set_status(ok);			/* begin 			    */
  DEBUG2(
    (void) fprintf(stderr, "sss_match_id: call.\n");
    (void) fprintf(stderr, "sss_match_id: identifer = %d.\n", id);
    (void) fprintf(stderr, "sss_match_id: identifer = %x.\n", shared_data);
  );

  DEBUG2(
    (void) fprintf(stderr, "sss_match_id:  exiting.\n");
  );
  return (shared_data->self == id);
} 					/* end sss_match_id		    */

/*--------------------------------------------------------------------------*\
|  Routine: sss_hash_id
| 
|  Description:  
|     Internal function which will convert an id used to index
|     in the array of hash lists.
\*--------------------------------------------------------------------------*/
int sss_hash_id(id)
iid_id_type id;
{ 					/* local type definitions	    */
/*
   Initialize.
*/
  set_status(ok);			/* begin 			    */
  DEBUG2(
    (void) fprintf(stderr, "sss_hash_id call.\n");
  );
/*
   Return a value in the right range.
*/
  DEBUG2(
    (void) fprintf(stderr, "sss_hash_id exiting.\n");
  );
  return((int)id % MAX_HASH);
} 					/* end sss_hash_id		    */

/*--------------------------------------------------------------------------*\
|  Test sapp.main
| 
|  Description:  
|     Performs a write operation using ids.
\*--------------------------------------------------------------------------*/
main()
{				/* local variables			    */
  transaction_type transaction;
  id_type id;

  communication_line_sdd *communication_line;
  sensor_sdd *sensor;

  int i;      			/* random counter			    */
  int sensor_count;		/* number of sensors			    */

  HASH id_table;                /* hash of id's of all shared data in view  */
  FILE *fp;			/* file pointer to the data file	    */

  generic_sdd *shared_data_element;	/* generic shared data element ptr  */
/*
   Inititize.
*/
  DEBUG2(
    (void) fprintf(stderr, "sapp.main called.\n");
    (void) fflush(stderr);
  );

  iid_debug = 0;
  ipc_debug = 0;
  ids_debug = 0;
  ism_debug = 0;
  isc_debug = 0;
  ubs_debug = 0;

  (void) signal (SIGINT, sss_handle_interrupt);

/*
   Initialize Serpent
*/
  serpent_init (MAIL_BOX, ILL_FILE);
  check_status("sapp.main:  bad status from serpent_init.");
/*
   Create an id hashtable.  This table contains the ids of all the shared
   data elements in the "view" at any given time.
*/
  id_table = make_hashtable(MAX_HASH, sss_hash_id, sss_match_id);
  check_null(
    id_table,
    "dea.main:  out of memory.\n",
    out_of_memory
  );
/*
   Open the data file.
*/
  fp = fopen(SDATA, "r");
  DEBUG3(
    (void) fprintf(stderr, "sapp.main: fp = %x.\n", fp);
  );
  check_null(
    fp,
    "sapp.main: could not open ill file.\n",
    system_operation_failed
  );
/*
   Get the number of sensors from the data file.
*/
  (void) fscanf(fp, "%d", &sensor_count);
  DEBUG3(
    (void) fprintf(stderr, "sapp.main: sensor_count = %d.\n", sensor_count);
  );
/*
   Start a transaction.  Then, read in each sensor and corresponding 
   communication lines and put into shared data.  
*/

  transaction = start_transaction();
  check_status("sapp.main:  bad status from start_transaction.");
  i = 0;
  while (i < sensor_count) {		/* while sensor's left		    */

    i++;
/*
   Create shared data record.
*/
    sensor = (sensor_sdd *)make_node(sizeof(sensor_sdd));
    check_null(
      sensor,
      "sapp.main: out of memory during make_node sensor_sdd.\n",
      out_of_memory
    );
/*
   Read in sensor data into shared data record.  Note: '&' address operators
   are omitted from certain structure elements because they are arrays, and
   are already passed by reference.
*/
    (void) fscanf(
      fp, 
      "%s%d%s%s%s%s", 
       (sensor->site_abbr), 
      &(sensor->status), 
       (sensor->site),
       (sensor->last_message),
       (sensor->rfo),
       (sensor->etro)
    );

    DEBUG3(
      (void) fprintf(
        stderr,
        "sapp.main: sensor name = %s.\n",
        sensor->site_abbr);
      (void) fprintf(
        stderr,
        "sapp.main: sensor status = %d.\n",
        sensor->status);
      (void) fprintf(stderr, "sapp.main: sensor site = %s.\n", sensor->site);
      (void) fprintf(
        stderr, 
        "sapp.main: sensor last message = %s.\n", 
        sensor->last_message
      );
      (void) fprintf(stderr, "sapp.main: sensor rfo = %s.\n", sensor->rfo);
      (void) fprintf(stderr, "sapp.main: sensor etro = %s.\n", sensor->etro);
    );
/*
   Put sensor into shared data and add to id table.
*/
    sensor->self = add_shared_data(
      transaction, 
      "sensor_sdd", 
      NULL, 
      (caddr_t) NULL
    );
    check_status("sapp.main:  status from add_shared_data.");

    put_shared_data(
      transaction, 
      sensor->self,
      "sensor_sdd", 
      NULL, 
      (caddr_t) sensor
    );
    check_status("sapp.main:  status from put_shared_data.");

    add_to_hashtable(id_table, sensor, sensor->self);
/*
   Create communication line shared data element.
*/   
    communication_line = (communication_line_sdd *)make_node(
      sizeof(communication_line_sdd)
    );
    check_null(
      communication_line,
      "sapp.main: out of memory during make_node communication_line_sdd.\n",
      out_of_memory
    );
/*
   Read in communication line data into shared data element.  Note: '&' 
   operator is omitted from the 'etro' structure field since it is an array,
   and is already passed by reference.
*/
    (void) fscanf(
      fp, 
      "%d%s", 
      &(communication_line->status), 
       (communication_line->etro)
    );

    DEBUG3(
      (void) fprintf(
        stderr, 
        "sapp.main: communication_line status = %d.\n", 
        communication_line->status
      );
      (void) fprintf(
        stderr, 
        "sapp.main: communication_line etro = %s.\n", 
        communication_line->etro
      );
    );
/*
   Set from_sensor and to_cc fields.
*/
    communication_line->from_sensor = sensor->self;
    communication_line->to_cc = CMC_CODE;
/*
   Put communcation line into shared data and add to id table.
*/
    communication_line->self = add_shared_data(
      transaction, 
      "communication_line_sdd", 
      NULL, 
      (caddr_t) NULL
    );
    check_status("sapp.main:  status from add_shared_data.");

    put_shared_data(
      transaction,
      communication_line->self, 
      "communication_line_sdd", 
      NULL, 
      (caddr_t) communication_line
    );
    check_status("sapp.main:  status from put_shared_data.");

    add_to_hashtable(
      id_table, 
      communication_line, 
      communication_line->self
    );
/*
   Create communication line shared data element.
*/   
    communication_line = (communication_line_sdd *)make_node(
      sizeof(communication_line_sdd)
    );
    check_null(
      communication_line,
      "sapp.main: out of memory during make_node communication_line_sdd.\n",
      out_of_memory
    );
/*
   Read in communication line data into shared data element.  Note: '&' 
   operator is omitted from the 'etro' structure field since it is an array,
   and is already passed by reference.
*/
    (void) fscanf(
      fp, 
      "%d%s", 
      &(communication_line->status), 
       (communication_line->etro)
    );

    DEBUG3(
      (void) fprintf(
        stderr, 
        "sapp.main: communication_line status = %d.\n", 
        communication_line->status
      );
      (void) fprintf(
        stderr, 
        "sapp.main: communication_line etro = %s.\n", 
        communication_line->etro
      );
    );
/*
   Set from_sensor and to_cc fields.
*/
    communication_line->from_sensor = sensor->self;
    communication_line->to_cc = OFT_CODE;
/*
   Put communcation line into shared data and add to id table.
*/
    communication_line->self = add_shared_data(
      transaction, 
      "communication_line_sdd", 
      NULL, 
      (caddr_t) communication_line
    );
    check_status("sapp.main:  status from add_shared_data.");

    add_to_hashtable(
      id_table, 
      communication_line, 
      communication_line->self
    );

/*
    If the number of sensors in this current transaction equals the
    threshold value, it's time to commit the transaction so that they'll
    be displayed on the screen.
*/
    if ( (i % SENSOR_TRANSACTION_THRESHOLD) == 0 ) {
      commit_transaction(transaction);
/*
      Start a new transaction for the next group of sensors, unless we're
      at the end of the sensors.
*/
      transaction = start_transaction();
      check_status("sapp.main:  bad status from start_transaction.");
    }

  }                             /* end while file not empty                 */
/*
   Close the data file.
*/
  (void) fclose(fp);
/*
   Commit transaction.
*/
  commit_transaction(transaction);
  check_status("sapp.main:  bad status from commit_transaction.");
  (void) fprintf (stderr, "done with transaction\n");
/*
   Go into wait loop for new transactions.
*/
  while (!done) {		/* do while not done			    */

    transaction = get_transaction();
    check_status("sapp.main:  bad status from get_transaction.");

    id = get_first_changed_element(transaction);
    check_status("reader.main:  bad status from get_first_changed_elem.");
/*
   Process the elements in the transaction.
*/
    while (id != null_id) {	/* while more changed elements		    */

      shared_data_element = (generic_sdd *)get_from_hashtable(id_table, id);
      check_null(
        shared_data_element,
        "sapp.main:  shared data element not found in id_table hashtable.\n",
        not_found
      );

      incorporate_changes(transaction, id, shared_data_element);
      check_status("sapp.main:  bad status from incorporate_changes.");

      id = get_next_changed_element(transaction);
      check_status("sapp.main:  bad status from get_next_changed_element.");


    }				/* end while more changed elements	    */

  }				/* end while not done			    */
/* 
   Cleanup and return.
*/
  serpent_cleanup();
  check_status("sapp.main:  bad status from serpent_cleanup.");

  DEBUG2(
    (void) fprintf(stderr, "sapp.main: exiting.\n");
    (void) fflush(stderr);
  );

  return;
}
