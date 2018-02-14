#!/usr/bin/env python
#-*-coding=utf8-*-

class HtmlOutput(object):
	def __init__(self):
		self.datas = [] #初始一个存放收集数据的列表

	def collect_data(self,data): #收集解析来的数据
		if data is None:
			return
		for num in data:
			self.datas.append(num)

	def output_html(self): #将收集的数据放入html文件中
		fout = open('output.html','w')

		fout.write("<html>")
		
		fout.write('''<head>
		<meta http-equiv="Content-Type" content="text/html; charset=utf-8">
		<title>在线招聘信息查询</title>
		<style type="text/css">
		<!--
		body,td,th {
			font-size: 15px;
		}
		body {
			background-color: #99FFFF;
		}
		-->
		</style></head>''')
		fout.write("<body>")
		fout.write("<table>")
		
		fout.write("<tr><td>   招聘单位    </td><td>    链接 </td><td>  面试地点 </td><td>    时间    </td>")
		for data in self.datas:
			fout.write("<tr>")
			fout.write("<td>%s</td>" % data['company'].encode('utf-8'))
			fout.write("<td><a href=%s>点击进入</a></td>" % data['link'].encode('utf-8'))
			fout.write("<td>%s</td>" % data['local'].encode('utf-8'))
			fout.write("<td>%s</td>" % data['time'].encode('utf-8'))
			fout.write("</tr>")
		
		fout.write("</table>")
		fout.write("</body>")
		fout.write("</html>")

		fout.close()
