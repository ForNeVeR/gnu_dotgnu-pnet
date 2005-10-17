## Process this file with automake to produce Makefile.in
lib_LTLIBRARIES = libCrayons.la

#libcrayonsdir = $(libdir)
#libcrayons_LTLIBRARIES = libCrayons.la

libCrayons_la_SOURCES = CMemory.c Crayons.h CrayonsInternal.h CrayonsConfig.h \
						CAffineTransform.c   CAffineTransform.h \
						CBezier.c            CBezier.h \
						CBitmap.c            CBitmap.h \
						CBitmapSurface.c     CBitmapSurface.h \
						CBlend.c             CBlend.h \
						CBrush.c             CBrush.h \
						CColorPalette.c      CColorPalette.h \
						CFiller.c            CFiller.h \
						CFlattener.c         CFlattener.h \
						CFTFont.c            CFTFont.h \
						CGraphics.c          CGraphics.h \
						CGraphicsPipeline.c  CGraphicsPipeline.h \
						CHatchBrush.c        CHatchBrush.h \
						CImage.c             CImage.h \
						CLineBrush.c         CLineBrush.h \
						CMath.c              CMath.h \
						CMatrix.c            CMatrix.h \
						CMutex.c             CMutex.h \
						CPathBrush.c         CPathBrush.h \
						CPath.c              CPath.h \
						CPathInterpreter.c   CPathInterpreter.h \
						CPen.c               CPen.h \
						CPointArray.c        CPointArray.h \
						CPolygon.c           CPolygon.h \
						CRegion.c            CRegion.h \
						CRegionCloner.c      CRegionCloner.h \
						CRegionDisposer.c    CRegionDisposer.h \
						CRegionInterpreter.c CRegionInterpreter.h \
						CRegionRasterizer.c  CRegionRasterizer.h \
						CRegionStack.c       CRegionStack.h \
						CRegionTransformer.c CRegionTransformer.h \
						CRegionTranslator.c  CRegionTranslator.h \
						CSolidBrush.c        CSolidBrush.h \
						CStringFormat.c      CStringFormat.h \
						CStroker.c           CStroker.h \
						CSurface.c           CSurface.h \
						CTextureBrush.c      CTextureBrush.h \
						CTrapezoids.c        CTrapezoids.h \
						CUtils.c             CUtils.h \
						CX11Surface.c        CX11Surface.h

EXTRA_libCrayons_la_SOURCES = CPThreadMutex.c CPThreadMutex.h \
							  CNoThreadMutex.h

#libSystemDrawing_la_LDFLAGS = -version-info 0:0:0 -no-undefined \
#							  $(X_LIBS) $(X_PRE_LIBS) $(X11_LIB) $(X_EXTRA_LIBS)

#libSystemDrawing_la_LDADD = $(X_PRE_LIBS) $(X_LIBS) $(X_EXTRA_LIBS)

AM_CFLAGS = $(X_CFLAGS)
