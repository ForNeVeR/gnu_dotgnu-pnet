#!/usr/bin/env python
import xml.dom.minidom
import string
import sys
import cgi
import regex
import regex_syntax

# check if it has a child with that name
def _haschild(node,name):
	return len(node.getElementsByTagName(name))

class opcode:
	def __init__(self,opcode):
		self.node=opcode
		self.name=opcode.getAttribute("name")
	def read(self):
		if(_haschild(self.node,"operation")):
			self.read_operation()
		if(_haschild(self.node,"format")):
			self.read_formats()
		if(_haschild(self.node,"form")):
			self.read_forms()
		if(_haschild(self.node,"before")):
			self.read_before()
		if(_haschild(self.node,"after")):
			self.read_after()
		if(_haschild(self.node,"description")):
			self.read_description()
		if(_haschild(self.node,"notes")):
			self.read_notes()
		if(_haschild(self.node,"exceptions")):
			self.read_exceptions()
	def read_operation(self):
		operation=self.node.getElementsByTagName("operation")[0];	
		self.operation=""
		for each in operation.childNodes:
			self.operation=self.operation+each.toxml()
			
	def read_formats(self):
		self.formats=[]
		formats=self.node.getElementsByTagName("format")
		for each in formats:
			ftext=""
			for each2 in each.childNodes:
				ftext=ftext+each2.toxml()
			ftext=string.replace(ftext,"<fsep/>","</i></td></tr><tr><td align=\"center\" width=\"100\"><i>")
			self.formats.append(ftext)

	def read_forms(self):
		self.forms=[]
		forms=self.node.getElementsByTagName("form")
		for each in forms:
			self.forms.append((each.getAttribute("name"),\
			each.getAttribute("code")))

	def read_before(self):
		before=(self.node.getElementsByTagName("before")[0])
		self.beforetext=before.childNodes[0].data;
		self.befores=string.split(before.childNodes[0].data,",")
		self.befores.reverse()

	def read_after(self):
		after=(self.node.getElementsByTagName("after")[0])
		self.aftertext=after.childNodes[0].data;
		self.afters=string.split(after.childNodes[0].data,",")
		self.afters.reverse()
	
	def read_description(self):
		description=self.node.getElementsByTagName("description")[0]
		self.description=""
		for each in description.childNodes:
			self.description=self.description+each.toxml()
	
	def read_notes(self):
		notes=self.node.getElementsByTagName("notes")[0]
		self.notes=""
		for each in notes.childNodes:
			self.notes=self.notes+each.toxml()

	def read_exceptions(self):
		exceptions=self.node.getElementsByTagName("exceptions")[0]
		self.exceptions=[]
		for each in exceptions.getElementsByTagName("exception"):
			text=""
			for each2 in each.childNodes:
				text=text+each2.toxml()
			self.exceptions.append(each.getAttribute("name"),text)
	
###########THE HTML WRITE FUNCTION############
	def write(self,fp,codes):
		fp.write("""<table cellpadding="5" cellspacing="0" 
		bordercolor="#000000" border="
		1" align="center" width="80%">\n""")
		
		fp.write("""<tr bgcolor="#7F7F7F" border="2"><td border="0" 
		width="30%">\n""")
		
		fp.write("<a name=\""+cgi.escape(self.name,1)+"\">\n")
		fp.write("""<font color="#FFFFFF" size="+5"> \n""")
		fp.write("&nbsp;"+cgi.escape(self.name)+"</font>")
		fp.write("</a>")
		fp.write("""</td><td align="right" border="0">&nbsp;""")
		#if(_haschild(self.node,"form")):
		#	self.write_forms(fp)
		fp.write("</td></tr>")
		if(_haschild(self.node,"operation")):
			self.write_operation(fp)
		if(_haschild(self.node,"format")):
			self.write_formats(fp)
		if(_haschild(self.node,"form")):
			self.write_forms_text(fp,codes)
		if(_haschild(self.node,"before")):
			self.write_stack(fp)
		if(_haschild(self.node,"description")):
			self.write_description(fp)
		if(_haschild(self.node,"notes")):
			self.write_notes(fp)
		if(_haschild(self.node,"exceptions")):
			self.write_exceptions(fp)
		#fp.write("""<tr border="0"><td border="0"><br></td><td border="0">
		#<br></td></tr>""")
		fp.write("</table><br><br>")

	def write_operation(self,fp):
		fp.write("""<tr border="1"><td border="0">&nbsp;&nbsp;&middot;
		&nbsp;<b>Operation</b></td><td border="0">""")
		
		fp.write(self.operation)
		fp.write("</td></tr>")
	
	def write_formats(self,fp):
		fp.write("""<tr border="0"><td border="0">&nbsp;&nbsp;&middot;&nbsp;
		<b>Format </b></td><td border="0">""")
		fp.write("""<table border="0" bordercolor="#AAAAAA" cellpadding="6" 
			cellspacing="0"><tr>""")
		for each in self.formats:
			fp.write ("<td><table border=\"1\" cellpadding=\"6\" cellspacing=\"0\"><tr><td align=\"center\" width=\"100\"><i>"+each+"</i></td></tr></table></td>")
		fp.write("</tr></table>")
		fp.write("</td></tr>")
	
	def	write_forms(self,fp):
		fp.write("""<font color="#FFFFFF" size="-2">""")
		for each in self.forms:
			fp.write("<a href=\"opcodes.html#"+each[1]+"\"><i>")
			fp.write("""<font color="#FFFFFF" size="-2">""")
			fp.write(each[1]+"</font></i></a>&nbsp;,")
		fp.write("</font>")
	
	def write_forms_text(self,fp,codes):
		fp.write("""<tr border="0"><td border="0">&nbsp;&nbsp;&middot;&nbsp;
		<b>Forms </b></td><td border="0">""")
		for each in self.forms:
			if string.find(each[1],"COP_PREFIX") == 0:
				fp.write("<i>"+each[0]+"</i> = 255, "+`string.atoi(codes[each[1]],0)`+" (0xFF, "+codes[each[1]]+")<br>");
			else:
				fp.write("<i>"+each[0]+"</i> = "+`string.atoi(codes[each[1]],0)`+" ("+codes[each[1]]+")<br>");
		fp.write("</td></tr>")

	def write_stack(self,fp):
		fp.write("""<tr border="0"><td border="0">&nbsp;&nbsp;&middot;&nbsp;
		<b>Stack</b></td>""")
		fp.write("""<td border="0"><i>""")
		fp.write(self.beforetext +"</i>&nbsp;=&gt;&nbsp;<i>"+self.aftertext+"</i>")
		fp.write("</td></tr>")

	def write_description(self,fp):
		fp.write( """<tr border="0"><td border="0" valign="top">&nbsp;
		&nbsp;&middot;&nbsp;<b>Description </b></td><td border="0">""")
		fp.write( self.description)
		fp.write( "</td></tr>")
		
	def write_notes(self,fp):
		fp.write( """<tr border="0"><td border="0" valign="top">&nbsp;
		&nbsp;&middot;&nbsp;<b>Notes </b></td><td border="0">""")
		fp.write( ""+self.notes+"")
		fp.write( "</td></tr>")
	
	def write_exceptions(self,fp):
		fp.write( """<tr border="0"><td border="0" valign="top">&nbsp;
		&nbsp;&middot;&nbsp;<b>Exceptions </b></td><td border="0">""")

		for each in self.exceptions:
			fp.write( "<code>"+each[0]+"</code>  -- "+each[1]+"</font><br>")
		fp.write( "</td></tr>")

