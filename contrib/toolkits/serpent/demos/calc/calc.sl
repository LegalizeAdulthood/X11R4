#include "saw.ill"

||| 

/* This is the slang program which constructs a calculator It has one vc
and multiple objects.  The three variables carry the accumulator, the
decimal position and the current operator.  */


#define calculate(number) if (decimalpos = 0) then\
              display := 10 * display + number;\
           else if (decimalpos = -1) then\
              display := number;\
              decimalpos := 0;\
           else\
              display := display + number / (10 ** \
                    decimalpos);\
              decimalpos :=decimalpos + 1;\
           endif;

#define set_operator(operator)\
              op := operator;\
              decimalpos := -1;\
              accumulator := display;

#define operate(symbol) decimalpos := -1;\
               accumulator := accumulator symbol display;\
               display := accumulator;


   VARIABLES:

   display : 0;
   accumulator : 0;
   decimalpos : 0;
   op : "";
/*
   The variable decimalpos serves three purposes.  
      If it is 0 then any numbers entered into the display are
        appended to the right of the current display but to the left of 
        the decimal point.
      if it is -1 then any number entered into the display erases the current
            display and sets display to number entered
      if it is positive then it represents the position to the right of
            the decimal point*/

   OBJECTS:


        main_background:     form_widget
        {ATTRIBUTES:
           width:  300;
           height: 600;
           background_color:  "white";
         }   

/*-----------------------------------------------------------------------
|    Object: display on top of calculator
|
---------------------------------------------------------------------*/

        display_widget: label_widget
        {ATTRIBUTES:

          parent: main_background;
          vert_distance: 20;
          horiz_distance: 50;
          height: 40;
          width: 200;
          justify: 2;
          label_text: display;
          border_width: 3;
          border_color: "black";
          background_color: "white";
         }

/*------------------------------------------------------------------------
|       Object: display  digit 7  
------------------------------------------------------------------------*/

	digit7: command_widget
	{ATTRIBUTES:
           parent: main_background;  
	   label_text: "7";
	   width: 50;
	   height: 50;
           vert_distance: 100;
           horiz_distance: 20;
	   justify: 1;

	   background_color: "white";
	   foreground_color: "black";
           border_color: "black";
           border_width: 2;


         METHODS:
          notify: {
             calculate (7)
          }
	}

/*------------------------------------------------------------------------
|       Object: display  digit 7  
------------------------------------------------------------------------*/

	digit8: command_widget
	{ATTRIBUTES:
           parent: main_background;  
	   label_text: "8";
	   width: 50;
	   height: 50;
	   justify: 1;
           vert_distance: 100;
           horiz_distance: 90;

	   background_color: "white";
	   foreground_color: "black";
           border_color: "black";
           border_width: 2;


         METHODS:
          notify: {
            calculate (8)
          }
	}

/*------------------------------------------------------------------------
|       Object: display  digit 7  
------------------------------------------------------------------------*/

	digit9: command_widget
	{ATTRIBUTES:
           parent: main_background;  
	   label_text: "9";
	   width: 50;
	   height: 50;
	   justify: 1;
           vert_distance: 100;
           horiz_distance: 160;

	   background_color: "white";
	   foreground_color: "black";
           border_color: "black";
           border_width: 2;


         METHODS:
          notify: {
            calculate (9)
          }
	}

/*------------------------------------------------------------------------
|       Object: display  digit 7  
------------------------------------------------------------------------*/

	digit4: command_widget
	{ATTRIBUTES:
           parent: main_background;  
	   label_text: "4";
	   width: 50;
	   height: 50;
	   justify: 1;
           vert_distance: 170;
           horiz_distance: 20;

	   background_color: "white";
	   foreground_color: "black";
           border_color: "black";
           border_width: 2;


         METHODS:
          notify: {
           calculate (4)
          }
	}

