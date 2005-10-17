/*
 * SDFTFont.c - FreeType font implementation.
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

typedef struct _tagSDFTFontFamily SDFTFontFamily;
struct _tagFTFontFamily
{
	SDFontFamily    _base;
	FcPattern      *pattern;
	SDMutex        *lock;
	SDUInt32        refCount;
	SDFontStyle     metricsStyle;
	SDFontMetricsI  metrics;
};
typedef struct _tagSDFTFont SDFTFont;
struct _tagSDFTFont
{
	SDFont          _base;
	FT_Face        *face;
	SDFloat         pixelSize;
	SDFloat         size;
	SDFontStyle     style;
	SDGraphicsUnit  unit;
};
typedef struct _tagSDFTFontCollection SDFTFontCollection;
struct _tagSDFTFontCollection
{
	SDFontCollection  _base;
	FcConfig         *config;
	FcFontSet        *fonts;
};

/* maintain mutually exclusive access to the generic families */
static SDMutex SDFTFontFamily_GenericSerifLock     = SDMutex_StaticInitializer;
static SDMutex SDFTFontFamily_GenericSansSerifLock = SDMutex_StaticInitializer;
static SDMutex SDFTFontFamily_GenericMonospaceLock = SDMutex_StaticInitializer;

/* declare generic families (created on demand) */
static SDFTFontFamily *SDFTFontFamily_GenericSerif     = 0;
static SDFTFontFamily *SDFTFontFamily_GenericSansSerif = 0;
static SDFTFontFamily *SDFTFontFamily_GenericMonospace = 0;

/* create the generic family names */
static const FcChar8 SDFTFontFamily_GenericSerifName[]     = "Serif";
static const FcChar8 SDFTFontFamily_GenericSansSerifName[] = "MS Sans Serif";
static const FcChar8 SDFTFontFamily_GenericMonospaceName[] = "Monospace";

/* maintain mutually exclusive access to the freetype library */
static SDMutex SDFTFont_FreeTypeLibraryLock = SDMutex_StaticInitializer;

/* declare freetype library (created on demand) */
static FT_Library SDFTFont_FreeTypeLibrary;

/* set freetype library initialization to false */
static SDBool SDFTFont_FreeTypeLibraryInit = 0;

/*\
|*| NOTE: the font frontend should load these itself, based on configure-time
|*|       settings/checks, which is why the classes include creation methods,
|*|       even though this is a bit backwards... it may make more sense to
|*|       expose the backend creation methods outside the library, and have
|*|       users select font backends on their own, just as with surfaces,
|*|       in which case we'll need to initialize the base from here
\*/

extern const SDFontCollectionClass SDFTFontCollection_Class =
{
	SDFTFontCollection_CreateInstalled,
	SDFTFontCollection_CreatePrivate,
	SDFTFontCollection_Destroy,
	SDFTFontCollection_AddFontFile,
	SDFTFontCollection_AddFontMemory,
	SDFTFontCollection_GetFamilyList,
	"sentinel"
};

extern const SDFontFamilyClass SDFTFontFamily_Class =
{
	SDFTFontFamily_CreateName,
	SDFTFontFamily_CreateGeneric,
	SDFTFontFamily_Destroy,
	SDFTFontFamily_GetMetrics,
	SDFTFontFamily_GetName,
	SDFTFontFamily_IsStyleAvailable,
	"sentinel"
};

extern const SDFontClass SDFTFont_Class =
{
	SDFTFontFamily_CreateName,
	SDFTFontFamily_CreateGeneric,
	SDFTFontFamily_Destroy,
	SDFTFontFamily_GetGlyphMetrics,
	SDFTFontFamily_GetMetrics,
	SDFTFontFamily_GetName,
	SDFTFontFamily_IsStyleAvailable,
	"sentinel"
};

