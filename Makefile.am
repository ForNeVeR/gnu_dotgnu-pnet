if USING_LIBFFI
LIBFFI_DIR = libffi
else
LIBFFI_DIR = 
endif
  
if USING_LIBGC
LIBGC_DIR = libgc
else
LIBGC_DIR = 
endif
  
if PNET_TOOLS

SUBDIRS = . $(LIBGC_DIR) $(LIBFFI_DIR) include support image dumpasm engine ilasm ildasm \
		  ilalink ilsize ilnative ilheader ilfind ildd codegen cscc \
		  resgen ilstrip ilranlib ilgac csdoc csant clrwrap samples doc tests

else

## We only want to build the runtime engine.
SUBDIRS = . $(LIBGC_DIR) $(LIBFFI_DIR) include coremul support image engine

endif