/*------------------------------------------------------------------------
|       Object: display  digit 5  
------------------------------------------------------------------------*/
	digit5: command_widget
	{ATTRIBUTES:
           parent: main_background;  
	   label_text: "5";
	   width: 50;
	   height: 50;
	   justify: 1;
           vert_distance: 170;
           horiz_distance: 90;

	   background_color: "white";
	   foreground_color: "black";
           border_color: "black";
           border_width: 2;


         METHODS:
          notify: {
           calculate (5)
          }
	}

	digit6: command_widget
	{ATTRIBUTES:
           parent: main_background;  
	   label_text: "6";
	   width: 50;
	   height: 50;
	   justify: 1;
           vert_distance: 170;
           horiz_distance: 160;

	   background_color: "white";
	   foreground_color: "black";
           border_color: "black";
           border_width: 2;


         METHODS:
          notify: {
           calculate (6)
          }
	}

	digit1: command_widget
	{ATTRIBUTES:
           parent: main_background;  
	   label_text: "1";
	   width: 50;
	   height: 50;
	   justify: 1;
           vert_distance: 240;
           horiz_distance: 20;

	   background_color: "white";
	   foreground_color: "black";
           border_color: "black";
           border_width: 2;


         METHODS:
          notify: {
           calculate (1)
          }
	}

	digit2: command_widget
	{ATTRIBUTES:
           parent: main_background;  
	   label_text: "2";
	   width: 50;
	   height: 50;
	   justify: 1;
           vert_distance: 240;
           horiz_distance: 90;

	   background_color: "white";
	   foreground_color: "black";
           border_color: "black";
           border_width: 2;


         METHODS:
          notify: {
            calculate (2)
          }
	}

	digit3: command_widget
	{ATTRIBUTES:
           parent: main_background;  
	   label_text: "3";
	   width: 50;
	   height: 50;
	   justify: 1;
           vert_distance: 240;
           horiz_distance: 160;

	   background_color: "white";
	   foreground_color: "black";
           border_color: "black";
           border_width: 2;


         METHODS:
          notify: {
           calculate (3)
          }
	}

	digit0: command_widget
	{ATTRIBUTES:
           parent: main_background;  
	   label_text: "0";
	   width: 50;
	   height: 50;
	   justify: 1;
           vert_distance: 310;
           horiz_distance: 90;

	   background_color: "white";
	   foreground_color: "black";
           border_color: "black";
           border_width: 2;


         METHODS:
          notify: {
            calculate (0)
          }
	}

	cleardisplay: command_widget
	{ATTRIBUTES:
           parent: main_background;  
	   label_text: "CE";
	   width: 50;
	   height: 50;
	   justify: 1;
           vert_distance: 310;
           horiz_distance: 20;

	   background_color: "white";
	   foreground_color: "black";
           border_color: "black";
           border_width: 2;


         METHODS:
          notify: {

          display := 0;
          decimalpos := 1;
          }
	}

	decimal: command_widget
	{ATTRIBUTES:
           parent: main_background;  
	   label_text: ".";
	   width: 50;
	   height: 50;
	   justify: 1;
           vert_distance: 310;
           horiz_distance: 160;

	   background_color: "white";
	   foreground_color: "black";
           border_color: "black";
           border_width: 2;


         METHODS:
          notify: {

         if(decimalpos = 0) then
            decimalpos := 1;
         endif;

          }
	}

	clearaccum: command_widget
	{ATTRIBUTES:
           parent: main_background;  
	   label_text: "C";
	   width: 30;
	   height: 30;
	   justify: 1;
           vert_distance: 100;
           horiz_distance: 230;

	   background_color: "white";
	   foreground_color: "black";
           border_color: "black";
           border_width: 2;


         METHODS:
          notify: {

            display := 0;
            decimalpos := 0;
            accumulator := 0;


          }
	}

	divide: command_widget
	{ATTRIBUTES:
           parent: main_background;  
	   label_text: "DIV";
	   width: 30;
	   height: 30;
	   justify: 1;
           vert_distance: 140;
           horiz_distance: 230;

	   background_color: "white";
	   foreground_color: "black";
           border_color: "black";
           border_width: 2;


         METHODS:
          notify: {
           set_operator("div")
          }
	}

	multiply: command_widget
	{ATTRIBUTES:
           parent: main_background;  
	   label_text: "X";
	   width: 30;
	   height: 30;
	   justify: 1;
           vert_distance: 180;
           horiz_distance: 230;

	   background_color: "white";
	   foreground_color: "black";
           border_color: "black";
           border_width: 2;


         METHODS:
          notify: {
            set_operator("mult")
          }
	}

	subtract: command_widget
	{ATTRIBUTES:
           parent: main_background;  
	   label_text: "-";
	   width: 30;
	   height: 30;
	   justify: 1;
           vert_distance: 260;
           horiz_distance: 230;

	   background_color: "white";
	   foreground_color: "black";
           border_color: "black";
           border_width: 2;


         METHODS:
          notify: {
            set_operator ("sub")
          }
	}

	add: command_widget
	{ATTRIBUTES:
           parent: main_background;  
	   label_text: "+";
	   width: 30;
	   height: 30;
	   justify: 1;
           vert_distance: 220;
           horiz_distance: 230;

	   background_color: "white";
	   foreground_color: "black";
           border_color: "black";
           border_width: 2;


         METHODS:
          notify: {
            set_operator("add")
          }
	}

	equal: command_widget
	{ATTRIBUTES:
           parent: main_background;  
	   label_text: "=";
	   width: 30;
	   height: 40;
	   justify: 1;
           vert_distance: 320;
           horiz_distance: 230;

	   background_color: "white";
	   foreground_color: "black";
           border_color: "black";
           border_width: 2;


         METHODS:
          notify: {
            If (op = "div") then
               operate (/)
            else if (op = "mult") then
               operate (*)
            else if (op = "add") then
               operate (+)
            else if (op = "sub") then
               operate (-)
            endif;
          }
	}

