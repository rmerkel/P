/********************************************************************************************//**
 * @file grammer.h
 *
 * The P language grammer in EBNF.
 * @see https://en.wikipedia.org/wiki/Extended_Backus%E2%80%93Naur_form
 *
 * @section grammer P (EBNF) grammer
 *
 *             program = block-decl statement-blk '.' ; 
 *          block-decl = [ const-decl-lst ';' ]
 *                       [ type-decl-lst ';'  ]
 *                       [ var-decl-blk ';'   ]
 *                       [ sub-decl-lst ';'   ]
 *                       { statement-blk }    ;
 *      const-decl-lst = 'const' const-decl { ';' const-decl } ;
 *          const-decl = identifier '=' const-expr ;
 *       type-decl-lst = type-decl { ';' type-decl } ;
 *           type-decl = 'type' '=' type ;
 *        var-decl-blk = 'var' var-decl-lst ;
 *        var-decl-lst = var-decl { ';' var-decl } ;
 *            var-decl = identifier-lst : type ;
 *                type = simple-type | structured-type | pointer-type ;
 *         simple-type = 'real' | ordinal-type ;
 *        ordinal-type = '(' identifier-lst ')' | const-expr '..' const-expr                      |
 *                       'boolean' | 'integer' | 'natual' | 'positive' | 'character' ;
 *     structured-type = 'array' '[' simple-type-list ']' 'of' type                         |
 *                       'record' field-lst 'end' ;
 *           field-lst = var-decl-lst ;
 *        pointer-type = '^' identifier ;
 *     simple-type-lst = simple-type { ',' simple-type } ;
 *        sub-decl-lst = func-decl | proc-decl ;
 *           proc-decl = 'procedure' identifier param-lst block-decl ';' ;
 *           func-decl = 'function'  identifier param-lst ':' type block-decl ';' ; 
 *           param-lst = '(' [ param-decl-lst ] ')' ;
 *      param-decl-lst = param-decl { ';' param-decl } ;
 *          param-decl = [ 'var' ] identifier-lst : type ;
 *      identifier-lst = identifier { ',' identifier } ;
 *            variable = identifier [ composite-desc { composite-desc } ] ;
 *      composite-desc = '[' expression-lst ']'                                             |
 *                       '.' identifier                                                     |
 *                       '^' identifier ;
 *       statement-blk = 'begin' statement-lst 'end' ;
 *       statement-lst = 'begin' statement {';' statement } 'end' ;
 *           statement = [  variable '=' expression                                         |
 *                          identifier '(' [ expression-lst ] ')'                           |
 *                          'if' expression 'then' statement { 'else' statement }           |
 *                          'while' expression 'do' statement                               |
 *                          'repeat' statement 'until' expression                           |
 *                          'for' identifer ':=' expression
 *                                            ( 'to' | 'downto' ) expression 'do' statement |
 *                          statement-blk ] ;
 *          const-expr = [ '+' | '-' ] number | identifier | character | string ;
 *      expression-lst = expression { ',' expression } ;
 *          expression = simple-expr { '<' | '<=' | '=' | '>=' | '>' | '<>' simple-expr } ;
 *         simple-expr = [ '+' | '-' | 'bit_not' ] terminal { simple-expr-op terminal } ;
 *      simple-expr-op = '+' | '-' | 'bit_or' |'bit_xor' | 'shift_left' | 'shift_right | 'or' ;
 *            terminal = factor { term_op  factor } ;
 *             term_op = '*' | '/' | 'rem' | 'bit_and' | | 'and' ;
 *              factor = variable | character | string                                      |
 *                       identifier '(' [ expression-lst ] ')'                              |
 *                       'round' '(' expression ')'                                         |
 *                       number                                                             |
 *                       '(' expression ')' ;
 *       
 *      EBNF Key:
 *           {}  Repeat zero or more times
 *           []  Optional; zero or *one* times
 *           ()  Grouping
 *           |   One of ...
 *           ;   End of production
 *
 * @author Randy Merkel, Slowly but Surly Software.
 * @copyright  (c) 2017 Slowly but Surly Software. All rights reserved.
 ************************************************************************************************/

#ifndef	GRAMMER_H
#define GRAMMER_H

#endif
