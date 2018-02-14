#!/usr/bin/env python2.6
#-*-coding:utf8-*-

import codecs
import requests
from bs4 import BeautifulSoup
import re
import urlparse
import html_output

#URL = 'http://jiuye.www.sust.edu.cn/zph.jsp?urltype=tree.TreeTempUrl&wbtreeid=1003'
URLHead = 'http://jiuye.www.sust.edu.cn'
URL = "?urltype=tree.TreeTempUrl&wbtreeid=1003"


def Download_Page(url):
	headers = {
		'User-Agent': 'Mozilla/5.0 (Macintosh; Intel Mac OS X 10_11_2) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/47.0.2526.80 Safari/537.36'
	}
	new_url = "http://jiuye.www.sust.edu.cn/zph.jsp%s" % url
	data = requests.get(new_url, headers=headers).content
	return data

def parse_html(html):

	#<tr style="line-height:180%;border-bottom: 1px dotted #cccccc;font-size:10pt;">          
	#<td width="50%" bgcolor="#FFFFFF" style="line-height:180%;border-bottom: 1px dotted #cccccc;font-size:10pt;"><img src="/images/dot-sj.gif">
	#<a href="/new.jsp?urltype=news.NewsContentUrl&amp;wbtreeid=1003&amp;wbnewsid=11042" target="_blank" title="2017年常州市赴陕西科技大学校园专场招聘会" style="">2017年常州市赴陕西科技大学校园专场招聘会</a></td>
	#<td width="30%" align="left" bgcolor="#FFFFFF" style="border-bottom: 1px dotted #cccccc;font-size:10pt;"> 三号教学楼大厅</td>
	#<td width="20%" align="center" bgcolor="#FFFFFF" style="border-bottom: 1px dotted #cccccc;font-size:10pt;"><font size="11pt"></font> 10-28 09:00</td></tr>
	soup = BeautifulSoup(html)
	datas = []
	list_souptrs = soup.find_all('tr', style="line-height:180%;border-bottom: 1px dotted #cccccc;font-size:10pt;")          

	for list_souptr in list_souptrs:
		#解析招聘公司
		name_list = {}
		list_soupa1 = list_souptr.find('a',target='_blank')
		link = URLHead + list_soupa1['href']
		list_company = list_soupa1.get_text()
		name_list['company'] = list_company
		name_list['link'] = link
		
		#解析招聘地点
		list_local = list_souptr.find('td',align='left')
		local = list_local.get_text()
		name_list['local'] = local
		
		#解析招聘时间
		list_time = list_souptr.find('td',align='center')
		time = list_time.get_text()
		name_list['time'] = time
		
		datas.append(name_list)
	
	next_page = soup.find('a',class_='Next')
	if next_page:
		link2 = next_page['href']
		return datas, link2
	return datas, None


def main():
	url = URL
	count=0
	output = html_output.HtmlOutput()
	all_datas = []
	while url:
		if count == 3:
			break
		count+=1
		html = Download_Page(url)
		data_part, url = parse_html(html)
		all_datas.extend(data_part)
		
	#将数据保存输出
	output.collect_data(all_datas)
	output.output_html()

#	for data in data_part:
#		print data['company'].encode('utf-8')
#		print data['link'].encode('utf-8')
#		print data['local'].encode('utf-8')
#		print data['time'].encode('utf-8')

def phtml():
	fout = open('output.html','r')
	while True:
		line = fout.readline()
		if not line: break
		print line
	fout.close();
	

#__name__ 是当前模块名，当模块被直接运行时模块名为 __main__ 。这句话的意思就是，当模块被直接运行时，以下代码块将被运行，当模块是被导入时，代码块不被运行
if __name__ == '__main__':
	main()
	phtml()


