ROOT=$(shell pwd)
cc=gcc
bin=httpd
src=$(shell ls *.c)

$(bin):$(src)
	$(cc) -o $@ $^ -pthread
.PHONY:clean
clean:
	rm -f $(bin)
.PHONY:debug
debug:
	@echo $(src)


