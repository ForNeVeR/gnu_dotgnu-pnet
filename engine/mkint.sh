#!/bin/sh
#
# Script that is used to generate "int_proto.h" and "int_table.c"
# from the compiled pnetlib assemblies.
#

DLLS="../samples/mscorlib.dll ../samples/System.dll ../samples/I18N.CJK.dll"
../ilnative/ilinternal -p $DLLS >int_proto.h
../ilnative/ilinternal -t $DLLS >int_table.c
exit 0