SDStatus
SDFTFontCollection_CreateInstalled(SDFontCollection **_this)
{
	/* ensure we have a this pointer pointer */
	SDStatus_Require((_this != 0), SDStatus_ArgumentNull);

	/* allocate the font collection */
	if(!(*_this = (SDFontCollection *)SDMalloc(sizeof(SDFTFontCollection))))
	{
		return SDStatus_OutOfMemory;
	}

	/* initialize the font collection */
	{
		/* declarations */
		SDStatus            status;
		SDFTFontCollection *fc;

		/* get this as a freetype font collection */
		fc = ((SDFTFontCollection *)(*_this));

		/* set the members to the defaults */
		fc->config = 0;
		fc->fonts  = 0;

		/* update the font set */
		if((status = SDFTFontCollection_UpdateFonts(fc)) != SDStatus_OK)
		{
			SDFree(*_this);
			*_this = 0;
			return status;
		}
	}

	/* return successfully */
	return SDStatus_OK;
}

SDStatus
SDFTFontCollection_CreatePrivate(SDFontCollection **_this)
{
	/* ensure we have a this pointer pointer */
	SDStatus_Require((_this != 0), SDStatus_ArgumentNull);

	/* allocate the font collection */
	if(!(*_this = (SDFontCollection *)SDMalloc(sizeof(SDFTFontCollection))))
	{
		return SDStatus_OutOfMemory;
	}

	/* initialize the font collection */
	{
		/* declarations */
		SDFTFontCollection *fc;

		/* get this as a freetype font collection */
		fc = ((SDFTFontCollection *)(*_this));

		/* create the font configuration */
		if(!(fc->config = FcConfigCreate()))
		{
			SDFree(*_this);
			*_this = 0;
			return SDStatus_OutOfMemory;
		}

		/* set the font set */
		fc->fonts = 0;
	}

	/* return successfully */
	return SDStatus_OK;
}

SDStatus
SDFTFontCollection_Destroy(SDFontCollection **_this)
{
	/* ensure we have a this pointer pointer */
	SDStatus_Require((_this != 0), SDStatus_ArgumentNull);

	/* finalize the font collection */
	{
		/* declarations */
		SDFTFontCollection *fc;

		/* get this as a free type font collection */
		fc = ((SDFTFontCollection *)(*_this));

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
	SDFree(*_this);

	/* null the font collection */
	*_this = 0;

	/* return successfully */
	return SDStatus_OK;
}

SStatus
SDFTFontCollection_AddFontFile(SDFontCollection *_this,
                               const SDChar16   *filename)
{
	/* declarations */
	FcConfig *config;
	FcChar8  *fname;

	/* ensure we have a this pointer */
	SDStatus_Require((_this != 0), SDStatus_ArgumentNull);

	/* ensure we have a file name pointer */
	SDStatus_Require((filename != 0), SDStatus_ArgumentNull);

	/* get the font configuration */
	config = ((SDFTFontCollection *)_this)->config;

	/* ensure we have a font configuration */
	SDStatus_Require((config != 0), SDStatus_Argument);

	/* convert the file name to utf8 */
	SDStatus_Check
		(SDUtils_Char16ToChar8
			(filename, (SDChar8 **)&fname));

	/* add the font file */
	if(!(FcConfigAppFontAddFile(config, fname))
	{
		SDFree(fname);
		return SDStatus_OutOfMemory;
	}

	/* dispose of the utf8 file name */
	SDFree(fname);

	/* return successfully */
	return SDStatus_OK;
}


SDStatus
SDFTFontFamily_Destroy(SDFontFamily **_this)
{
	/* declarations */
	SDMutex *lock;
	SDBool   lockOwner;

	/* ensure we have a this pointer pointer */
	SDStatus_Require((_this != 0), SDStatus_ArgumentNull);

	/* ensure we have a this pointer */
	SDStatus_Require((*_this != 0), SDStatus_ArgumentNull);

	/* get the lock */
	lock = (*_this)->lock;

	/* finalize and dispose of the font family */
	SDMutex_Lock(lock);
	{
		/* declarations */
		SDFTFontFamily **global;

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
			if((*_this) == SDFTFontFamily_GenericSerif)
			{
				SDFTFontFamily_GenericSerif = 0;
				lockOwner = 0;
			}
			else if((*_this) == SDFTFontFamily_GenericSansSerif)
			{
				SDFTFontFamily_GenericSansSerif = 0;
				lockOwner = 0;
			}
			else if((*_this) == SDFTFontFamily_GenericMonospace)
			{
				SDFTFontFamily_GenericMonospace = 0;
				lockOwner = 0;
			}

			/* dispose of the font family */
			SDFree(*_this);
		}
		else
		{
			/* the remaining references own the lock */
			lockOwner = 0;
		}
	}
	SDMutex_Unlock(lock);

	/* destroy the lock, as needed */
	if(lockOwner) { SDMutex_Destroy(&lock); }

	/* null the font family pointer */
	*_this = 0;

	/* return successfully */
	return SDStatus_OK;
}

static SDStatus
SDFTFontCollection_UpdateFonts(SDFTFontCollection *_this)
{
	/* assertions */
	SDASSERT((_this != 0));

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
			return SDStatus_OutOfMemory;
		}

		/* create the font config pattern */
		if(!(pattern = FcPatternBuild(0, FC_SCALABLE, FcTypeBool, FcTrue, 0)))
		{
			FcObjectSetDestroy(objects);
			return SDStatus_OutOfMemory;
		}

		/* get all the scalable fonts */
		tmp = FcFontList(_this->config, pattern, objects);

		/* destroy the font config object set */
		FcObjectSetDestroy(objects);

		/* destroy the font config pattern */
		FcPatternDestroy(pattern);

		/* ensure we have a font set */
		SDStatus_Require((tmp != 0), SDStatus_OutOfMemory);

		/* dispose of the old font set, as needed */
		if(_this->fonts != 0)
		{
			FcFontSetDestroy(_this->fonts);
		}

		/* set the font set */
		_this->fonts = tmp;
	}

	/* return successfully */
	return SDStatus_OK;
}

