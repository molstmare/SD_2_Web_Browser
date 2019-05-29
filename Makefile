LIBS=-I"include"
COMPILER=g++
FLAGS=-lsqlite3 -pthread
DEPENDENCIES=middleware.o

all: login

middleware.o: middleware.cpp
	$(COMPILER) -I$(LIBS) -c middleware.cpp

login: $(DEPENDENCIES)
	$(COMPILER) -I"include" $(FLAGS) login.cpp -o login $(DEPENDENCIES)
