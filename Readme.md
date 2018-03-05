pnet
====

This is a repository for .NET C compiler development. For now, the project is
freshly resurrected and nothing has been built yet.

Main project components are:

- `treecc`, "Tree Compiler-Compiler"
- `pnet` itself, "Portable.NET C Compiler"
- `pnetC`, ANSI-compatible C library for `pnet`

Building treecc
---------------

For now, could only be build in a Unix-compatible environment, tested on Ubuntu
16.04 inside WSL:

```console
# apt-get install automake bison flex texinfo
$ cd treecc
$ ./auto_gen.sh
$ ./configure
$ make all
$ make check
```
