/*--------------------------------------------------------------------------*\
|  Macro: button
|
|  Description:
|    Creates a button object.
\*--------------------------------------------------------------------------*/
#define button(name, parent_id, vertical_distance, down_from, horizontal_distance, away_from, text, method)\
name: command_widget {\
ATTRIBUTES:\
/*\
   Variable attributes.\
*/\
    parent: parent_id;\
    vert_distance: vertical_distance;\
    from_vert: down_from;\
    horiz_distance: horizontal_distance;\
    from_horiz: away_from;\
    label_text: text;\
/*\
   Constant attributes.\
*/\
    width: 50;\
    height: 20;\
  METHODS:\
    notify: method\
}                               /*\ end button object                  */

/*--------------------------------------------------------------------------*\
|  Macro: big_button
|
|  Description:
|    Creates a slightly larger button object.
\*--------------------------------------------------------------------------*/
#define big_button(name, parent_id, vertical_distance, down_from, horizontal_distance, away_from, text, method)\
name: command_widget {\
ATTRIBUTES:\
/*\
   Variable attributes.\
*/\
    parent: parent_id;\
    vert_distance: vertical_distance;\
    from_vert: down_from;\
    horiz_distance: horizontal_distance;\
    from_horiz: away_from;\
    label_text: text;\
/*\
   Constant attributes.\
*/\
    width: 60;\
    height: 20;\
  METHODS:\
    notify: method\
}                               /*\ end big_button object                  */
