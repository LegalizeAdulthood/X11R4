#include "saw.ill"

|||

#define MAX_SLIDES 19

#ifdef mc68000

#define DISPLAY_WIDTH 1152
#define DISPLAY_HEIGHT 900

#else

#define DISPLAY_WIDTH 1024
#define DISPLAY_HEIGHT 864

#endif

#define TITLE_FONT "vr-40"
#define BULLET_FONT "vr-40"
#define SUBBULLET_FONT "vr-30"

#define BULLET_OFFSET 50
#define SUBBULLET_OFFSET 100

#define LEFT 0
#define CENTER 1
#define RIGHT 2

#define cnext(fname,fparent,fhoriz) \
fname: command_widget {\
Attributes:\
  parent: fparent;\
  label_text: "Next";\
  font: "vr-40";\
  width: 170;\
  height: 50;\
  vert_distance: DISPLAY_HEIGHT - 100;\
  horiz_distance: fhoriz;\
  border_color: "black";\
 Methods:\
  notify: {\
    if (slide < MAX_SLIDES) then\
      slide := slide + 1;\
    endif;\
  }\
}

#define cprev(fname,fparent,fhoriz) \
fname: command_widget {\
Attributes:\
  parent: fparent;\
  label_text: "Previous";\
  font: "vr-40";\
  width: 170;\
  height: 50;\
  vert_distance: DISPLAY_HEIGHT - 100;\
  horiz_distance: fhoriz;\
  border_color: "black";\
 Methods:\
  notify: {\
    if (slide > 1) then\
      slide := slide - 1;\
    endif;\
  }\
}

#define cform(fname) \
fname: form_widget {\
Attributes:\
  parent: toplevel;\
  height: DISPLAY_HEIGHT;\
  width: DISPLAY_WIDTH;\
}

#define major_heading(fname, fparent, flabel, ffy) \
fname: label_widget {\
Attributes:\
  parent: fparent;\
  font: "vr-40";\
  label_text: flabel;\
  vert_distance: ffy;\
  horiz_distance: 0;\
  border_color: "white";\
  height: 50;\
  width: DISPLAY_WIDTH;\
  justify: CENTER;\
}

#define minor_heading(fname, fparent, flabel, ffy, ffont) \
fname: label_widget {\
Attributes:\
  parent: fparent;\
  font: ffont;\
  label_text: flabel;\
  vert_distance: ffy;\
  horiz_distance: 0;\
  border_color: "white";\
  height: 50;\
  width: DISPLAY_WIDTH;\
  justify: CENTER;\
}

#define title(fname,flabel) \
fname: label_widget {\
Attributes:\
  parent: slide_form;\
  font: TITLE_FONT;\
  label_text: flabel;\
  vert_distance: 150;\
  justify: CENTER;\
  border_color: "white";\
  height: 50;\
  width: DISPLAY_WIDTH;\
}

#define bullet(fname, flabel, ffy)\
fname: label_widget {\
Attributes:\
  parent: slide_form;\
  font: BULLET_FONT;\
  label_text: flabel;\
  vert_distance: ffy;\
  horiz_distance: BULLET_OFFSET;\
  border_color: "white";\
  height: 50;\
}

#define subbullet(fname, flabel, ffy)\
fname: label_widget {\
Attributes:\
  parent: slide_form;\
  font: SUBBULLET_FONT;\
  label_text: flabel;\
  vert_distance: ffy;\
  horiz_distance: SUBBULLET_OFFSET;\
  border_color: "white";\
}

variables:

  main_talk : false;

objects:

    slide_form: form_widget {
      Attributes:
        height: DISPLAY_HEIGHT;
        width: DISPLAY_WIDTH;
    }

vc: title_slide_vc

  creation condition:  (NOT main_talk)

  objects:

    major_heading(j1, slide_form, "Serpent, A User Interface", 200)
    major_heading(j2, slide_form, "Management System", 260)
    minor_heading(j3, slide_form, "Software Engineering Institute", 350, "vr-30")
    minor_heading(j4, slide_form, "Carnegie Mellon University", 400, "vr-20")
    minor_heading(j5, slide_form, "Pittsburgh, PA 15213", 450, "vr-20")
    minor_heading(j6, slide_form, "Sponsored by the U.S. Department of Defense", 600, "vr-25")

    next_button: command_widget {
      Attributes:
        parent: slide_form;
       label_text: "Next";
       font: BULLET_FONT;
       width: 170;
       height: 50;
       vert_distance: DISPLAY_HEIGHT - 100;
       horiz_distance: DISPLAY_WIDTH - 220;
       border_color: "black";
     Methods:
       notify: {
         main_talk := true;
       }
    }

