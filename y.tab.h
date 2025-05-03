/* A Bison parser, made by GNU Bison 3.8.2.  */

/* Bison interface for Yacc-like parsers in C

   Copyright (C) 1984, 1989-1990, 2000-2015, 2018-2021 Free Software Foundation,
   Inc.

   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <https://www.gnu.org/licenses/>.  */

/* As a special exception, you may create a larger work that contains
   part or all of the Bison parser skeleton and distribute that work
   under terms of your choice, so long as that work isn't itself a
   parser generator using the skeleton or a modified version thereof
   as a parser skeleton.  Alternatively, if you modify or redistribute
   the parser skeleton itself, you may (at your option) remove this
   special exception, which will cause the skeleton and the resulting
   Bison output files to be licensed under the GNU General Public
   License without this special exception.

   This special exception was added by the Free Software Foundation in
   version 2.2 of Bison.  */

/* DO NOT RELY ON FEATURES THAT ARE NOT DOCUMENTED in the manual,
   especially those whose name start with YY_ or yy_.  They are
   private implementation details that can be changed or removed.  */

#ifndef YY_YY_Y_TAB_H_INCLUDED
# define YY_YY_Y_TAB_H_INCLUDED
/* Debug traces.  */
#ifndef YYDEBUG
# define YYDEBUG 0
#endif
#if YYDEBUG
extern int yydebug;
#endif

/* Token kinds.  */
#ifndef YYTOKENTYPE
# define YYTOKENTYPE
  enum yytokentype
  {
    YYEMPTY = -2,
    YYEOF = 0,                     /* "end of file"  */
    YYerror = 256,                 /* error  */
    YYUNDEF = 257,                 /* "invalid token"  */
    IDENTIFIER = 258,              /* IDENTIFIER  */
    FUNCTION_NAME = 259,           /* FUNCTION_NAME  */
    STRING = 260,                  /* STRING  */
    NUMBER = 261,                  /* NUMBER  */
    MAKE = 262,                    /* MAKE  */
    TYPE_INT = 263,                /* TYPE_INT  */
    PRINT = 264,                   /* PRINT  */
    WRITE = 265,                   /* WRITE  */
    OPERATION = 266,               /* OPERATION  */
    COPY = 267,                    /* COPY  */
    EQUALS = 268,                  /* EQUALS  */
    SUM = 269,                     /* SUM  */
    MULT = 270,                    /* MULT  */
    SUB = 271,                     /* SUB  */
    MAIN = 272,                    /* MAIN  */
    ASSIGN = 273,                  /* ASSIGN  */
    COMMA = 274,                   /* COMMA  */
    LBRACKET = 275,                /* LBRACKET  */
    RBRACKET = 276,                /* RBRACKET  */
    Colon = 277                    /* Colon  */
  };
  typedef enum yytokentype yytoken_kind_t;
#endif
/* Token kinds.  */
#define YYEMPTY -2
#define YYEOF 0
#define YYerror 256
#define YYUNDEF 257
#define IDENTIFIER 258
#define FUNCTION_NAME 259
#define STRING 260
#define NUMBER 261
#define MAKE 262
#define TYPE_INT 263
#define PRINT 264
#define WRITE 265
#define OPERATION 266
#define COPY 267
#define EQUALS 268
#define SUM 269
#define MULT 270
#define SUB 271
#define MAIN 272
#define ASSIGN 273
#define COMMA 274
#define LBRACKET 275
#define RBRACKET 276
#define Colon 277

/* Value type.  */
#if ! defined YYSTYPE && ! defined YYSTYPE_IS_DECLARED
union YYSTYPE
{
#line 20 "novex.y"

    int num;
    char *str;

#line 116 "y.tab.h"

};
typedef union YYSTYPE YYSTYPE;
# define YYSTYPE_IS_TRIVIAL 1
# define YYSTYPE_IS_DECLARED 1
#endif


extern YYSTYPE yylval;


int yyparse (void);


#endif /* !YY_YY_Y_TAB_H_INCLUDED  */
