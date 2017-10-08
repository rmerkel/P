/********************************************************************************************//**
 * @file grammer.h
 *
 * The Pascal-lite language grammer in EBNF.
 * @see https://en.wikipedia.org/wiki/Extended_Backus%E2%80%93Naur_form
 *
 * @section grammer Pascal-lite (EBNF) grammer
 *
 *             program = block-decl 'begin' statement-lst 'end' '.' ;
 *          block-decl = [ const-decl-lst ';' ]
 *                       [ type-decl-lst ';' ]
 *                       [ var-decl-blk ';' ]
 *                       [ sub-decl-lst ';' ]
 *                       { statement-blk } ;
 *      const-decl-lst = 'const' const-decl { ';' const-decl } ;
 *          const-decl = identifier '=' const-expr ;
 *       type-decl-lst = type-decl { ';' type-decl } ;
 *           type-decl = 'type' '=' type ;
 *        var-decl-blk = 'var' var-decl-lst ;
 *        var-decl-lst = var-decl { ';' var-decl } ;
 *            var-decl = identifier-lst : type ;
 *                type = simple-type                                                        |
 *                       'real'                                                             |
 *                       'array' '[' simple-type-list ']' 'of' type                         |
 *                       'record' field-lst 'end' ;                                           X
 *           field-lst = var-decl-lst ;                                                       X
 *     simple-type-lst = simple-type { ',' simple-type } ;
 *         simple-type = 'integer'                                                          |
 *                       '(' identifier-lst ')'                                             |
 *                       const-expr '..' const-expr
 *      identifier-lst = identifier { ',' identifier } ;
 *        sub-decl-lst = func-decl | proc-decl ;
 *           proc-decl = 'procedure' identifier param-lst ';' block-decl ';' ;
 *           func-decl = 'function'  identifier param-lst ':' type ';' block-decl ';' ; 
 *           param-lst = [ '(' var-decl-lst ')' ] ;
 *            variable = identifier [ '[' expression-lst ']' { '[' expression-lst ']' } ] ;
 *       statement-blk = 'begin' statement-lst 'end' ;
 *       statement-lst = 'begin' statement {';' statement } 'end' ;
 *           statement = [  variable '=' expression                                         |
 *                          'if' cond 'then' statement { 'else' statement }                 |
 *                          'while' cond 'do' statement                                     |
 *                          'repeat' statement 'until' cond                                 |
 *                          'for' identifer ':=' expression
 *                                            ( 'to' | 'downto' ) expression 'do' statement | X
 *                          statement-blk ] ;
 *          const-expr = [ '+' | '-' ] number | identifier ;
 *      expression-lst = expression { ',' expression } ;
 *          expression = simple-expr { '<' | '<=' | '=' | '>=' | '>' | '<>' simple-expr } ;
 *         simple-expr = [ '+' | '-' ] terminal { '+' | '-' | 'or' terminal } ;
 *            terminal = factor { '*' | '/' | 'rem' | 'and' factor } ;
 *              factor = variable                                                           |
 *                       identifier [ '(' expression-lst ')' ]                              |
 *                       'round' '(' expression ')'                                         |
 *                       number                                                             |
 *                       '(' expression ')' ;
 *      
 *      EBNF Key:
 *           X   Not implemented
 *           {}  Repeat zero or more times
 *           []  Optional; zero or *one* times
 *           ()  Grouping
 *           |   One of ...
 *           ;   End of production
 ************************************************************************************************/

#ifndef	GRAMMER_H
#define GRAMMER_H

#endif