end vc title_slide_vc;


vc: main_talk_vc

  creation condition:  (main_talk)

  variables:

   slide : 1;

  objects:

#if 0
    death_star: pixmap_object {
      Attributes:
        parent: slide_form;
        x: 10;
        y: 10;
        bitmap_file: "death_star.bitmap";
    }
#endif

    cmu_logo: label_widget {
      Attributes:
        parent: slide_form;
        font: "vr-20";
        label_text: "Carnegie Mellon University";
        vert_distance: 	50;
        horiz_distance: 150;
        border_color: "white";
        height: 30;
        justify: LEFT;
    }

    sei_logo: label_widget {
      Attributes:
        parent: slide_form;
        font: "vr-25";
        label_text: "Software Engineering Institute";
        vert_distance: 	80;
        horiz_distance: 150;
        border_color: "white";
        height: 30;
        justify: LEFT;
    }

    bar: label_widget {
      Attributes:
        parent: slide_form;
        label_text: "";
        background_color: "black";
        vert_distance: 	110;
        horiz_distance: 150;
        height: 2;
        width: DISPLAY_WIDTH - 250;
    }

    cnext(next, slide_form, DISPLAY_WIDTH - 220)
    cprev(prev, slide_form, DISPLAY_WIDTH - 420)

/*--------------------- Introduction ---------------------------------------*/

vc: slide1_vc

  creation condition:  (slide = 1)

  objects:

    title(j1, "Introduction")
    bullet(j2, "* Motivation", 250)
    bullet(j3, "* Requirements", 320)
    bullet(j4, "* Serpent UIMS", 390)
    subbullet(j5, "- use as prototyping tool", 460)
    subbullet(j6, "- features to support production", 510)
    subbullet(j7, "- editor", 560)

end vc slide1_vc

/*--------------------- Current User Interface Life Cycle ------------------*/

vc: slide2_vc

  creation condition:  (slide = 2)

  objects:

    title(j1, "Current User Interface Life Cycle")
    bullet(j2, "* Prototype displays using prototyping tool", 250)
    bullet(j3, "* Implement prototype using user interface toolkit", 320)
    bullet(j4, "* Modify implementation to reflect errors in original", 390)
    bullet(j5, "  prototype or user enhancements", 450)
    bullet(j6, "* Best case scenario.  In reality, prototyping not", 520)
    bullet(j7, "  always done.", 580)

end vc slide2_vc

/*--------------------- Problems of Current Approach -----------------------*/

vc: slide3_vc

  creation condition:  (slide = 3)

  objects:

    title(j1, "Problems of Current Approach")
    bullet(j2, "* Prototype is thrown away after serving as model", 250)
    bullet(j3, "  for implementation", 310)
    bullet(j4, "* Mismatch between prototyping tool and", 380)
    bullet(j5, "  implementation toolkit", 440)
    bullet(j6, "* User Interface toolkits are complicated and difficult", 510)
    bullet(j7, "  to use", 570)
    bullet(j8, "* Integration of new input/output media usually", 640)
    bullet(j9, "  requires total re-engineering of application", 700)

end vc slide3_vc

/*--------------------- Incremental Development ----------------------------*/

vc: slide4_vc

  creation condition:  (slide = 4)

  objects:

    title(j1, "Incremental Development")
    bullet(j2, "* Use same tool for prototyping and for production", 250)
    subbullet(j3, "- prototype is not thrown away", 320)
    subbullet(j4, "- constrains prototypes to easily implementable solutions", 370)
    subbullet(j5, "- requires tool that supports both aspects", 420)
    bullet(j6, "* Integrate new media to tool", 490)
    subbullet(j7, "- requires portion of application system be media independent", 560)
    subbullet(j8, "- requires extensible tool", 610)
    subbullet(j9, "- evolution from one presentation mechanism to another", 660)

end vc slide4_vc

/*--------------------- Requirement for UI Tool ----------------------------*/