SDStatus
SDFTFontCollection_GetFamilyList(SDFontCollection   *_this,
                                 SDFontFamily     ***families,
                                 SDUInt32           *count)
{
	/* declarations */
	SDFTFontCollection *fc;
	SDUInt32            size;

	/* ensure we have a this pointer */
	SDStatus_Require((_this != 0), SDStatus_ArgumentNull);

	/* ensure we have a font family list pointer */
	SDStatus_Require((families != 0), SDStatus_ArgumentNull);

	/* ensure we have a count pointer */
	SDStatus_Require((count != 0), SDStatus_ArgumentNull);

	/* get this as a freetype font collection */
	fc = (SDFTFontCollection *)_this;

	/* update the font set */
	SDStatus_Check(SDFTFontCollection_UpdateFonts(fc));

	/* get the count */
	*count = fontCollection->fonts->nfont;

	/* calculate the list size */
	size = ((*count) * sizeof(SDFTFontFamily *));

	/* allocate the font family list */
	if(!(*families = (SDFontFamily **)SDMalloc(size)))
	{
		return SDStatus_OutOfMemory;
	}

	/* get the font families */
	{
		/* declarations */
		FcPattern      **currP;
		SDFTFontFamily **currF;
		SDFTFontFamily **start;
		SDFTFontFamily **end;

		/* get the pattern pointer */
		currP = fontCollection->fonts->fonts;

		/* get the font family pointer */
		currF = ((SDFTFontFamily **)(*families));

		/* get the start pointer */
		start = (currF - 1);

		/* get the end pointer */
		end = (currF + (*count));

		/* create the font families */
		while(currF != end)
		{
			/* declarations */
			SDStatus status;

			/* allocate the font family */
			if(!(*currF = (SDFTFontFamily *)SDMalloc(sizeof(SDFTFontFamily))))
			{
				/* destroy all the previously created families */
				while(*currF != start)
				{
					--currF;
					SDFTFontFamily_Destroy(currF);
				}

				/* finish clean up */
				SDFree(*families);
				*families = 0;
				*count    = 0;

				/* return out of memory status */
				return SDStatus_OutOfMemory;
			}

			/* reference the current pattern */
			FcPatternReference(*currP);

			/* initialize the current font family */
			status = SDFTFontFamily_Initialize(*currF, *currP);

			/* handle initialization errors */
			if(status != SDStatus_OK)
			{
				/* dereference the current pattern */
				FcPatternDestroy(*currP);

				/* dispose of the current font family */
				SDFree(*currF);

				/* destroy all the previously created families */
				while(*currF != start)
				{
					--currF;
					SDFTFontFamily_Destroy(currF);
				}

				/* finish clean up */
				SDFree(*families);
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
	return SDStatus_OK;
}

static SDStatus
SDFTFontFamily_Initialize(SDFTFontFamily *_this,
                          FcPattern      *pattern)
{
	/* assertions */
	SDASSERT((_this   != 0));
	SDASSERT((pattern != 0));

	/* create the mutex */
	SDStatus_Check
		(SDMutex_Create
			(&(_this->lock)));

	/* set the metrics style to the default */
	_this->metricsStyle = (SDFontStyle)-1;

	/* set the pattern */
	_this->pattern = pattern;

	/* return successfully */
	return SDStatus_OK;
}

SDStatus
SDFTFontFamily_CreateName(SDFontFamily     **_this,
                          const SDChar16    *name,
                          SDFontCollection  *fc)
{
	/* ensure we have a this pointer pointer */
	SDStatus_Require((_this != 0), SDStatus_ArgumentNull);

	/* ensure we have a this pointer */
	SDStatus_Require((*_this != 0), SDStatus_ArgumentNull);

	/* ensure we have a name pointer */
	SDStatus_Require((name != 0), SDStatus_ArgumentNull);

	/* allocate the font family */
	if(!(*_this = (SDFontFamily *)SDMalloc(sizeof(SDFTFontFamily))))
	{
		SDFree(fname);
		return SDStatus_OutOfMemory;
	}

	/* initialize the font family */
	{
		/* declarations */
		FcResult        result;
		FcChar8        *fname;
		FcPattern      *pattern;
		FcConfig       *config;
		SDFTFontFamily *family;
		SDStatus        status;

		/* get the font configuration, if available */
		if(fc != 0)
		{
			config = ((SDFTFontCollection *)fc)->config;
		}
		else
		{
			config = 0;
		}

		/* get the font name in utf8 */
		status =
			SDUtils_Char16ToChar8
				(name, (SDChar8 **)&fname);

		/* handle utf8 conversion failures */
		if(status != SDStatus_OK)
		{
			SDFree(*_this);
			*_this = 0;
			return status;
		}

		/* build the pattern */
		if((status = _BuildPattern(&pattern, config, fname)) != SDStatus_OK)
		{
			SDFree(fname);
			SDFree(*_this);
			*_this = 0;
			return status;
		}

		/* dispose of the utf8 font name */
		SDFree(fname);

		/* initialize the font family */
		if((status = SDFTFontFamily_Initialize(family, pattern)) != SDStatus_OK)
		{
			FcPatternDestroy(pattern);
			SDFree(*_this);
			*_this = 0;
			return status;
		}
	}

	/* return successfully */
	return SDStatus_OK;
}

static SDStatus
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
		return SDStatus_OutOfMemory;
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
			return SDStatus_Argument_FontFamilyNotFound;
		}
		else
		{
			return SDStatus_OutOfMemory;
		}
	}

	/* destroy the pattern */
	FcPatternDestroy(pattern);

	/* return successfully */
	return SDStatus_OK;
}

