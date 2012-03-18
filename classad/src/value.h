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

#ifndef __VALUES_H__
#define __VALUES_H__

#include "common.h"
#include "util.h"

BEGIN_NAMESPACE( classad )

class Literal;
class ExprList;
class ClassAd;

/// Represents the result of an evaluation.
class Value 
{
	public:
			/// Value types
		enum ValueType {
												NULL_VALUE          = 0,
		/** The error value */ 					ERROR_VALUE         = 1<<0,
		/** The undefined value */ 				UNDEFINED_VALUE     = 1<<1,
		/** A boolean value (false, true)*/ 	BOOLEAN_VALUE 		= 1<<2,
		/** An integer value */ 				INTEGER_VALUE       = 1<<3,
		/** A real value */ 					REAL_VALUE          = 1<<4,
		/** A relative time value */ 			RELATIVE_TIME_VALUE = 1<<5,
		/** An absolute time value */ 			ABSOLUTE_TIME_VALUE = 1<<6,
		/** A string value */ 					STRING_VALUE        = 1<<7,
		/** A classad value */ 					CLASSAD_VALUE       = 1<<8,
		/** An expression list value */     	LIST_VALUE 			= 1<<9
		};

			/// Number factors
		enum NumberFactor {
	    /** No factor specified */  NO_FACTOR= 0,
		/** Byte factor */          B_FACTOR = 1,
		/** Kilo factor */          K_FACTOR = 2,
		/** Mega factor */          M_FACTOR = 3,
		/** Giga factor */          G_FACTOR = 4,
		/** Terra factor*/          T_FACTOR = 5
		};

 
		/// Values of number multiplication factors
		static const double ScaleFactor[];


		/// Constructor
		Value();

		/// Destructor
		~Value();

		/** Discards the previous value and sets the value to UNDEFINED */
		void Clear (void);

		/** Copies the value of another value object.
			@param v The value copied from.
		*/
		void CopyFrom( const Value &v );

		/** Sets a boolean value; previous value discarded.
			@param b The boolean value.
		*/
		void SetBooleanValue(bool b);

		/** Sets a real value; previous value discarded.
			@param r The real value.
		*/
		void SetRealValue(double r);

		/** Sets an integer value; previous value discarded.
			@param i The integer value.
		*/
		void SetIntegerValue(int i);

		/** Sets the undefined value; previous value discarded.
		*/
		void SetUndefinedValue(void);

		/** Sets the error value; previous value discarded.
		*/
		void SetErrorValue(void);

		/** Sets an expression list value; previous value discarded.
			@param l The list value.
		*/
		void SetListValue(const ExprList* l);

		/** Sets a ClassAd value; previous value discarded.
			@param c The ClassAd value.
		*/
		void SetClassAdValue(ClassAd* c);	

		/** Sets a string value; previous value discarded.
			@param str The string value.
		*/
		void SetStringValue( const std::string &str );

		/** Sets a string value; previous value discarded.
			@param str The string value.
		*/
		void SetStringValue( const char *str );

		/** Sets an absolute time value in seconds since the UNIX epoch, & the 
            time zone it's measured in.
			@param secs - Absolute Time Literal(seconds since the UNIX epoch, timezone offset) .
		*/
		void SetAbsoluteTimeValue( abstime_t secs );

		/** Sets a relative time value.
			@param secs Number of seconds.
		*/
		void SetRelativeTimeValue( time_t secs );

		/** Gets the type of the value.
			@return The value type.
			@see ValueType
		*/
		inline ValueType GetType() const { return valueType; }