vc: slide5_vc

  creation condition: (slide = 5)

  objects:

    title(j1, "Requirement for UI Tool")
    bullet(j2, "* For use as prototyper", 250)
    subbullet(j3, "- interactive drawing editor", 320)
    subbullet(j4, "- simulation of behavior", 370)
    bullet(j5, "* For use in production", 440)
    subbullet(j6, "- support for abstraction", 510)  
    subbullet(j7, "- inclusion of arbitrary application functionality", 560)  
    subbullet(j8, "- extensible in term of media", 610)
    subbullet(j9, "- reasonable performance", 660)  

end vc slide5_vc

/*--------------------- Serpent --------------------------------------------*/

vc: slide6_vc

  creation condition: (slide = 6)

  objects:

    title(j1, "Serpent")
    bullet(j2, "* General purpose user interface management system", 250)
    bullet(j3, "* Supports incremental development of user interface", 320)
    bullet(j4, "* Multiple media for presentation", 390)
    bullet(j5, "* Language for specification of user interface", 460)  
    bullet(j6, "* Interactive editor (under development) using ", 530)  
    bullet(j7, "  Serpent language for UI specification and execution", 590)
    bullet(j8, "* X11 Athena Widget Toolkit set currently used for", 660)
    bullet(j9, "  presentation", 720)

end vc slide6_vc

/*--------------------- Serpent as Prototyping Tool ------------------------*/

vc: slide7_vc
  creation condition: (slide = 7)
  objects:

    title(j1, "Serpent as Prototyping Tool")
    bullet(j2, "* Editor includes drawing component", 250)
    bullet(j3, "* Editor outputs Serpent language", 320)
    bullet(j4, "* Language has facility for recording user", 390)
    bullet(j5, "  performance data", 450)

end vc slide7_vc

/*--------------------- Serpent as Production Tool -------------------------*/

vc: slide8_vc
  creation condition: (slide = 8)
  objects:

    title(j1, "Serpent as Production Tool")
    bullet(j2, "* Supports arbitrary applications", 250)
    bullet(j3, "* Supports multiple media, simultaneously, for user", 320)
    bullet(j35, "  interactions", 380)
    bullet(j4, "* Extensible in input/output media supported", 450)
    bullet(j5, "* Supports", 520)
    subbullet(j6, "- fixed application programmer interface", 590)  
    subbullet(j7, "- user control of task sequencing", 640)  
    subbullet(j8, "- consistency of data presentation", 690)  

end vc slide8_vc

/*--------------------- Slang, UI Specification Language -------------------*/

vc: slide9_vc

  creation condition: (slide = 9)

  objects:

    title(j1, "Slang, UI Specification Language")
    bullet(j2, "* Based on production model", 250)
    subbullet(j3, "- data driven", 320)  
    subbullet(j4, "- allows multiple threads of control", 370)  
    bullet(j5, "* Multiple views of the same data", 440)
    subbullet(j6, "- implemented with constraint mechanism", 510)  
    subbullet(j7, "- dependent values automatically re-evaluated", 560)  
    subbullet(j8, "  when independent values modified", 600)  

end vc slide9_vc

/*--------------------- Interaction Objects --------------------------------*/

vc: slide10_vc

  creation condition: (slide = 10)

  objects:

    title(j1, "Interaction Objects")
    bullet(j2, "* Slang provides language for manipulating arbitrary", 250)
    bullet(j3, "  interaction objects", 310)
    bullet(j4, "* Interaction objects contain", 380)
    subbullet(j5, "- attributes for presentation", 450)  
    subbullet(j6, "- methods for interaction", 500)  
    bullet(j7, "* Slang allows for extensible presentation technologies", 570)
    subbullet(j8, "- X11 Athena Widget Toolkit and digital mapping", 640)  
    subbullet(j9, "  system currently supported", 680)  
    subbullet(j10, "- video and gesturing system planned", 730)  

end vc slide10_vc

/*--------------------- Serpent Architecture -------------------------------*/

