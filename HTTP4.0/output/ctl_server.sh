#########################################################################
# File Name: ctl_server.sh
# Author: ma6174
# mail: ma6174@163.com
# Created Time: Fri 19 Jan 2018 04:18:54 PM PST
#########################################################################
#!/bin/bash

ROOT=$(pwd)
CONF=$ROOT/conf/http.conf
BIN=$ROOT/httpd

server_pid=''
proc=$(basename $0) 
function usage()
{
	printf "%s : [start(-s)] | [stop(-t)] | [restart(-rs)]\n" "$proc"	
}

if [ $# -ne 1 ];then # -ne 不等于
	usage
	exit 1
fi

function is_exist()
{
	name=$(basename $BIN)
	server_pid=$(pidof $name)
	#if [ $? -eq 0 -a $server_pid -gt 0 ];then
	if [ $? -eq 0 ];then
		return 0
	else
		return 1
	fi
}


function start_server()
{
	if is_exist;then
		echo "server is exist, pid is $server_pid"
	else
		ip=$(awk -F: '/^IP/{print $NF}' $CONF)
		port=$(awk -F: '/^PORT/{print $NF}' $CONF)
		$BIN $ip $port & #把httpd放到后台执行,暂时取代精灵
		echo "start ... done"
	fi
}

function stop_server()
{
	if is_exist;then
		kill -9 $server_pid
		echo "stop ... done"
	else
		echo "server not exist, not need to stop"
	fi
}
case $1 in
	start | -s )
		start_server	
	;;
	stop | -t )
		stop_server
	;;
	restart | -rs )
		stop_server
		start_server
	;;
	*)
		usage
		exit 2
	;;
esac

# $# minglinghang diyige canshu 

