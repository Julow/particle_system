#!/bin/bash
# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    gen.sh                                             :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: jaguillo <jaguillo@student.42.fr>          +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2016/10/14 11:52:54 by jaguillo          #+#    #+#              #
#    Updated: 2016/10/18 22:32:49 by juloo            ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

function gen
{
	{
		echo
		echo "char const		$1[] = R\"("
		python - "$MODULE_DIR/$2" << "EOF" 2>&3
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
		sys.stderr.write("Warning: #include: %s: %s\n" %
				(os.path.relpath(f_name), e.strerror))
		return

	for line in inp:
		inc = match_include(line.rstrip("\n"))
		if inc is not None:
			include_file(get_inc_path(inc), outp)
		else:
			outp.write(line)

def inc_gen(src_name):
	include_file(os.path.abspath(src_name), sys.stdout)

if len(sys.argv) != 2:
	sys.stderr.write("Usage %s src_file > dst_file\n" % sys.argv[0])
	sys.exit(1)

inc_gen(sys.argv[1])

EOF
		echo ")\";"
	} >&2
	echo "extern char const		$1[];"
}

{
	echo "#ifndef GEN_GEN_H"
	echo "# define GEN_GEN_H"
	echo

	echo "#include \"gen.h\"" >&2

	gen prog_particle_init particle_init.cl
	gen prog_particle_update particle_update.cl

	gen prog_particle_render_vert particle_render.vert
	gen prog_particle_render_frag particle_render.frag

	echo
	echo "#endif"
} > "$MODULE_DIR/gen.h" 3>&2 2> "$MODULE_DIR/gen.cpp"