vc: slide14_vc

  creation condition:  (slide = 11)

  objects:

    title(j1, "Serpent Architecture")

    application_label: label_widget {
      Attributes:
        parent: slide_form;
        font: "vr-20";
        label_text: "Application Layer";
        vert_distance: 240;
/*        horiz_distance: 400;	*/
        horiz_distance: DISPLAY_WIDTH / 2 - 200;
        width: 400;
        height: 75;
    }

    arrow1: label_widget {
      Attributes:
        parent: slide_form;
        label_text: "";
        background_color: "black";
        vert_distance: 	315;
/*        horiz_distance: 601;	*/
        horiz_distance: DISPLAY_WIDTH / 2;
        height: 25;
        width: 6;
    }

    serpent_form: form_widget {
      Attributes:
        parent: slide_form;
        height: 400;
        width: 600;
        vert_distance: 340;
/*        horiz_distance: 300;	*/
        horiz_distance: DISPLAY_WIDTH / 2 - 300;
    }

    arrow2: label_widget {
      Attributes:
        parent: serpent_form;
        label_text: "";
        background_color: "black";
        vert_distance: 	0;
        horiz_distance: 299;
        height: 25;
        width: 6;
    }

    dialogue_label: label_widget {
      Attributes:
        parent: serpent_form;
        font: "vr-20";
        label_text: "Dialogue Layer";
        vert_distance: 25;
        horiz_distance: 100;
        width: 400;
        height: 75;
    }

    arrow3: label_widget {
      Attributes:
        parent: serpent_form;
        label_text: "";
        background_color: "black";
        vert_distance: 	100;
        horiz_distance: 300;
        height: 50;
        width: 6;
    }

    presentation_label: label_widget {
      Attributes:
        parent: serpent_form;
        font: "vr-20";
        label_text: "Presentation Layer";
        vert_distance: 150;
        horiz_distance: 100;
        width: 400;
        height: 75;
    }

    x_label: label_widget {
      Attributes:
        parent: serpent_form;
        font: "vr-20";
        label_text: "X Window System";
        vert_distance: 225;
        horiz_distance: 100;
        width: 200;
        height: 75;
    }

    other_label: label_widget {
      Attributes:
        parent: serpent_form;
        font: "vr-20";
        label_text: "I/O Technologies";
        vert_distance: 225;
        horiz_distance: 300;
        width: 200;
        height: 75;
    }

    serpent_label: label_widget {
      Attributes:
        parent: serpent_form;
        font: "vr-20";
        label_text: "Serpent";
        vert_distance: 325;
        horiz_distance: 0;
        width: 600;
        height: 50;
        border_color: "white";
    }

end vc slide14_vc

/*--------------------- Serpent Editor - 1 -------------------------------*/

vc: slide17_vc

  creation condition: (slide = 12)

  objects:

    title(j1, "Serpent Editor - 1")
    bullet(j2, "* Layouts of user interface are best specified or", 250)
    bullet(j3, "  examined graphically", 300)
    bullet(j4, "* Logic, dependencies and calculations are best", 370)
    bullet(j5, "  specified textually", 420)
    bullet(j6, "* Serpent editor has two portions", 490)
    subbullet(j7, "- structure editor for textual specification", 560)
    subbullet(j8, "- graphical editor for visualization of layout", 610)
    bullet(j9, "* Mechanism for moving between portions", 680)

end vc slide17_vc

/*--------------------- Serpent Editor - 2 -------------------------------*/

vc: slide18_vc

  creation condition: (slide = 13)

  objects:

    title(j1, "Serpent Editor - 2")
    bullet(j2, "* Implemented using Serpent", 250)
    bullet(j3, "* Currently over 5000 lines of Slang", 320)
    bullet(j4, "* Application layer", 390)
    subbullet(j5, "- has commands such as create vc, add object to vc, etc.", 460)
    subbullet(j6, "- saves and restores user interface specifications", 510)
    bullet(j7, "* User interface portion", 580)
    subbullet(j8, "- has forms for structured input", 650)
    subbullet(j9, "- manages layout for realization", 700)

end vc slide18_vc

/*--------------------- Summary -------------------------------*/

vc: slide19_vc

  creation condition: (slide = 14)

  objects:

    title(j1, "Summary")
    bullet(j2, "* Serpent  is a media independent UIMS which", 250)
    bullet(j3, "  supports incremental development of user", 300)
    bullet(j35, "  interfaces through", 350)
    subbullet(j4, "- features which support prototyping", 420)
    subbullet(j5, "- power which supports production", 470)
    subbullet(j6, "- providing an application programming interface", 520)
    subbullet(j7, "  which is constant through changes in the user", 560)
    subbullet(j8, "  interface or in input/output technology", 600)
    subbullet(j9, "- extensibility in user interface technologies", 650)

end vc slide19_vc

end vc main_talk_vc;
