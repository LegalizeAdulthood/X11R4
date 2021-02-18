/* Wlm widget private definitions */

#ifndef _WlmCompiler_h
#define _WlmCompiler_h

/*
 * Prelude
 */

typedef enum {
	     WlmLocalContext, WlmGlobalContext
	     } WlmContext ;

typedef struct WlmManageItem
    {
    String widget ;
    String type ;
    String value ;
    struct WlmManageItem *next ;
    } WlmManageItem ;

#define MAX_DIRECTIVE_ARGUMENTS 10
#define MAX_DIRECTIVE_CALLDATA_ITEMS 10
#define MAX_DIRECTIVE_CALLDATA_INDICES 2

typedef enum
    {
     WlmDirectiveEquivalence
    ,WlmDirectiveNonEquivalence
    ,WlmDirectiveGreaterThan
    ,WlmDirectiveLessThan
    ,WlmDirectiveGreaterThanOrEqualTo
    ,WlmDirectiveLessThanOrEqualTo
    } WlmDirectiveOperator ;

typedef struct Resource
    {
    String name ;
    String class ;
    String value ;
    WlmContext context ;
    struct Resource *next ;
    } Resource ;

typedef struct Directive
    {
    String callback_name ;
    String call_data_converter[MAX_DIRECTIVE_CALLDATA_ITEMS] ;
    String call_data[MAX_DIRECTIVE_CALLDATA_ITEMS] ;
    WlmDirectiveOperator call_data_operator[MAX_DIRECTIVE_CALLDATA_ITEMS] ;
    int call_data_index[MAX_DIRECTIVE_CALLDATA_ITEMS]
			[MAX_DIRECTIVE_CALLDATA_INDICES] ;
    int      n_call_comparisons ;
    Cardinal n_call_indices[MAX_DIRECTIVE_CALLDATA_ITEMS] ;
    String target_class ;
    String target_name ;
    Resource resource ;
    String procedure ;
    Cardinal n_arguments ;
    String argument_converters[MAX_DIRECTIVE_ARGUMENTS] ;
    String argument_strings[MAX_DIRECTIVE_ARGUMENTS] ;
    long client_data[3] ;
    struct Directive *next ;
    } Directive ;

typedef struct Fetus
    {
    String class_name ;
    Cardinal n_resources ;
    Resource *resource ;
    Cardinal n_directives ;
    Directive *directives ;
    Cardinal n_children ;
    struct Fetus **children ;
    Cardinal n_manage_list ;
    WlmManageItem *manage_list ;
    Widget id ;
    } Fetus ;

typedef struct
    {
    String layout_file ;
    String compiled_file ;
    String class_file ;
    String wldl_file ;
    Cardinal cur_depth ;
    Cardinal max_depth ;
    Stack *child_stack ;
    } WlmCompilerPart ;

#endif _WlmCompiler_h
