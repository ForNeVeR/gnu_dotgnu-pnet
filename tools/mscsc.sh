#! /bin/sh
#
# mscsc.sh - Wrapper around Microsoft's CSC.EXE to give it a more
#            cscc-like command-line syntax.
#
# Copyright (C) 2001  Southern Storm Software, Pty Ltd.
#
# This program is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 2 of the License, or
# (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program; if not, write to the Free Software
# Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA

# Get the name of the compiler to invoke.
if test -z "$1" ; then
	echo "Usage: $0 csc [options] inputfiles" 1>&2
	exit 1
fi
PROGNAME="$0"
CSC="$1"
shift

# Parse the command-line options.
FLAGS="/nologo"
FILES=""
NEEDARG=""
DEFINES=""
LIBDIRS=""
LIBRARIES=""
RESOURCES=""
for i in $*; do
	if test -z "$NEEDARG" ; then
		case "$i" in
			-nostdlib)				FLAGS="$FLAGS /nostdlib" ;;
			-g)						FLAGS="$FLAGS /debug+" ;;
			-O|-O2|-O3)				FLAGS="$FLAGS /optimize+" ;;
			-O0)					FLAGS="$FLAGS /optimize-" ;;
			-o)						NEEDARG="o" ;;
			-c|-S|-E|-v|-vv)		;;
			-Werror)				FLAGS="$FLAGS /warnaserror+" ;;
			-Wall)					FLAGS="$FLAGS /warn:4" ;;
			-Wno-dll-import)		FLAGS="$FLAGS /nowarn:626" ;;
			-Wno-field-assign)		FLAGS="$FLAGS /nowarn:649" ;;
			-fsyntax-check)			;; #FLAGS="$FLAGS /nooutput" ;;
			-fchecked)				FLAGS="$FLAGS /checked+" ;;
			-funchecked)			FLAGS="$FLAGS /checked-" ;;
			-fincremental)			FLAGS="$FLAGS /incremental" ;;
			-funsafe)				FLAGS="$FLAGS /unsafe" ;;
			-fresources=)
				 RESOURCES=`expr "$i" : '-fresources=\(.*\)$'` ;;
			-e)						NEEDARG="e" ;;
			-I*|-f*|-W*)			;;
			-D*) if test -z "$DEFINES" ; then
					DEFINES=`expr "$i" : '-D\(.*\)$'`
				 else
				 	DEFINES="$DEFINES;"`expr "$i" : '-D\(.*\)$'`
				 fi ;;
			-L*) LIBDIRS="$LIBDIRS "`expr "$i" : '-L\(.*\)$'` ;;
			-l*) LIBRARIES="$LIBRARIES "`expr "$i" : '-l\(.*\)$'` ;;
			-*)	 echo "${PROGNAME}: unknown option $i" 1>&2
				 exit 1 ;;
			 *)	 FILES="$FILES $i" ;;
		esac
	else
		case "$NEEDARG" in
			o)	case "$i" in
					*.dll) FLAGS="$FLAGS /target:library" ;;
					*.exe) FLAGS="$FLAGS /target:exe" ;;
					    *) FLAGS="$FLAGS /target:module" ;;
				esac
				FLAGS="$FLAGS /out:$i" ;;
			e)	FLAGS="$FLAGS /main:$i" ;;
			*)	;;
		esac
		NEEDARG=""
	fi
done

# Add extra flags as necessary.
if test -n "$DEFINES" ; then
	FLAGS="$FLAGS /define:$DEFINES"
fi
if test -n "$LIBRARIES" -a -n "$LIBDIRS" ; then
	REFS=""
	for lib in $LIBRARIES; do
		for dir in $LIBDIRS; do
			if test -f "${dir}/${lib}.dll" ; then
				if test -z "$REFS" ; then
					REFS="${dir}/${lib}.dll"
				else
					REFS="$REFS;${dir}/${lib}.dll"
				fi
			fi
		done
	done
	if test -n "$REFS" ; then
		FLAGS="$FLAGS /reference:"`echo "$REFS" | sed -e '1,$s%/%\\\\%g' -`
	fi

fi
if test -n "$RESOURCES" ; then
	FLAGS="$FLAGS /resource:"`echo "$RESOURCES" | sed -e '1,$s%/%\\\\%g' -`
fi

# Convert forward slashes in the filenames into backslashes
# so that CSC.EXE doesn't try to treat the names as options.
FILES=`echo "$FILES" | sed -e '1,$s%/%\\\\%g' -`

# Execute the compiler.
echo "$CSC" $FLAGS $FILES
exec "$CSC" $FLAGS $FILES
