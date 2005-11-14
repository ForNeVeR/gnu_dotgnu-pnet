/*
 * CFTFont.c - FreeType font implementation.
 *
 * Copyright (C) 2005  Free Software Foundation, Inc.
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 */

#if 0

#include "CFTFont.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct _tagCFTFontFamily CFTFontFamily;
struct _tagFTFontFamily
{
	CFontFamily    _base;
	FcPattern      *pattern;
	CMutex        *lock;
	CUInt32        refCount;
	CFontStyle     metricsStyle;
	CFontMetricsI  metrics;
};
typedef struct _tagCFTFont CFTFont;
struct _tagCFTFont
{
	CFont          _base;
	FT_Face        *face;
	CFloat         pixelSize;
	CFloat         size;
	CFontStyle     style;
	CGraphicsUnit  unit;
};
typedef struct _tagCFTFontCollection CFTFontCollection;
struct _tagCFTFontCollection
{
	CFontCollection  _base;
	FcConfig         *config;
	FcFontSet        *fonts;
};

/* maintain mutually exclusive access to the generic families */
static CMutex CFTFontFamily_GenericSerifLock     = CMutex_StaticInitializer;
static CMutex CFTFontFamily_GenericSansSerifLock = CMutex_StaticInitializer;
static CMutex CFTFontFamily_GenericMonospaceLock = CMutex_StaticInitializer;

/* declare generic families (created on demand) */
static CFTFontFamily *CFTFontFamily_GenericSerif     = 0;
static CFTFontFamily *CFTFontFamily_GenericSansSerif = 0;
static CFTFontFamily *CFTFontFamily_GenericMonospace = 0;

/* create the generic family names */
static const FcChar8 CFTFontFamily_GenericSerifName[]     = "Serif";
static const FcChar8 CFTFontFamily_GenericSansSerifName[] = "MS Sans Serif";
static const FcChar8 CFTFontFamily_GenericMonospaceName[] = "Monospace";

/* maintain mutually exclusive access to the freetype library */
static CMutex CFTFont_FreeTypeLibraryLock = CMutex_StaticInitializer;

/* declare freetype library (created on demand) */
static FT_Library CFTFont_FreeTypeLibrary;

/* set freetype library initialization to false */
static CBool CFTFont_FreeTypeLibraryInit = 0;

/*\
|*| NOTE: the font frontend should load these itself, based on configure-time
|*|       settings/checks, which is why the classes include creation methods,
|*|       even though this is a bit backwards... it may make more sense to
|*|       expose the backend creation methods outside the library, and have
|*|       users select font backends on their own, just as with surfaces,
|*|       in which case we'll need to initialize the base from here
\*/

extern const CFontCollectionClass CFTFontCollection_Class =
{
	CFTFontCollection_CreateInstalled,
	CFTFontCollection_CreatePrivate,
	CFTFontCollection_Destroy,
	CFTFontCollection_AddFontFile,
	CFTFontCollection_AddFontMemory,
	CFTFontCollection_GetFamilyList,
	"sentinel"
};

extern const CFontFamilyClass CFTFontFamily_Class =
{
	CFTFontFamily_CreateName,
	CFTFontFamily_CreateGeneric,
	CFTFontFamily_Destroy,
	CFTFontFamily_GetMetrics,
	CFTFontFamily_GetName,
	CFTFontFamily_IsStyleAvailable,
	"sentinel"
};

extern const CFontClass CFTFont_Class =
{
	CFTFontFamily_CreateName,
	CFTFontFamily_CreateGeneric,
	CFTFontFamily_Destroy,
	CFTFontFamily_GetGlyphMetrics,
	CFTFontFamily_GetMetrics,
	CFTFontFamily_GetName,
	CFTFontFamily_IsStyleAvailable,
	"sentinel"
};

