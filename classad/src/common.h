/*********************************************************************
 *
 * Condor ClassAd library
 * Copyright (C) 1990-2003, Condor Team, Computer Sciences Department,
 * University of Wisconsin-Madison, WI and Rajesh Raman.
 *
 * This source code is covered by the Condor Public License, which can
 * be found in the accompanying LICENSE file, or online at
 * www.condorproject.org.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * AND THE UNIVERSITY OF WISCONSIN-MADISON "AS IS" AND ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY, OF SATISFACTORY QUALITY, AND FITNESS
 * FOR A PARTICULAR PURPOSE OR USE ARE DISCLAIMED. THE COPYRIGHT
 * HOLDERS AND CONTRIBUTORS AND THE UNIVERSITY OF WISCONSIN-MADISON
 * MAKE NO MAKE NO REPRESENTATION THAT THE SOFTWARE, MODIFICATIONS,
 * ENHANCEMENTS OR DERIVATIVE WORKS THEREOF, WILL NOT INFRINGE ANY
 * PATENT, COPYRIGHT, TRADEMARK, TRADE SECRET OR OTHER PROPRIETARY
 * RIGHT.
 *
 *********************************************************************/


#ifndef __COMMON_H__
#define __COMMON_H__

#if defined( WANT_NAMESPACES ) && defined(__cplusplus)
#define BEGIN_NAMESPACE( x ) namespace x {
#define END_NAMESPACE }
#else
#define BEGIN_NAMESPACE( x )
#define END_NAMESPACE
#endif

#ifdef CLASSAD_DISTRIBUTION
#ifndef _GNU_SOURCE
#define _GNU_SOURCE /* to get definition for strptime on Linux */
#endif

#ifndef __EXTENSIONS__
#define __EXTENSIONS__ /* to get gmtime_r and localtime_r on Solaris */
#endif

#ifndef _POSIX_C_SOURCE
#define _POSIX_C_SOURCE 199506L /* To get asctime_r */
#endif

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>
#include "debug.h"
#ifndef WIN32
	#include <unistd.h>
	#define DLL_IMPORT_MAGIC  /* a no-op on Unix */
#endif
#include <errno.h>
#include <ctype.h>

#ifndef WORD_BIT
#define WORD_BIT 32
#endif



#ifdef WIN32
	// special definitions we need for Windows
#define DLL_IMPORT_MAGIC __declspec(dllimport)
#include <windows.h>
#include <float.h>
#include <io.h>
#define fsync _commit
#define open _open
#define strcasecmp _stricmp
#define rint(num) floor(num + .5)
#define isnan _isnan
	// isinf() defined in util.h



#define snprintf _snprintf

	// Disable warnings about multiple template instantiations
	// (done for gcc)
#pragma warning( disable : 4660 )  
	// Disable warnings about forcing bools
#pragma warning( disable : 4800 )  
	// Disable warnings about truncated debug identifiers
#pragma warning( disable : 4786 )
	// use new SDK stuff in STLport
#define _STLP_NEW_PLATFORM_SDK
#endif // WIN32


#else /* CLASSAD_DISTRIBUTION isn't defined */

#ifndef _POSIX_C_SOURCE
#define _POSIX_C_SOURCE 199506L /* To get asctime_r */
#endif

#ifdef __cplusplus
using namespace std;
#endif

	
#include "condor_common.h"
#include "condor_debug.h"
#include "condor_attributes.h"
#include "classad_features.h"
#endif /* CLASSAD_DISTRIBUTION */

#ifdef __cplusplus
#include <string>
#endif

BEGIN_NAMESPACE( classad )

static const char ATTR_AD					[]	= "Ad";
static const char ATTR_CONTEXT				[] 	= "Context";
static const char ATTR_DEEP_MODS			[] 	= "DeepMods";
static const char ATTR_DELETE_AD			[] 	= "DeleteAd";
static const char ATTR_DELETES				[] 	= "Deletes";
static const char ATTR_KEY					[]	= "Key";
static const char ATTR_NEW_AD				[]	= "NewAd";
static const char ATTR_OP_TYPE				[]	= "OpType";
static const char ATTR_PARENT_VIEW_NAME		[]	= "ParentViewName";
static const char ATTR_PARTITION_EXPRS 		[]  = "PartitionExprs";
static const char ATTR_PARTITIONED_VIEWS	[] 	= "PartitionedViews";
static const char ATTR_PROJECT_THROUGH		[]	= "ProjectThrough";
static const char ATTR_RANK_HINTS			[] 	= "RankHints";
static const char ATTR_REPLACE				[] 	= "Replace";
static const char ATTR_SUBORDINATE_VIEWS	[]	= "SubordinateViews";
static const char ATTR_UPDATES				[] 	= "Updates";
static const char ATTR_WANT_LIST			[]	= "WantList";
static const char ATTR_WANT_PRELUDE			[]	= "WantPrelude";
static const char ATTR_WANT_RESULTS			[]	= "WantResults";
static const char ATTR_WANT_POSTLUDE		[]	= "WantPostlude";
static const char ATTR_VIEW_INFO			[]	= "ViewInfo";
static const char ATTR_VIEW_NAME			[]	= "ViewName";
static const char ATTR_XACTION_NAME			[]	= "XactionName";

#if defined( CLASSAD_DISTRIBUTION )
static const char ATTR_REQUIREMENTS			[]	= "Requirements";
static const char ATTR_RANK					[]	= "Rank";
#endif

#if defined(__cplusplus)
struct CaseIgnLTStr {
    bool operator( )( const std::string &s1, const std::string &s2 ) const {
       return( strcasecmp( s1.c_str( ), s2.c_str( ) ) < 0 );
	}
};

struct CaseIgnEqStr {
	bool operator( )( const std::string &s1, const std::string &s2 ) const {
		return( strcasecmp( s1.c_str( ), s2.c_str( ) ) == 0 );
	}
};

class ExprTree;
struct ExprHash {
	size_t operator()( const ExprTree *const &x ) const {
		return( (size_t)x );
	}
};

struct StringHash {
	size_t operator()( const std::string &s ) const {
		size_t h = 0;
		for( int i = s.size()-1; i >= 0; i-- ) {
			h = 5*h + s[i];
		}
		return( h );
	}
};

struct StringCaseIgnHash {
	size_t operator()( const std::string &s ) const {
		size_t h = 0;
		for( int i = s.size()-1; i >= 0; i-- ) {
			h = 5*h + tolower(s[i]);
		}
		return( h );
	}
};
extern std::string       CondorErrMsg;
#endif

extern int 		CondorErrno;
static const std::string NULL_XACTION = "";

END_NAMESPACE // classad

char* strnewp( const char* );

#if defined(CLASSAD_DISTRIBUTION)
#include "classadErrno.h"
#else
#include "condor_errno.h"
#endif

#endif//__COMMON_H__
