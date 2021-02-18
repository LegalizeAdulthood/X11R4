/**********************************************
(c) Copyright 1988 Carnegie Mellon University
See andrew/oda/doc/COPYRITE
***********************************************/


class atkodaapp : application[app] {
    overrides:
	ParseArgs(int argc,char **argv) returns boolean;
	Run() returns int;
        ReadInitFile();
    classprocedures:
	InitializeObject() returns boolean;
};
