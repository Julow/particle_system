#!/usr/bin/env python
# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    cl_gen.py                                          :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: jaguillo <jaguillo@student.42.fr>          +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2016/10/10 17:16:25 by jaguillo          #+#    #+#              #
#    Updated: 2016/10/10 18:10:18 by jaguillo         ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

import sys, os, re

INCLUDE_RE = re.compile('^\s*#\s*include\s+"([^"]+)"$')

def include_file(f_name, outp):
	def match_include(line):
		m = INCLUDE_RE.match(line)
		return m.group(1) if m is not None else None

	def get_inc_path(inc):
		return os.path.join(os.path.dirname(f_name), inc)

	try:
		inp = open(f_name, "r")
	except IOError as e:
		print("Warning: #include: %s: %s" % (os.path.relpath(f_name), e.strerror))
		return

	for line in inp:
		line = line.rstrip("\n")
		inc = match_include(line)
		if inc is not None:
			include_file(get_inc_path(inc), outp)
		else:
			outp.write(line)
		outp.write("\\n\"\n\"")

def cl_gen(src_name, dst_name):
	def get_name(src_name):
		name, _ = os.path.splitext(os.path.basename(src_name))
		return "cl_program_" + name

	with open(sys.argv[2], "w") as outp:
		outp.write("char const *const %s = \"" % get_name(src_name))
		include_file(os.path.abspath(src_name), outp)
		outp.write("\";\n")

if len(sys.argv) != 3:
	print("Usage %s src.cl dst.c" % sys.argv[0])
	os.exit(1)

cl_gen(sys.argv[1], sys.argv[2])
