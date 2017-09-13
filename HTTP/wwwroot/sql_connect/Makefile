ROOT_PATH=$(shell pwd)
INCLUDE_PATH=$(ROOT_PATH)/lib/include
LIB_PATH=$(ROOT_PATH)/lib/lib
LIB=mysqlclient

.PHONY:all
all:insert_cgi select_cgi

insert_cgi:insert_cgi.o sql_api.o
	g++ -o $@ $^ -L$(LIB_PATH) -l$(LIB)
select_cgi:select_cgi.o sql_api.o
	g++ -o $@ $^ -L$(LIB_PATH) -l$(LIB)
%.o:%.cpp
	g++ -c $< -I$(INCLUDE_PATH)

.PHONY:clean
clean:
	rm -f insert_cgi select_cgi *.o