def print_header(fp,heading,alphindex,contents):
	fp.write("""
	<!DOCTYPE HTML PUBLIC "-//W3C//DTD HTML 3.2//EN">
	<html>
	<head>
	<title>.:: The Converted Virtual Machine Instruction Set ::.</title>
	</head>
	<body BGCOLOR="#DbDbDb" text="#000000" LINK="#0C0C0C" VLINK="#070707" ALINK="#007007">
	""")
	fp.write("""<h1 align="center"> %s </h1>""" % string.upper(heading))
	if(contents=="index"):
		return 
	fp.write("""<hr>
	<table align="center" width="80%"  bordercolor="#cdcdcd" 
	cellspacing="0" cellpadding="5"><tr border="2" >
	</td><td align="left">""")

	fp.write("""<a href="index.html">
	Contents</a>""")
	for each in alphindex:
		fp.write("&nbsp;&middot;&nbsp;")
		if(each==contents):
			fp.write("<b>("+string.upper(each)+")</b>")
		else:
			fp.write("""<a href="%s.html">
			%s
			</a>""" % (each,string.upper(each)))
	fp.write("</font>")
	fp.write("</td></tr></table>")
	fp.write("<hr><br>")
def print_footer(fp):
	fp.write("""<p align="center"> <font size="-2">Copyright &copy; Southern 
	Storm Software Pty Ltd 2002 <br> Licensed under GNU FDL </font></p>""")
	fp.write("</body></html>")

def print_index(fp,namelist,alphindex):
	print_header(fp,"Converted Virtual Machine Instruction Set",alphindex,"index")
	fp.write("""
	<table bordercolor="#CDCDCD" border="0" align="center" width="90%"> 
	""")
	
	i=0
	for each in namelist:
		if((i!=0 and i%4==0) or i==len(namelist)):
			fp.write("</tr>")
		if(i%4==0):
			fp.write("<tr border=\"2\">")
		fp.write("""<td border="1">""")
		fp.write("<a href=\""+each[0]+".html#"+cgi.escape(each,1)+"\"><b>"+cgi.escape(each)+"</b></a>")
		fp.write("""</td>""")
		
		i=i+1
#	print "</p>"
	while(i%4!=0):
		fp.write("<td>&nbsp;</td>")
		i=i+1
	fp.write("</tr>")
	fp.write("</table>")
	fp.write("<br><br>")
	print_footer(fp)
	
if __name__=="__main__":
	#src=xml.dom.minidom.parse("cvm.xml")
	src=xml.dom.minidom.parse(sys.stdin)
	src.normalize()
	list={}
	for each in src.getElementsByTagName("opcode"):
		op=opcode(each)
		op.read()
		list[op.name]=op
	namelist=list.keys()
	namelist.sort()
	filenames={}
	for each in namelist:
		if(not filenames.has_key(each[0])):
			fp=open(sys.argv[1]+"/"+each[0]+".html","w")
			filenames[each[0]]=fp
	
	codes={}
	codefile=open(sys.argv[2], 'r')
	syntax=regex.set_syntax(regex_syntax.RE_SYNTAX_GREP)
	try:
		prog=regex.compile("^#define[ \t]*COP_")
	finally:
		syntax=regex.set_syntax(syntax)
	while 1:
		line = codefile.readline()
		if not line: break
		if prog.search(line) >= 0:
			fields=string.split(line)
			codes[fields[1]]=fields[2]

	alphindex=filenames.keys()
	alphindex.sort()
	index=open(sys.argv[1]+"/index.html","w")
	print_index(index,namelist,alphindex)
	index.close()
	for each in alphindex:
		fp=filenames[each]
		print_header(fp,each,alphindex,each)

	for each in namelist:
		fp=filenames[each[0]]
		list[each].write(fp,codes)
	for each in filenames.values():
		print_footer(each)
		each.close()