CStatus
CFTFontCollection_CreateInstalled(CFontCollection **_this)
{
	/* ensure we have a this pointer pointer */
	CStatus_Require((_this != 0), CStatus_ArgumentNull);

	/* allocate the font collection */
	if(!(*_this = (CFontCollection *)CMalloc(sizeof(CFTFontCollection))))
	{
		return CStatus_OutOfMemory;
	}

	/* initialize the font collection */
	{
		/* declarations */
		CStatus            status;
		CFTFontCollection *fc;

		/* get this as a freetype font collection */
		fc = ((CFTFontCollection *)(*_this));

		/* set the members to the defaults */
		fc->config = 0;
		fc->fonts  = 0;

		/* update the font set */
		if((status = CFTFontCollection_UpdateFonts(fc)) != CStatus_OK)
		{
			CFree(*_this);
			*_this = 0;
			return status;
		}
	}

	/* return successfully */
	return CStatus_OK;
}

CStatus
CFTFontCollection_CreatePrivate(CFontCollection **_this)
{
	/* ensure we have a this pointer pointer */
	CStatus_Require((_this != 0), CStatus_ArgumentNull);

	/* allocate the font collection */
	if(!(*_this = (CFontCollection *)CMalloc(sizeof(CFTFontCollection))))
	{
		return CStatus_OutOfMemory;
	}

	/* initialize the font collection */
	{
		/* declarations */
		CFTFontCollection *fc;

		/* get this as a freetype font collection */
		fc = ((CFTFontCollection *)(*_this));

		/* create the font configuration */
		if(!(fc->config = FcConfigCreate()))
		{
			CFree(*_this);
			*_this = 0;
			return CStatus_OutOfMemory;
		}

		/* set the font set */
		fc->fonts = 0;
	}

	/* return successfully */
	return CStatus_OK;
}

CStatus
CFTFontCollection_Destroy(CFontCollection **_this)
{
	/* ensure we have a this pointer pointer */
	CStatus_Require((_this != 0), CStatus_ArgumentNull);

	/* finalize the font collection */
	{
		/* declarations */
		CFTFontCollection *fc;

		/* get this as a free type font collection */
		fc = ((CFTFontCollection *)(*_this));

		/* destroy the font set, as needed */
		if(fc->fonts != 0)
		{
			FcFontSetDestroy(fc->fonts);
		}

		/* destroy the font configuration, as needed */
		if(fc->config != 0)
		{
			FcConfigDestroy(fc->config);
		}
	}

	/* free the font collection */
	CFree(*_this);

	/* null the font collection */
	*_this = 0;

	/* return successfully */
	return CStatus_OK;
}

