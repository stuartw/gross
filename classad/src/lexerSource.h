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

#ifndef __LEXER_SOURCE_H__
#define __LEXER_SOURCE_H__

#include "common.h"
#include <iostream>

/*
 * LexerSource is a class that provides an abstract interface to the
 * lexer. The lexer reads tokens and gives them to the parser.  The
 * lexer reads single characters from a LexerSource. Because we want
 * to read characters from different types of sources (files, strings,
 * etc.) without burdening the lexer, the lexer uses this LexerSource
 * abstract class. There are several implementations of the
 * LexerSource that provide access to specific types of sources. 
 *
 */

class LexerSource
{
public:
	LexerSource()
	{
		return;
	}
	virtual ~LexerSource()
	{
		return;
	}
	
	// Reads a single character from the source
	virtual int ReadCharacter(void) = 0;

	// Returns the last character read (from ReadCharacter()) from the
	// source
	virtual int ReadPreviousCharacter(void) { return _previous_character; };

	// Puts back a character so that when ReadCharacter is called
	// again, it returns the character that was previously
	// read. Although this interface appears to require the ability to
	// put back an arbitrary number of characters, in practice we only
	// ever put back a single character. 
	virtual void UnreadCharacter(void) = 0;
	virtual bool AtEnd(void) const = 0;
protected:
	int _previous_character;
};

// This source allows input from a traditional C FILE *
class FileLexerSource : public LexerSource
{
public:
	FileLexerSource(FILE *file);
	virtual ~FileLexerSource();

	virtual void SetNewSource(FILE *file);
	
	virtual int ReadCharacter(void);
	virtual void UnreadCharacter(void);
	virtual bool AtEnd(void) const;

private:
	FILE *_file;
};

// This source allows input from a C++ stream. Note that
// the user passes in a pointer to the stream.
class InputStreamLexerSource : public LexerSource
{
public:
	InputStreamLexerSource(std::istream &stream);
	virtual ~InputStreamLexerSource();

	virtual void SetNewSource(std::istream &stream);
	
	virtual int ReadCharacter(void);
	virtual void UnreadCharacter(void);
	virtual bool AtEnd(void) const;

private:
	std::istream *_stream;
};

// This source allows input from a traditional C string.
class CharLexerSource : public LexerSource
{
public:
	CharLexerSource(const char *string, int offset=0);
	virtual ~CharLexerSource();
	
	virtual void SetNewSource(const char *string, int offset=0);
	virtual int ReadCharacter(void);
	virtual void UnreadCharacter(void);
	virtual bool AtEnd(void) const;

	virtual int GetCurrentLocation(void) const;
private:
	const char *_source_start;
	const char *_current;
};

// This source allows input from a C++ string.
class StringLexerSource : public LexerSource
{
public:
	StringLexerSource(const std::string *string, int offset=0);
	virtual ~StringLexerSource();

	virtual void SetNewSource(const std::string *string, int offset=0);
	
	virtual int ReadCharacter(void);
	virtual void UnreadCharacter(void);
	virtual bool AtEnd(void) const;

	virtual int GetCurrentLocation(void) const;
private:
	const std::string *_string;
	int           _offset;
};

#endif /* __LEXER_SOURCE_H__ */
