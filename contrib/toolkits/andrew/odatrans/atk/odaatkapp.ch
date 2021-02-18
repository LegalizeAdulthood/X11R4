/**********************************************
(c) Copyright 1988 Carnegie Mellon University
See andrew/oda/doc/COPYRITE
***********************************************/


class odaatkapp : application[app] {
    methods:
        DoInCoreDoc(char * inputName, char * outputName, DOCUMENT_type InputDoc, BOOL_type doDebugging) returns boolean;
    overrides:
	ParseArgs(int argc,char **argv) returns boolean;
	Run() returns int;
        ReadInitFile();
    classprocedures:
	InitializeObject() returns boolean;
};