SDStatus
SDFTFontFamily_CreateGeneric(SDFontFamily          **_this,
                             SDGenericFontFamilies   generic)
{
	/* ensure we have a this pointer pointer */
	SDStatus_Require((_this != 0), SDStatus_ArgumentNull);

	/* initialize the font family */
	{
		/* declarations */
		SDFTFontFamily **global;
		SDMutex         *lock;
		FcChar8         *name;

		/* get the lock and name, based on the generic type */
		switch(generic)
		{
			case SDGenericFontFamilies_Serif:
			{
				global = &(SDFTFontFamily_GenericSerif);
				lock   = &(SDFTFontFamily_GenericSerifLock);
				name   = SDFTFontFamily_GenericSerifName;
			}
			break;
			case SDGenericFontFamilies_SansSerif:
			{
				global = &(SDFTFontFamily_GenericSansSerif);
				lock   = &(SDFTFontFamily_GenericSansSerifLock);
				name   = SDFTFontFamily_GenericSansSerifName;
			}
			break;
			case SDGenericFontFamilies_Monospace:
			default:
			{
				global = &(SDFTFontFamily_GenericMonospace);
				lock   = &(SDFTFontFamily_GenericMonospaceLock);
				name   = SDFTFontFamily_GenericMonospaceName;
			}
			break;
		}

		/* get the generic font family, synchronously */
		SDMutex_Lock(lock);
		{
			/* create the generic, as needed */
			if(*global == 0)
			{
				/* declarations */
				SDStatus   status;
				FcPattern *pattern;

				/* allocate the font family */
				if(!(*global = (SDFontFamily *)SDMalloc(sizeof(SDFTFontFamily))))
				{
					/* unlock global space */
					SDMutex_Unlock(lock);

					/* finish clean	up */
					*_this  = 0;

					/* return out of memory status */
					return SDStatus_OutOfMemory;
				}

				/* build the pattern */
				if((status _BuildPattern(&pattern, 0, name)) != SDStatus_OK)
				{
					/* clean up global space */
					SDFree(*global);
					*global = 0;

					/* unlock global space */
					SDMutex_Unlock(lock);

					/* finish clean up */
					*_this  = 0;

					/* return status */
					return status;
				}

				/* initialize the members */
				(*global)->lock         = lock;
				(*global)->metricsStyle = (SDFontStyle)-1;
				(*global)->pattern      = pattern;
			}

			/* set the font family to the global generic family */
			*_this = *global;
		}
		SDMutex_Unlock(lock);
	}

	/* return successfully */
	return SDStatus_OK;
}

