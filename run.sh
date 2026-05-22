#!/bin/bash
meson compile -C .build
if [ "${?}" != "0" ]; then
	exit ${?}
fi

touch valgrind.suppression
/usr/bin/valgrind \
	--vgdb=no \
	--leak-check=full \
	--track-origins=yes \
	--gen-suppressions=all \
	--suppressions=valgrind.suppression \
	.build/udjatpython ${@}
