ROOT=$(shell pwd)
WWWROOT=$(ROOT)/wwwroot
SQL=$(ROOT)/wwwroot/sql_connect
PLUGIN=$(ROOT)/plugin

cc=gcc
bin=httpd
src=$(shell ls *.c)
 
.PHONY:all
all:$(bin) cgi

$(bin):$(src)
	$(cc) -o $@ $^ -pthread

cgi:
	cd $(WWWROOT)/cgi_bin;make;cp math_cgi $(ROOT);cd - #回到上级目录
	cd $(SQL);make;cp insert_cgi $(ROOT);cp select_cgi $(ROOT);cd -
.PHONY:clean
clean:
	cd $(WWWROOT)/cgi_bin;make clean;cd - #回到上级目录
	cd $(SQL);make clean;cd -
	rm -rf output

.PHONY:output
output:
	mkdir -p output/wwwroot/cgi_bin 
	mkdir -p output/conf
	cp -rf wwwroot/error output
	cp -rf wwwroot/imag output
	cp conf/http.conf output/conf
	cp -rf wwwroot/sust output/wwwroot
	cp wwwroot/favicon.ico output/wwwroot
	mv httpd output
	mv math_cgi output/wwwroot/cgi_bin
	mv insert_cgi output/wwwroot/cgi_bin
	mv select_cgi output/wwwroot/cgi_bin
	cp wwwroot/index.html output/wwwroot
	cp $(PLUGIN)/ctl_server.sh output

.PHONY:debug
debug:
	@echo $(src)


