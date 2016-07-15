##
## Makefile for is_utf8
##
## Made by julien palard
## Login   <is_utf8@mandark.fr>
##

NAME = isutf8
SRC = main.c is_utf8.c
OBJ = $(SRC:.c=.o)

VERSION = 0
MINOR = 0
RELEASE = 0

LIB_SRC = is_utf8.c

LINKERNAME = lib$(NAME).so
SONAME = $(LINKERNAME).$(VERSION)
REALNAME = $(SONAME).$(MINOR).$(RELEASE)

CC = gcc
CFLAGS = -O3 -Wextra -Wall -ansi -Wstrict-prototypes

$(NAME):	IS_UTF8_LIB $(OBJ)
	$(CC) $(CFLAGS) -o $(NAME) $(OBJ)

IS_UTF8_LIB:
	$(CC) --shared -fPIC $(CFLAGS) $(LIB_SRC) -o $(LINKERNAME)

all:
		@make $(NAME)

clean:
		rm -f $(NAME) $(LINKERNAME) $(OBJ) $(LIB_OBJ)

re:		clean all