SStatus
CFTFontCollection_AddFontFile(CFontCollection *_this,
                               const CChar16   *filename)
{
	/* declarations */
	FcConfig *config;
	FcChar8  *fname;

	/* ensure we have a this pointer */
	CStatus_Require((_this != 0), CStatus_ArgumentNull);

	/* ensure we have a file name pointer */
	CStatus_Require((filename != 0), CStatus_ArgumentNull);

	/* get the font configuration */
	config = ((CFTFontCollection *)_this)->config;

	/* ensure we have a font configuration */
	CStatus_Require((config != 0), CStatus_Argument);

	/* convert the file name to utf8 */
	CStatus_Check
		(CUtils_Char16ToChar8
			(filename, (CChar8 **)&fname));

	/* add the font file */
	if(!(FcConfigAppFontAddFile(config, fname))
	{
		CFree(fname);
		return CStatus_OutOfMemory;
	}

	/* dispose of the utf8 file name */
	CFree(fname);

	/* return successfully */
	return CStatus_OK;
}


CStatus
CFTFontFamily_Destroy(CFontFamily **_this)
{
	/* declarations */
	CMutex *lock;
	CBool   lockOwner;

	/* ensure we have a this pointer pointer */
	CStatus_Require((_this != 0), CStatus_ArgumentNull);

	/* ensure we have a this pointer */
	CStatus_Require((*_this != 0), CStatus_ArgumentNull);

	/* get the lock */
	lock = (*_this)->lock;

	/* finalize and dispose of the font family */
	CMutex_Lock(lock);
	{
		/* declarations */
		CFTFontFamily **global;

		/* update the reference count */
		--((*_this)->refCount);

		/* finalize, as needed */
		if((*_this)->refCount == 0)
		{
			/* set the lock ownership flag to the default */
			lockOwner = 1;

			/* destroy the pattern */
			FcPatternDestroy((*_this)->pattern);

			/* null any global references and determine lock ownership */
			if((*_this) == CFTFontFamily_GenericSerif)
			{
				CFTFontFamily_GenericSerif = 0;
				lockOwner = 0;
			}
			else if((*_this) == CFTFontFamily_GenericSansSerif)
			{
				CFTFontFamily_GenericSansSerif = 0;
				lockOwner = 0;
			}
			else if((*_this) == CFTFontFamily_GenericMonospace)
			{
				CFTFontFamily_GenericMonospace = 0;
				lockOwner = 0;
			}

			/* dispose of the font family */
			CFree(*_this);
		}
		else
		{
			/* the remaining references own the lock */
			lockOwner = 0;
		}
	}
	CMutex_Unlock(lock);

	/* destroy the lock, as needed */
	if(lockOwner) { CMutex_Destroy(&lock); }

	/* null the font family pointer */
	*_this = 0;

	/* return successfully */
	return CStatus_OK;
}

static CStatus
CFTFontCollection_UpdateFonts(CFTFontCollection *_this)
{
	/* assertions */
	CASSERT((_this != 0));

	/* update fonts, as needed */
	if(_this->config != 0 || _this->fonts == 0)
	{
		/* declarations */
		FcObjectSet *objects;
		FcPattern   *pattern;
		FcFontSet   *tmp;

		/* create the font config obect set */
		if(!(objects = FcObjectSetBuild(FC_FAMILY, FC_FOUNDRY, FC_SCALABLE, 0)))
		{
			return CStatus_OutOfMemory;
		}

		/* create the font config pattern */
		if(!(pattern = FcPatternBuild(0, FC_SCALABLE, FcTypeBool, FcTrue, 0)))
		{
			FcObjectSetDestroy(objects);
			return CStatus_OutOfMemory;
		}

		/* get all the scalable fonts */
		tmp = FcFontList(_this->config, pattern, objects);

		/* destroy the font config object set */
		FcObjectSetDestroy(objects);

		/* destroy the font config pattern */
		FcPatternDestroy(pattern);

		/* ensure we have a font set */
		CStatus_Require((tmp != 0), CStatus_OutOfMemory);

		/* dispose of the old font set, as needed */
		if(_this->fonts != 0)
		{
			FcFontSetDestroy(_this->fonts);
		}

		/* set the font set */
		_this->fonts = tmp;
	}

	/* return successfully */
	return CStatus_OK;
}

CStatus
CFTFontCollection_GetFamilyList(CFontCollection   *_this,
                                 CFontFamily     ***families,
                                 CUInt32           *count)
{
	/* declarations */
	CFTFontCollection *fc;
	CUInt32            size;

	/* ensure we have a this pointer */
	CStatus_Require((_this != 0), CStatus_ArgumentNull);

	/* ensure we have a font family list pointer */
	CStatus_Require((families != 0), CStatus_ArgumentNull);

	/* ensure we have a count pointer */
	CStatus_Require((count != 0), CStatus_ArgumentNull);

	/* get this as a freetype font collection */
	fc = (CFTFontCollection *)_this;

	/* update the font set */
	CStatus_Check(CFTFontCollection_UpdateFonts(fc));

	/* get the count */
	*count = fontCollection->fonts->nfont;

	/* calculate the list size */
	size = ((*count) * sizeof(CFTFontFamily *));

	/* allocate the font family list */
	if(!(*families = (CFontFamily **)CMalloc(size)))
	{
		return CStatus_OutOfMemory;
	}

	/* get the font families */
	{
		/* declarations */
		FcPattern      **currP;
		CFTFontFamily **currF;
		CFTFontFamily **start;
		CFTFontFamily **end;

		/* get the pattern pointer */
		currP = fontCollection->fonts->fonts;

		/* get the font family pointer */
		currF = ((CFTFontFamily **)(*families));

		/* get the start pointer */
		start = (currF - 1);

		/* get the end pointer */
		end = (currF + (*count));

		/* create the font families */
		while(currF != end)
		{
			/* declarations */
			CStatus status;

			/* allocate the font family */
			if(!(*currF = (CFTFontFamily *)CMalloc(sizeof(CFTFontFamily))))
			{
				/* destroy all the previously created families */
				while(*currF != start)
				{
					--currF;
					CFTFontFamily_Destroy(currF);
				}

				/* finish clean up */
				CFree(*families);
				*families = 0;
				*count    = 0;

				/* return out of memory status */
				return CStatus_OutOfMemory;
			}

			/* reference the current pattern */
			FcPatternReference(*currP);

			/* initialize the current font family */
			status = CFTFontFamily_Initialize(*currF, *currP);

			/* handle initialization errors */
			if(status != CStatus_OK)
			{
				/* dereference the current pattern */
				FcPatternDestroy(*currP);

				/* dispose of the current font family */
				CFree(*currF);

				/* destroy all the previously created families */
				while(*currF != start)
				{
					--currF;
					CFTFontFamily_Destroy(currF);
				}

				/* finish clean up */
				CFree(*families);
				*families = 0;
				*count    = 0;

				/* return status */
				return status;
			}

			/* move to the next position */
			++currP; ++currF;
		}
	}

	/* return successfully */
	return CStatus_OK;
}

static CStatus
CFTFontFamily_Initialize(CFTFontFamily *_this,
                          FcPattern      *pattern)
{
	/* assertions */
	CASSERT((_this   != 0));
	CASSERT((pattern != 0));

	/* create the mutex */
	CStatus_Check
		(CMutex_Create
			(&(_this->lock)));

	/* set the metrics style to the default */
	_this->metricsStyle = (CFontStyle)-1;

	/* set the pattern */
	_this->pattern = pattern;

	/* return successfully */
	return CStatus_OK;
}

CStatus
CFTFontFamily_CreateName(CFontFamily     **_this,
                          const CChar16    *name,
                          CFontCollection  *fc)
{
	/* ensure we have a this pointer pointer */
	CStatus_Require((_this != 0), CStatus_ArgumentNull);

	/* ensure we have a this pointer */
	CStatus_Require((*_this != 0), CStatus_ArgumentNull);

	/* ensure we have a name pointer */
	CStatus_Require((name != 0), CStatus_ArgumentNull);

	/* allocate the font family */
	if(!(*_this = (CFontFamily *)CMalloc(sizeof(CFTFontFamily))))
	{
		CFree(fname);
		return CStatus_OutOfMemory;
	}

	/* initialize the font family */
	{
		/* declarations */
		FcResult        result;
		FcChar8        *fname;
		FcPattern      *pattern;
		FcConfig       *config;
		CFTFontFamily *family;
		CStatus        status;

		/* get the font configuration, if available */
		if(fc != 0)
		{
			config = ((CFTFontCollection *)fc)->config;
		}
		else
		{
			config = 0;
		}

		/* get the font name in utf8 */
		status =
			CUtils_Char16ToChar8
				(name, (CChar8 **)&fname);

		/* handle utf8 conversion failures */
		if(status != CStatus_OK)
		{
			CFree(*_this);
			*_this = 0;
			return status;
		}

		/* build the pattern */
		if((status = _BuildPattern(&pattern, config, fname)) != CStatus_OK)
		{
			CFree(fname);
			CFree(*_this);
			*_this = 0;
			return status;
		}

		/* dispose of the utf8 font name */
		CFree(fname);

		/* initialize the font family */
		if((status = CFTFontFamily_Initialize(family, pattern)) != CStatus_OK)
		{
			FcPatternDestroy(pattern);
			CFree(*_this);
			*_this = 0;
			return status;
		}
	}

	/* return successfully */
	return CStatus_OK;
}

static CStatus
_BuildPattern(FcPattern **fpattern,
              FcConfig   *config,
              FcChar8    *name)
{
	/* declarations */
	FcPattern *pattern;
	FcResult   result;

	/* create the font config pattern */
	pattern =
		FcPatternBuild
			(0,
			 FC_FAMILY,   FcTypeString, name,
			 FC_SCALABLE, FcTypeBool,   FcTrue,
			 0);

	/* handle pattern creation errors */
	if(!pattern)
	{
		*fpattern = 0;
		return CStatus_OutOfMemory;
	}

	/* TODO: figure out the optimal order for the next two ops */

	/* set up the pattern for font matching */
	FcConfigSubstitute(config, pattern, FcMatchPattern);
	FcDefaultSubstitute(pattern);

	/* get the family pattern */
	if(!(*fpattern = FcFontMatch(config, pattern, &result)))
	{
		/* clean up */
		FcPatternDestroy(pattern);

		/* return status based on result */
		if(result == FcResultNoMatch)
		{
			return CStatus_Argument_FontFamilyNotFound;
		}
		else
		{
			return CStatus_OutOfMemory;
		}
	}

	/* destroy the pattern */
	FcPatternDestroy(pattern);

	/* return successfully */
	return CStatus_OK;
}

CStatus
CFTFontFamily_CreateGeneric(CFontFamily          **_this,
                             CGenericFontFamilies   generic)
{
	/* ensure we have a this pointer pointer */
	CStatus_Require((_this != 0), CStatus_ArgumentNull);

	/* initialize the font family */
	{
		/* declarations */
		CFTFontFamily **global;
		CMutex         *lock;
		FcChar8         *name;

		/* get the lock and name, based on the generic type */
		switch(generic)
		{
			case CGenericFontFamilies_Serif:
			{
				global = &(CFTFontFamily_GenericSerif);
				lock   = &(CFTFontFamily_GenericSerifLock);
				name   = CFTFontFamily_GenericSerifName;
			}
			break;
			case CGenericFontFamilies_SansSerif:
			{
				global = &(CFTFontFamily_GenericSansSerif);
				lock   = &(CFTFontFamily_GenericSansSerifLock);
				name   = CFTFontFamily_GenericSansSerifName;
			}
			break;
			case CGenericFontFamilies_Monospace:
			default:
			{
				global = &(CFTFontFamily_GenericMonospace);
				lock   = &(CFTFontFamily_GenericMonospaceLock);
				name   = CFTFontFamily_GenericMonospaceName;
			}
			break;
		}

		/* get the generic font family, synchronously */
		CMutex_Lock(lock);
		{
			/* create the generic, as needed */
			if(*global == 0)
			{
				/* declarations */
				CStatus   status;
				FcPattern *pattern;

				/* allocate the font family */
				if(!(*global = (CFontFamily *)CMalloc(sizeof(CFTFontFamily))))
				{
					/* unlock global space */
					CMutex_Unlock(lock);

					/* finish clean	up */
					*_this  = 0;

					/* return out of memory status */
					return CStatus_OutOfMemory;
				}

				/* build the pattern */
				if((status _BuildPattern(&pattern, 0, name)) != CStatus_OK)
				{
					/* clean up global space */
					CFree(*global);
					*global = 0;

					/* unlock global space */
					CMutex_Unlock(lock);

					/* finish clean up */
					*_this  = 0;

					/* return status */
					return status;
				}

				/* initialize the members */
				(*global)->lock         = lock;
				(*global)->metricsStyle = (CFontStyle)-1;
				(*global)->pattern      = pattern;
			}

			/* set the font family to the global generic family */
			*_this = *global;
		}
		CMutex_Unlock(lock);
	}

	/* return successfully */
	return CStatus_OK;
}

CStatus
CFTFontFamily_GetMetrics(CFontFamily   *_this,
                          CFontStyle     style,
                          CFontMetricsI *metrics)
{
	/* declarations */
	CFTFontFamily *family;
	CFTFont       *font;
	CStatus        status;

	/* ensure we have a this pointer */
	CStatus_Require((_this != 0), CStatus_ArgumentNull);

	/* ensure we have a metrics pointer */
	CStatus_Require((metrics != 0), CStatus_ArgumentNull);

	/* get this as a freetype font family */
	family = (CFontFamily *)_this;

	/* get the family metrics, synchronously */
	CMutex_Lock(family->lock);
	{
		/* get the cached metrics, if available */
		if(family->metricsStyle == style)
		{
			/* get the cached metrics */
			*metrics = family->metrics;

			/* unlock the font family*/
			CMutex_Unlock(family->lock);

			/* return successfully */
			return CStatus_OK;
		}
	}
	CMutex_Unlock(family->lock);

	/*\
	|*| NOTE: we get the metrics outside the lock to avoid blocking during
	|*|       the long font creation process, and to avoid recursive locking
	|*|       once we put font and/or face caching in place
	\*/

	/* create the font */
	status =
		CFTFont_Create
			(&((CFont *)font), family, 0.0f, style, CGraphicsUnit_Pixel);

	/* handle font creation failures */
	if(status != CStatus_OK) { return status; }

	/* get the font metrics */
	status = CFTFont_GetMetricsI(((CFont *)font), metrics);

	/* destroy the font */
	CFTFont_Destroy(&((CFont *)font));

	/* handle metrics failures */
	if(status != CStatus_OK) { return status; }

	/* cache the family metrics, synchronously */
	CMutex_Lock(family->lock);
	{
		/*\
		|*| NOTE: we don't bother to recheck for caching, because the odds
		|*|       are low that another thread beat us to it, and assignments
		|*|       are cheaper than branches
		\*/

		/* set the cached metrics */
		family->metrics      = metrics;
		family->metricsStyle = style;
	}
	CMutex_Unlock(family->lock);

	/* return successfully */
	return CStatus_OK;
}

CStatus
CFTFontFamily_IsStyleAvailable(CFontFamily *_this,
                                CFontStyle   style,
                                CBool       *available)
{
	/* ensure we have a this pointer */
	CStatus_Require((_this != 0), CStatus_ArgumentNull);

	/* ensure we have a style available flag pointer */
	CStatus_Require((available != 0), CStatus_ArgumentNull);

	/* assume all styles are available */
	*available = 1;

	/* return successfully */
	return CStatus_OK;
}
CStatus
CFTFont_GetMetricsI(CFont         *_this,
                     CFontMetricsI *metrics)
{
	/* ensure we have a this pointer */
	CStatus_Require((_this != 0), CStatus_ArgumentNull);

	/* ensure we have a metrics pointer */
	CStatus_Require((metrics != 0), CStatus_ArgumentNull);

	/* get the metrics */
	{
		/* declarations */
		FT_Face face;

		/* get the face */
		face = _this->face;

		/* assertions */
		CASSERT((FT_IS_SCALABLE(face)));

		/* get the metrics information */
		metrics->emHeight    = face->units_per_EM;
		metrics->cellAscent  = face->ascender;
		metrics->cellDescent = -(face->descender);
		metrics->lineSpacing = face->height;
	}

	/* return successfully */
	return CStatus_OK;
}

CStatus
CFTFont_GetMetricsF(CFont         *_this,
                     CFontMetricsF *metrics)
{
	/* ensure we have a this pointer */
	CStatus_Require((_this != 0), CStatus_ArgumentNull);

	/* ensure we have a metrics pointer */
	CStatus_Require((metrics != 0), CStatus_ArgumentNull);

	/* get the metrics */
	{
		/* declarations */
		FT_Face face;
		CFloat pixels;

		/* get the face */
		face = _this->face;

		/* assertions */
		CASSERT((FT_IS_SCALABLE(face)));

		/* get the pixel size */
		pixels = _this->pixelSize;

		/* get the metrics information */
		metrics->emHeight    = face->units_per_EM;
		metrics->cellAscent  = pixels * (face->ascender / metrics->emHeight);
		metrics->cellDescent = pixels * -(face->descender / metrics->emHeight);
		metrics->lineSpacing = pixels * (face->height / metrics->emHeight);
	}

	/* return successfully */
	return CStatus_OK;
}

CStatus
CFTFont_Create(CFont         **_this,
                CFontFamily    *family,
                CFontStyle      style,
                CFloat          size,
                CGraphicsUnit   unit)
{
	/* ensure we have a this pointer pointer */
	CStatus_Require((_this != 0), CStatus_ArgumentNull);

	/* ensure we have a font family pointer */
	CStatus_Require((family != 0), CStatus_ArgumentNull);

	/* allocate the font */
	if(!(*_this = (CFont *)CMalloc(sizeof(CFTFont))))
	{
		return CStatus_OutOfMemory;
	}

	/* initialize the font */
	{
		/* declarations */
		int              slant;
		int              weight;
		FT_Error         error;
		FT_Face          face;
		FcChar8         *name;
		FcChar8        **filename;
		FcPattern       *pattern;
		FcPattern       *match;
		FcResult         result;
		CFloat          pointSize;
		CFTFont        *font;
		CFTFontFamily  *ff;

		/* get the family as a freetype family */
		ff = (CFTFontFamily *)family;

		/* get the family name */
		result = FcPatternGetString(ff->pattern, FC_FAMILY, 0, &name);

		/* assertions */
		CASSERT((result == FcResultMatch));

		/* get this as a freetype font */
		font = ((CFTFont *)(*_this));

		/* calculate size in pixels */
		font->pixelSize =
			CUtils_ConvertUnits
				(unit, CGraphicsUnit_Pixel, size);

		/* initialize the remaining simple members */
		font->size  = size;
		font->unit  = unit;
		font->style = style;

		/* get the font weight */
		if((style & CFontStyle_Bold) == CFontStyle_Bold)
		{
			weight = FC_WEIGHT_BOLD;
		}
		else
		{
			weight = FC_WEIGHT_LIGHT;
		}

		/* get the font slant */
		if((style & CFontStyle_Italic) == CFontStyle_Italic)
		{
			slant = FC_SLANT_ITALIC;
		}
		else
		{
			slant = FC_SLANT_ROMAN;
		}

		/* ensure library is initialized, synchronously */
		CMutex_Lock(&(CFTFont_FreeTypeLibraryLock));
		{
			/* initialize the library, as needed */
			if(!CFTFont_FreeTypeLibraryInit)
			{
				/* initialize the freetype library */
				if((error = FT_Init_FreeType(CFTFont_FreeTypeLibrary)))
				{
					/* assume out of memory for now */
					CMutex_Unlock(&(CFTFont_FreeTypeLibraryLock));
					CFree(*_this);
					*_this = 0;
					return CStatus_OutOfMemory;
				}

				/* set the freetype library initialized flag to true */
				CFTFont_FreeTypeLibraryInit = 1;
			}
		}
		CMutex_Unlock(&(CFTFont_FreeTypeLibraryLock));

		/* build the pattern */
		pattern =
			FcPatternBuild
				(0,
				 FC_FAMILY,   FcTypeString,  name,
				 FC_SCALABLE, FcTypeBool,    FcTrue,
				 FC_SLANT,    FcTypeInteger, slant,
				 FC_WEIGHT,   FcTypeInteger, weight,
				 0);

		/* handle pattern creation failures */
		if(!pattern)
		{
			CFree(*_this);
			*_this = 0;
			return CStatus_OutOfMemory;
		}

		/* set up pattern for font matching */
		FcConfigSubstitute(0, pattern, FcMatchPattern);
		FcDefaultSubstitute(pattern);

		/* get the matching pattern */
		if(!(match = FcFontMatch(0, pattern, &result)))
		{
			/* clean up */
			FcPatternDestroy(pattern);
			CFree(*_this);
			*_this = 0;

			/* return status based on result */
			if(result == FcResultNoMatch)
			{
				return CStatus_Argument_FontFamilyNotFound;
			}
			else
			{
				return CStatus_OutOfMemory;
			}
		}

		/* destroy the pattern */
		FcPatternDestroy(pattern);

		/* get the file name for the font */
		result = FcPatternGetString(match, FC_FILE, 0, filename);

		/* handle file name getter failures */
		if(result != FcResultMatch)
		{
			/* clean up */
			FcPatternDestroy(match);
			CFree(*_this);
			*_this = 0;

			/* return status based on result */
			if(result == FcResultNoMatch)
			{
				return CStatus_Argument_FontFamilyNotFound;
			}
			else
			{
				return CStatus_OutOfMemory;
			}
		}

		/* try to create the face from the filename */
		if((error = FT_New_Face(ft_library, filename, 0, &face)))
		{
			/* assume out of memory for now */
			FcPatternDestroy(match);
			CFree(*_this);
			*_this = 0;
			return CStatus_OutOfMemory;
		}

		/* destroy the match pattern */
		FcPatternDestroy(match);

		/* calculate size in points */
		pointSize =
			CUtils_ConvertUnits
				(unit, CGraphicsUnit_Pixel, size);

		/*\
		|*| NOTE: we don't use FT_Set_Pixel_Sizes to avoid rounding away
		|*|       subpixels before glyph transformation
		\*/

		/* set the point size */
		FT_Set_Char_Size
			(face,
			 DOUBLE_TO_26_6(pointSize), DOUBLE_TO_26_6(pointSize),
			 CGraphics_DefaultDpiY,    CGraphics_DefaultDpiY);

		/* set the face */
		font->face = face;
	}

	/* return successfully */
	return CStatus_OK;
}

CStatus
CFTFont_Destroy(CFont **_this)
{
	/* ensure we have a this pointer pointer */
	CStatus_Require((_this != 0), CStatus_ArgumentNull);

	/* ensure we have a this pointer */
	CStatus_Require((*_this != 0), CStatus_ArgumentNull);

	/* finalize the font */
	{
		/* finalize the face */
		FT_Done_Face(face);
	}

	/* return successfully */
	return CStatus_OK;
}

CStatus
CFTFontCollection_AddFontMemory(CFontCollection *_this,
                                 const CByte     *memory,
                                 CUInt32          length)
{
	/* ensure we have a this pointer */
	CStatus_Require((_this != 0), CStatus_ArgumentNull);

	/* ensure we have a memory pointer */
	CStatus_Require((memory != 0), CStatus_ArgumentNull);

	/* ensure we have memory */
	CStatus_Require((length != 0), CStatus_Argument);

	/* add the memory font */
	{
		/* declarations */
		int fd, len;

		/* create the temporary file name */
		CByte fname[] = "/tmp/XXXXXXXXXXXXXXXX";

		/* create the temporary file */
		if((fd = mkstemp((char *)fname)) == -1) { return CStatus_Error; }

		/* get the length */
		len = (int)length;

		/* output the data to the file */
		while((len = write(fd, memory, len)) > 0) { /* write */ }

		/* close the file */
		close(fd);

		/* handle write errors */
		if(len == -1) { return CStatus_Error; }

		/* add the font file to the collection */
		FcConfigAppFontAddFile(((CFTFontCollection *)_this)->config, fname);
	}

	/* return successfully */
	return CStatus_OK;
}

CStatus
CFTFont_GetGlyphMetrics(CFont    *_this,
                         CChar16  *s,
                         CSizeF  **metrics,
                         CUInt32  *count)
{
	/* TODO */
	return CStatus_NotImplemented;
}


#ifdef __cplusplus
};
#endif

#endif
