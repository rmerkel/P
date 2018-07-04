/********************************************************************************************//**
 * @file grammer.h
 *
 * @page grammer The P EBNF grammer
 * @see [Extended Backus Naur Form](https://en.wikipedia.org/wiki/Extended_Backus%E2%80%93Naur_form)
 *
 *             program = "program" identifier param-lst "is" block-decl "endprog" ;
 *          block-decl = [ const-decl-lst ';'           ]
 *                       [ type-decl-lst ';'            ]
 *                       [ var-decl-blk ';'             ]
 *                       [ sub-decl-lst ';'             ]
 *                       { "begin" statement-lst "end"  } ;
 *      const-decl-lst = "const" const-decl { ';' const-decl } ;
 *          const-decl = identifier '=' const-expr ;
 *       type-decl-lst = type-decl { ';' type-decl } ;
 *           type-decl = "type" "is" type ;
 *        var-decl-blk = "var" var-decl-lst ;
 *        var-decl-lst = var-decl { ';' var-decl } ;
 *            var-decl = identifier-lst : type ;
 *                type = simple-type | structured-type | pointer-type ;
 *         simple-type = "real" | ordinal-type ;
 *    ordinal-type-lst =  ordinal-type { ',' ordinal-type } ;
 *        ordinal-type = '(' identifier-lst ')' | const-expr ".." const-expr                |
 *                       "boolean" | "integer" | "natual" | "positive" | "character" ;
 *     structured-type = "array" '[' ordinal-type-lst ']' "of" type                          |
 *                       "record" field-lst "end" ;
 *           field-lst = var-decl-lst ;
 *        pointer-type = '^' identifier ;
 *        sub-decl-lst = func-decl | proc-decl ;
 *           proc-decl = "procedure" identifier param-lst "is" block-decl "endproc" ;
 *           func-decl = "function"  identifier param-lst ':' type "is" block-decl "endfunc" ; 
 *           param-lst = '(' [ param-decl-lst ] ')' ;
 *      param-decl-lst = param-decl { ';' param-decl } ;
 *          param-decl = [ "var" ] identifier-lst : type ;
 *      identifier-lst = identifier { ',' identifier } ;
 *            variable = identifier [ composite-desc { composite-desc } ] ;
 *      composite-desc = '[' expression-lst ']'                                             |
 *                       '.' identifier                                                     |
 *                       '^' identifier ;
 *       statement-lst = statement {';' statement }  ;
 *        if-statement = "if" expression "then" statement-lst
 *                          { "elif" expresson "then" statement-lst }
 *                          [ "else" statement-lst ]
 *                          "endif" ;
 *     while-statement = "while" expression "loop" statement-lst "endloop" ;
 *       for-statement = "for" identifer "in" [ "reverse" ] ordinal-type
 *                          "loop" statement "endloop" ;
 *    return-statement = "return" [ expression ] ;
 *           statement = [  variable ':=' expression                                        |
 *                          identifier '(' [ expression-lst ] ')'                           |
 *                          if-statement                                                    |
 *                          while-statement                                                 |
 *                          "repeat" statement "until" expression endloop                   |
 *                          for-statement                                                   |
 *                          return-statement ] ;
 *          const-expr = [ '+' | '-' ] number | identifier | character | string ;
 *      expression-lst = expression { ',' expression } ;
 *          expression = simple-expr { '<' | "<=" | '=' | ">=" | '>' | "<>" simple-expr } ;
 *         simple-expr = [ '+' | '-' | "bnot" ] terminal { simple-expr-op terminal } ;
 *      simple-expr-op = '+' | '-' | "bor" | "bxor" | "sleft" | "sright" | "or' ;
 *            terminal = factor { term_op  factor } ;
 *             term_op = '*' | '/' | "rem" | "band" | | "and" ;
 *              factor = variable | character | string                                      |
 *                       identifier '(' [ expression-lst ] ')'                              |
 *                       "round" '(' expression ')'                                         |
 *                       number                                                             |
 *                       '(' expression ')' ;
 *           character = nul | sox | .. | '}' | '`' | del
 *              string = '"' { character } '"'
 *              number = whole number | floting point value
 *
 * Key:
 *
 *      {} Repeat zero or more times
 *      [] Optional; zero or *one* times
 *      () Grouping
 *       | One of ...
 *       ; End of production
 *
 * @author Randy Merkel, Slowly but Surly Software.
 * @copyright  (c) 2017 Slowly but Surly Software. All rights reserved.
 ************************************************************************************************/

#ifndef	GRAMMER_H
#define GRAMMER_H

#endif