SDStatus
SDFTFontFamily_GetMetrics(SDFontFamily   *_this,
                          SDFontStyle     style,
                          SDFontMetricsI *metrics)
{
	/* declarations */
	SDFTFontFamily *family;
	SDFTFont       *font;
	SDStatus        status;

	/* ensure we have a this pointer */
	SDStatus_Require((_this != 0), SDStatus_ArgumentNull);

	/* ensure we have a metrics pointer */
	SDStatus_Require((metrics != 0), SDStatus_ArgumentNull);

	/* get this as a freetype font family */
	family = (SDFontFamily *)_this;

	/* get the family metrics, synchronously */
	SDMutex_Lock(family->lock);
	{
		/* get the cached metrics, if available */
		if(family->metricsStyle == style)
		{
			/* get the cached metrics */
			*metrics = family->metrics;

			/* unlock the font family*/
			SDMutex_Unlock(family->lock);

			/* return successfully */
			return SDStatus_OK;
		}
	}
	SDMutex_Unlock(family->lock);

	/*\
	|*| NOTE: we get the metrics outside the lock to avoid blocking during
	|*|       the long font creation process, and to avoid recursive locking
	|*|       once we put font and/or face caching in place
	\*/

	/* create the font */
	status =
		SDFTFont_Create
			(&((SDFont *)font), family, 0.0f, style, SDGraphicsUnit_Pixel);

	/* handle font creation failures */
	if(status != SDStatus_OK) { return status; }

	/* get the font metrics */
	status = SDFTFont_GetMetricsI(((SDFont *)font), metrics);

	/* destroy the font */
	SDFTFont_Destroy(&((SDFont *)font));

	/* handle metrics failures */
	if(status != SDStatus_OK) { return status; }

	/* cache the family metrics, synchronously */
	SDMutex_Lock(family->lock);
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
	SDMutex_Unlock(family->lock);

	/* return successfully */
	return SDStatus_OK;
}

SDStatus
SDFTFontFamily_IsStyleAvailable(SDFontFamily *_this,
                                SDFontStyle   style,
                                SDBool       *available)
{
	/* ensure we have a this pointer */
	SDStatus_Require((_this != 0), SDStatus_ArgumentNull);

	/* ensure we have a style available flag pointer */
	SDStatus_Require((available != 0), SDStatus_ArgumentNull);

	/* assume all styles are available */
	*available = 1;

	/* return successfully */
	return SDStatus_OK;
}
SDStatus
SDFTFont_GetMetricsI(SDFont         *_this,
                     SDFontMetricsI *metrics)
{
	/* ensure we have a this pointer */
	SDStatus_Require((_this != 0), SDStatus_ArgumentNull);

	/* ensure we have a metrics pointer */
	SDStatus_Require((metrics != 0), SDStatus_ArgumentNull);

	/* get the metrics */
	{
		/* declarations */
		FT_Face face;

		/* get the face */
		face = _this->face;

		/* assertions */
		SDASSERT((FT_IS_SCALABLE(face)));

		/* get the metrics information */
		metrics->emHeight    = face->units_per_EM;
		metrics->cellAscent  = face->ascender;
		metrics->cellDescent = -(face->descender);
		metrics->lineSpacing = face->height;
	}

	/* return successfully */
	return SDStatus_OK;
}

