mm gen

CL_FILES=$(find srcs | grep '\.cl$')
DEPEND_FILE=depend.mk
TMP_FILE=/tmp/gen_depend_tmp

{
	printf "O_FILES +="
	for cl_file in $CL_FILES; do printf "%s" " \$(O_DIR)/$cl_file.o"; done
	echo
} | cat - "$DEPEND_FILE" > "$TMP_FILE" && mv "$TMP_FILE" "$DEPEND_FILE"

for cl_file in $(find srcs | grep '\.cl$'); do
	cat << EOF

\$(O_DIR)/$cl_file.o: \$(O_DIR)/$cl_file.c
\$(O_DIR)/$cl_file.c: $cl_file
	./cl_gen.py "\$<" "\$@"
EOF
done >> "$DEPEND_FILE"
