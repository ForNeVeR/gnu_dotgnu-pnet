#!/bin/sh
#
# pnet_build.sh - Script for automatically building pnet from a cron job.
#
# Copyright (C) 2002  Southern Storm Software, Pty Ltd.
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

# Usage: pnet_build.sh [-q] pnet_build.cfg

# Function which is called when the build fails.
failed()
{
	DATEVAL=`date`
	echo '*** Build failed at '"$DATEVAL"' ***' >>"$PNET_BUILD_LOG"
	if test -z "$QUIET"; then
		echo '*** Build failed at '"$DATEVAL"' ***'
	fi
	if test -n "$PNET_EMAIL_USER" ; then
		cat << EOF | $PNET_EMAIL_SEND $PNET_EMAIL_USER
To: $PNET_EMAIL_USER
Subject: Portable.NET build failed

Portable.NET build failed at ${DATEVAL}.  Build log is in:

    $PNET_BUILD_LOG
EOF
	fi
	exit 1
}

# Function which runs a command, logs it and its output,
# and fails the build process if the command fails.
run()
{
	# Report the command to the log and stdout.
	echo '$' $* >>"$PNET_BUILD_LOG"
	if test -z "$QUIET"; then
		echo 'Running:' $*
	fi

	# Run the command, redirecting its output.
	if test "$1" = "cd"; then
		cd "$2"
	else
		if $* >>"$PNET_BUILD_LOG" 2>&1; then
			:
		else
			failed
		fi
	fi
	return 0
}

# Function which runs a sample.  This logs the command and
# its success or failure, but not its output.
runsample()
{
	# Report the command to the log and stdout.
	echo '$' $* >>"$PNET_BUILD_LOG"
	if test -z "$QUIET"; then
		echo 'Running:' $*
	fi

	# Run the command, redirecting its output to /dev/null.
	if $* >/dev/null 2>&1; then
		:
	else
		failed
	fi
	return 0
}

# Determine if we should run in "quiet" mode.
if test "x$1" = "x-q"; then
	QUIET=1
	shift
else
	QUIET=""
fi

# Load the contents of the configuration file.
if test -z "$1"; then
	echo "Usage: $0 pnet_build.cfg" 1>&2
	exit 1
fi
. "$1"

# Check the validity of the configuration information.
if test -z "$PNET_BUILD_BASE"; then
	echo "$0: invalid configuration file supplied" 1>&2
	exit 1
fi
if test ! -d "$PNET_BUILD_BASE"; then
	echo "$PNET_BUILD_BASE: No such file or directory" 1>&2
	exit 1
fi

# Create the initial directory structure.
if test ! -d "$PNET_BUILD_PRISTINE"; then
	if mkdir "$PNET_BUILD_PRISTINE"; then
		:
	else
		exit 1
	fi
fi
if test ! -d "$PNET_BUILD_ACTUAL"; then
	if mkdir "$PNET_BUILD_ACTUAL"; then
		:
	else
		exit 1
	fi
fi
if test ! -d "$PNET_BUILD_LOG_DIR"; then
	if mkdir "$PNET_BUILD_LOG_DIR"; then
		:
	else
		exit 1
	fi
fi

# Start the build log.
DATEVAL=`date`
echo '*** Build started at '"$DATEVAL"' ***' >"$PNET_BUILD_LOG"
if test -z "$QUIET"; then
	echo '*** Build started at '"$DATEVAL"' ***'
fi

# Update the CVS trees.
run cd "$PNET_BUILD_PRISTINE"
if test -d treecc ; then
	run cd treecc
	run $PNET_CVS update -d
	run cd ..
else
	run $PNET_CVS co treecc
fi
if test -d pnet ; then
	run cd pnet
	run $PNET_CVS update -d
	run cd ..
else
	run $PNET_CVS co pnet
fi
if test -d pnetlib ; then
	run cd pnetlib
	run $PNET_CVS update -d
	run cd ..
else
	run $PNET_CVS co pnetlib
fi
if test -d cscctest ; then
	run cd cscctest
	run $PNET_CVS update -d
	run cd ..
else
	run $PNET_CVS co cscctest
fi

# Remove the actual trees.
run rm -rf "$PNET_BUILD_ACTUAL/treecc"
run rm -rf "$PNET_BUILD_ACTUAL/pnet"
run rm -rf "$PNET_BUILD_ACTUAL/pnetlib"
run rm -rf "$PNET_BUILD_ACTUAL/cscctest"

# Copy the pristine trees to the actual trees.
run cp -pr "$PNET_BUILD_PRISTINE/treecc" "$PNET_BUILD_ACTUAL/treecc"
run cp -pr "$PNET_BUILD_PRISTINE/pnet" "$PNET_BUILD_ACTUAL/pnet"
run cp -pr "$PNET_BUILD_PRISTINE/pnetlib" "$PNET_BUILD_ACTUAL/pnetlib"
run cp -pr "$PNET_BUILD_PRISTINE/cscctest" "$PNET_BUILD_ACTUAL/cscctest"

# Build treecc.
run cd "$PNET_BUILD_ACTUAL/treecc"
run ./auto_gen.sh
run ./configure
run make
run make check

# Build pnet.
run cd "$PNET_BUILD_ACTUAL/pnet"
run ./auto_gen.sh
TREECC="$PNET_BUILD_ACTUAL/treecc/treecc"
export TREECC
run ./configure $PNET_CONFIGURE_OPTIONS
run make

# Run the pnet samples and tests (basically checking for
# segfaults, failures, and the like).
run cd samples
runsample ../engine/ilrun hello.exe
runsample ../engine/ilrun fib.exe
runsample ../engine/ilrun evenodd.exe
runsample ../engine/ilrun except.exe
runsample ../engine/ilrun codepage.exe
runsample ../engine/ilrun codepage.exe 932
runsample ../engine/ilrun getenv.exe
runsample ../engine/ilrun getenv.exe PATH
run cd ../tests
run ./test_verify -f
run ./test_thread -f

# Build pnetlib.
run cd "$PNET_BUILD_ACTUAL/pnetlib"
run ./auto_gen.sh
run ./configure --with-pnet="$PNET_BUILD_ACTUAL/pnet"
run make

# Configure and run cscctest.
run cd "$PNET_BUILD_ACTUAL/cscctest"
run ./auto_gen.sh
run ./configure --with-pnet="$PNET_BUILD_ACTUAL/pnet"
run make check

# Done.
DATEVAL=`date`
echo '*** Build ended at '"$DATEVAL"' ***' >>"$PNET_BUILD_LOG"
if test -z "$QUIET"; then
	echo '*** Build ended at '"$DATEVAL"' ***'
	echo '*** Build log written to '"$PNET_BUILD_LOG"' ***'
fi
exit 0