		/** Checks if the value is boolean.
			@param b The boolean value if the value is boolean.
			@return true iff the value is boolean.
		*/
		inline bool IsBooleanValue(bool& b) const;
		/** Checks if the value is boolean.
			@return true iff the value is boolean.
		*/	
		inline bool IsBooleanValue() const;
		/** Checks if the value is integral.
			@param i The integer value if the value is integer.
			@return true iff the value is an integer.
		*/
		inline bool IsIntegerValue(int &i) const; 	
		/** Checks if the value is integral.
			@return true iff the value is an integer.
		*/
		inline bool IsIntegerValue() const;
		/** Checks if the value is real.
			@param r The real value if the value is real.
			@return true iff the value is real.
		*/	
		inline bool IsRealValue(double &r) const; 	
		/** Checks if the value is real.
			@return true iff the value is real.
		*/
		inline bool IsRealValue() const;
		/** Checks if the value is a string.  
			@param str A reference to a string object, which is filled with the
				string value.
			@return true iff the value is a string.
		*/
		bool IsStringValue( std::string &str ) const; 	
		/** Checks if the value is a string.  
			@param str A reference to a C string, which will point to the 
				string value.  This pointer must <b>not</b> be deallocated or
				tampered with.
			@return true iff the value is a string.
		*/
		bool IsStringValue( const char *&str ) const; 	
		/** Checks if the value is a string.  
			@param str A buffer to hold the string value.
			@param len The size of the buffer.
			@return true iff the value is a string.
		*/
		bool IsStringValue( char *str, int len ) const; 	
		/** Checks if the value is a string.
			@return true iff the value is string.
		*/
		inline bool IsStringValue() const;
		/** Checks if the value is an expression list.
			@param l The expression list if the value is an expression list.
			@return true iff the value is an expression list.
		*/
		inline bool IsListValue(const ExprList*& l) const;
		/** Checks if the value is an expression list.
			@return true iff the value is an expression list.
		*/
		inline bool IsListValue() const;
		/** Checks if the value is a ClassAd.
			@param c The ClassAd if the value is a ClassAd.
			@return true iff the value is a ClassAd.
		*/
		inline bool IsClassAdValue(const ClassAd *&c) const; 
		/** Checks if the value is a ClassAd.
			@param c The ClassAd if the value is a ClassAd.
			@return true iff the value is a ClassAd.
		*/
		inline bool IsClassAdValue(ClassAd *&c); 
		/** Checks if the value is a ClassAd.
			@return true iff the value is a ClassAd value.
		*/
		inline bool IsClassAdValue() const;
		/** Checks if the value is the undefined value.
			@return true iff the value if the undefined value.
		*/
		inline bool IsUndefinedValue() const;
		/** Checks if the value is the error value.
			@return true iff the value if the error value.
		*/
		inline bool IsErrorValue() const;
		/** Checks if the value is exceptional.
			@return true iff the value is either undefined or error.
		*/
		inline bool IsExceptional() const;
		/** Checks if the value is numerical. 
			@return true iff the value is a number
		*/
		bool IsNumber () const;
		/** Checks if the value is numerical. If the value is a real, it is 
				converted to an integer through truncation.
			@param i The integer value of the value if the value is a number.
			@return true iff the value is a number
		*/
		bool IsNumber (int &i) const;
		/** Checks if the value is numerical. If the value is an integer, it 
				is promoted to a real.
			@param r The real value of the value if the value is a number.
			@return true iff the value is a number
		*/
		bool IsNumber (double &r) const;
		/** Checks if the value is an absolute time value.
			@return true iff the value is an absolute time value.
		*/
		bool IsAbsoluteTimeValue( ) const;
		/** Checks if the value is an absolute time value.
			@param secs - Absolute time literal (Number of seconds since the UNIX epoch,timezone offset).
			@return true iff the value is an absolute time value.
		*/
		bool IsAbsoluteTimeValue( abstime_t& secs ) const;
		/** Checks if the value is a relative time value.
			@return true iff the value is a relative time value
		*/
		bool IsRelativeTimeValue( ) const;
		/** Checks if the value is a relative time value.
			@param secs Number of seconds
			@return true iff the value is a relative time value
		*/
		bool IsRelativeTimeValue( time_t& secs ) const;

		friend std::ostream& operator<<(std::ostream &stream, Value &value);

	private:
		friend class Literal;
		friend class ClassAd;
		friend class ExprTree;