SDStatus
SDFTFont_GetMetricsF(SDFont         *_this,
                     SDFontMetricsF *metrics)
{
	/* ensure we have a this pointer */
	SDStatus_Require((_this != 0), SDStatus_ArgumentNull);

	/* ensure we have a metrics pointer */
	SDStatus_Require((metrics != 0), SDStatus_ArgumentNull);

	/* get the metrics */
	{
		/* declarations */
		FT_Face face;
		SDFloat pixels;

		/* get the face */
		face = _this->face;

		/* assertions */
		SDASSERT((FT_IS_SCALABLE(face)));

		/* get the pixel size */
		pixels = _this->pixelSize;

		/* get the metrics information */
		metrics->emHeight    = face->units_per_EM;
		metrics->cellAscent  = pixels * (face->ascender / metrics->emHeight);
		metrics->cellDescent = pixels * -(face->descender / metrics->emHeight);
		metrics->lineSpacing = pixels * (face->height / metrics->emHeight);
	}

	/* return successfully */
	return SDStatus_OK;
}

SDStatus
SDFTFont_Create(SDFont         **_this,
                SDFontFamily    *family,
                SDFontStyle      style,
                SDFloat          size,
                SDGraphicsUnit   unit)
{
	/* ensure we have a this pointer pointer */
	SDStatus_Require((_this != 0), SDStatus_ArgumentNull);

	/* ensure we have a font family pointer */
	SDStatus_Require((family != 0), SDStatus_ArgumentNull);

	/* allocate the font */
	if(!(*_this = (SDFont *)SDMalloc(sizeof(SDFTFont))))
	{
		return SDStatus_OutOfMemory;
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
		SDFloat          pointSize;
		SDFTFont        *font;
		SDFTFontFamily  *ff;

		/* get the family as a freetype family */
		ff = (SDFTFontFamily *)family;

		/* get the family name */
		result = FcPatternGetString(ff->pattern, FC_FAMILY, 0, &name);

		/* assertions */
		SDASSERT((result == FcResultMatch));

		/* get this as a freetype font */
		font = ((SDFTFont *)(*_this));

		/* calculate size in pixels */
		font->pixelSize =
			SDUtils_ConvertUnits
				(unit, SDGraphicsUnit_Pixel, size);

		/* initialize the remaining simple members */
		font->size  = size;
		font->unit  = unit;
		font->style = style;

		/* get the font weight */
		if((style & SDFontStyle_Bold) == SDFontStyle_Bold)
		{
			weight = FC_WEIGHT_BOLD;
		}
		else
		{
			weight = FC_WEIGHT_LIGHT;
		}

		/* get the font slant */
		if((style & SDFontStyle_Italic) == SDFontStyle_Italic)
		{
			slant = FC_SLANT_ITALIC;
		}
		else
		{
			slant = FC_SLANT_ROMAN;
		}

		/* ensure library is initialized, synchronously */
		SDMutex_Lock(&(SDFTFont_FreeTypeLibraryLock));
		{
			/* initialize the library, as needed */
			if(!SDFTFont_FreeTypeLibraryInit)
			{
				/* initialize the freetype library */
				if((error = FT_Init_FreeType(SDFTFont_FreeTypeLibrary)))
				{
					/* assume out of memory for now */
					SDMutex_Unlock(&(SDFTFont_FreeTypeLibraryLock));
					SDFree(*_this);
					*_this = 0;
					return SDStatus_OutOfMemory;
				}

				/* set the freetype library initialized flag to true */
				SDFTFont_FreeTypeLibraryInit = 1;
			}
		}
		SDMutex_Unlock(&(SDFTFont_FreeTypeLibraryLock));

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
			SDFree(*_this);
			*_this = 0;
			return SDStatus_OutOfMemory;
		}

		/* set up pattern for font matching */
		FcConfigSubstitute(0, pattern, FcMatchPattern);
		FcDefaultSubstitute(pattern);

		/* get the matching pattern */
		if(!(match = FcFontMatch(0, pattern, &result)))
		{
			/* clean up */
			FcPatternDestroy(pattern);
			SDFree(*_this);
			*_this = 0;

			/* return status based on result */
			if(result == FcResultNoMatch)
			{
				return SDStatus_Argument_FontFamilyNotFound;
			}
			else
			{
				return SDStatus_OutOfMemory;
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
			SDFree(*_this);
			*_this = 0;

			/* return status based on result */
			if(result == FcResultNoMatch)
			{
				return SDStatus_Argument_FontFamilyNotFound;
			}
			else
			{
				return SDStatus_OutOfMemory;
			}
		}

		/* try to create the face from the filename */
		if((error = FT_New_Face(ft_library, filename, 0, &face)))
		{
			/* assume out of memory for now */
			FcPatternDestroy(match);
			SDFree(*_this);
			*_this = 0;
			return SDStatus_OutOfMemory;
		}

		/* destroy the match pattern */
		FcPatternDestroy(match);

		/* calculate size in points */
		pointSize =
			SDUtils_ConvertUnits
				(unit, SDGraphicsUnit_Pixel, size);

		/*\
		|*| NOTE: we don't use FT_Set_Pixel_Sizes to avoid rounding away
		|*|       subpixels before glyph transformation
		\*/

		/* set the point size */
		FT_Set_Char_Size
			(face,
			 DOUBLE_TO_26_6(pointSize), DOUBLE_TO_26_6(pointSize),
			 SDGraphics_DefaultDpiY,    SDGraphics_DefaultDpiY);

		/* set the face */
		font->face = face;
	}

	/* return successfully */
	return SDStatus_OK;
}

