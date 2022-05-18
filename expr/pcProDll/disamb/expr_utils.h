/*****************************************************************\
*                                                                *
*  Copyright (C) Regents of University of California, 2003       *
*  This source code is a part of EXPRESSION project and is       *
*  copyrighted by the Regents of the University of California,   *
*  Irvine.                                                       *
*  The authors hereby grant permission to use this description   *
*  and its documentation for educational and non-commercial      *
*  purpose under the BSD license                                 *
*  (http://www.cecs.uci.edu/~express/BSD_License.txt). 	         *
*  The source code comes with no warranty and no author or       *
*  distributor accepts any responsibility for the consequences   *
*  of its use. Permission is granted to distribute this file in  *
*  compiled or executable form under the same conditions that    *
*  apply for source code. Permission is granted	to anyone to     *
*  make or distribute copies of this source code, either as      *
*  received or modified, in any medium, provided that all        *
*  copyright notices, permission and non warranty notices are    *
*  preserved, and that the distributor grants the recipient      *
*  permission for further redistribution as permitted by this    *
*  document. No written agreement, license, or royalty fee is    *
*  required for authorized use of this software.                 *
*                                                                *
*******************************************************************/

int gcd(int a,int b);
expr build_expr_tree(char *&s);
void remove(expr_list l);
void remove_equivalents(expr_list &l1,expr_list &l2,expr_list &removed_list);
int equivalent_lists(expr_list &l1,expr_list &l2,expr_list &removed_list);
expr multiply_with_minus_one(expr e);
int find_constant_in_list(expr_list l);
void parse_expression_list(string*,string,string);
void simplify(expr &e);

// tokens
#define Ident 0
#define Const 1
// '(', ')', '+', '-', '*', '/', '^', '|' are the other tokens

int min4(int,int,int,int);
int max4(int,int,int,int);

int my_add(int a,int b);
int my_mult(int a,int b);
int my_div(int a,int b);

void delete_everything(void);