		ValueType 		valueType;		// the type of the value


		union {
			bool			booleanValue;
			int				integerValue;
			double 			realValue;
			const ExprList	*listValue;
			ClassAd			*classadValue;
			time_t			relTimeValueSecs;
			abstime_t absTimeValueSecs;
		  
		};
		std::string			strValue;		// has ctor/dtor cannot be in the union
};


// implementations of the inlined functions
inline bool Value::
IsBooleanValue( bool& b ) const
{
	b = booleanValue;
	return( valueType == BOOLEAN_VALUE );
}

inline bool Value::
IsBooleanValue() const
{
	return( valueType == BOOLEAN_VALUE );
}

inline bool Value::
IsIntegerValue (int &i) const
{
    i = integerValue;
    return (valueType == INTEGER_VALUE);
}  

inline bool Value::
IsIntegerValue () const
{
    return (valueType == INTEGER_VALUE);
}  

inline bool Value::
IsRealValue (double &r) const
{
    r = realValue;
    return (valueType == REAL_VALUE);
}  

inline bool Value::
IsRealValue () const
{
    return (valueType == REAL_VALUE);
}  

inline bool Value::
IsListValue( const ExprList *&l) const
{
	l = listValue;
	return(valueType == LIST_VALUE);
}

inline bool Value::
IsListValue () const
{
	return (valueType == LIST_VALUE);
}


inline bool Value::
IsStringValue() const
{
    return (valueType == STRING_VALUE);
}


inline bool Value::
IsStringValue( const char *&s ) const
{
	// We want to be careful not to copy it in here. 
	// People may accumulate in the "s" after this call,
	// So it best to only touch it if it exists.
	// (Example: the strcat classad function)
	if (valueType == STRING_VALUE) {
		s = strValue.c_str( );
		return true;
	} else {
		return false;
	}
}

inline bool Value::
IsStringValue( char *s, int len ) const
{
	if( valueType == STRING_VALUE ) {
		strncpy( s, strValue.c_str( ), len );
		return( true );
	}
	return( false );
}

inline bool Value::
IsStringValue( std::string &s ) const
{
	if ( valueType == STRING_VALUE ) {
		s = strValue;
		return true;
	} else {
		return false;
	}
}

inline bool Value::
IsClassAdValue(const ClassAd *&ad) const
{
	if ( valueType == CLASSAD_VALUE ) {
		ad = classadValue;
		return true;
	} else {
		return false;
	}
}

inline bool Value::
IsClassAdValue(ClassAd *&ad)
{
	if ( valueType == CLASSAD_VALUE ) {
		ad = classadValue;
		return true;
	} else {
		return false;
	}
}

inline bool Value:: 
IsClassAdValue() const
{
	return( valueType == CLASSAD_VALUE );	
}

inline bool Value::
IsUndefinedValue (void) const
{ 
	return (valueType == UNDEFINED_VALUE);
}

inline bool Value::
IsErrorValue(void) const
{ 
	return (valueType == ERROR_VALUE); 
}

inline bool Value::
IsExceptional(void) const
{
	return( valueType == UNDEFINED_VALUE || valueType == ERROR_VALUE );
}

inline bool Value::
IsAbsoluteTimeValue( ) const
{
	return( valueType == ABSOLUTE_TIME_VALUE );
}

inline bool Value::
IsAbsoluteTimeValue( abstime_t &secs ) const
{
	secs = absTimeValueSecs;
	return( valueType == ABSOLUTE_TIME_VALUE );
}

inline bool Value::
IsRelativeTimeValue( ) const
{
	return( valueType == RELATIVE_TIME_VALUE );
}

inline bool Value::
IsRelativeTimeValue( time_t &secs ) const
{
	secs = relTimeValueSecs;
	return( valueType == RELATIVE_TIME_VALUE );
}
inline bool Value::
IsNumber( ) const
{
	return( valueType==INTEGER_VALUE || valueType==REAL_VALUE );
}
END_NAMESPACE // classad

#endif//__VALUES_H__