SDStatus
SDFTFont_Destroy(SDFont **_this)
{
	/* ensure we have a this pointer pointer */
	SDStatus_Require((_this != 0), SDStatus_ArgumentNull);

	/* ensure we have a this pointer */
	SDStatus_Require((*_this != 0), SDStatus_ArgumentNull);

	/* finalize the font */
	{
		/* finalize the face */
		FT_Done_Face(face);
	}

	/* return successfully */
	return SDStatus_OK;
}

SDStatus
SDFTFontCollection_AddFontMemory(SDFontCollection *_this,
                                 const SDByte     *memory,
                                 SDUInt32          length)
{
	/* ensure we have a this pointer */
	SDStatus_Require((_this != 0), SDStatus_ArgumentNull);

	/* ensure we have a memory pointer */
	SDStatus_Require((memory != 0), SDStatus_ArgumentNull);

	/* ensure we have memory */
	SDStatus_Require((length != 0), SDStatus_Argument);

	/* add the memory font */
	{
		/* declarations */
		int fd, len;

		/* create the temporary file name */
		SDByte fname[] = "/tmp/XXXXXXXXXXXXXXXX";

		/* create the temporary file */
		if((fd = mkstemp((char *)fname)) == -1) { return SDStatus_Error; }

		/* get the length */
		len = (int)length;

		/* output the data to the file */
		while((len = write(fd, memory, len)) > 0) { /* write */ }

		/* close the file */
		close(fd);

		/* handle write errors */
		if(len == -1) { return SDStatus_Error; }

		/* add the font file to the collection */
		FcConfigAppFontAddFile(((SDFTFontCollection *)_this)->config, fname);
	}

	/* return successfully */
	return SDStatus_OK;
}

SDStatus
SDFTFont_GetGlyphMetrics(SDFont    *_this,
                         SDChar16  *s,
                         SDSizeF  **metrics,
                         SDUInt32  *count)
{
	/* TODO */
	return SDStatus_NotImplemented;
}


#ifdef __cplusplus
};
#endif

#endif
