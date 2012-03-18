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

#ifndef __EXPR_LIST_H__
#define __EXPR_LIST_H__

#include <vector>

BEGIN_NAMESPACE( classad )

class ExprListIterator;

/** 
	Expression node which represents a list of expressions 
*/
class ExprList : public ExprTree
{
	public:
		ExprList();
		ExprList(std::vector<ExprTree*>& list);

		/// Destructor
		~ExprList();

		/** Factory to make an expression list expression
		 * 	@param	list A vector of the expressions to be contained in the
		 * 		list
		 * 	@return The constructed expression list
		 */
		static ExprList *MakeExprList( const std::vector< ExprTree* > &list );

		/** Deconstructor to obtain the components of an expression list
		 * 	@param list The list of expressions
		 */
		void GetComponents( std::vector<ExprTree*>& list) const;

		/// Make a deep copy of the expression
        // We should return an AttributeReference, but Visual
        // Studio 6 won't accept that part of the standard. 
		virtual ExprTree* Copy( ) const;
		
		// STL-like iterators and functions
	    typedef std::vector<ExprTree*>::iterator       iterator;
    	typedef std::vector<ExprTree*>::const_iterator const_iterator;

        iterator begin()             { return exprList.begin(); }
		iterator end()               { return exprList.end();   }
		const_iterator begin() const { return exprList.begin(); }
		const_iterator end() const   { return exprList.end();   }

		void insert(iterator it, ExprTree* t);
		void push_back(ExprTree* t);
		void erase(iterator it);
		void erase(iterator f, iterator l);
	
	private:
		friend class ExprListIterator;

		std::vector<ExprTree*> exprList;

		void Clear (void);
		virtual void _SetParentScope( const ClassAd* p );
		virtual bool _Evaluate (EvalState &, Value &) const;
		virtual bool _Evaluate (EvalState &, Value &, ExprTree *&) const;
		virtual bool _Flatten( EvalState&, Value&, ExprTree*&, int* ) const;
};


/** 
	Expression List iterator
*/
class ExprListIterator
{
	public:
		/// Constructor
		ExprListIterator( );

		/** Constructor which initializes the iterator.
			@param l The list to be iterated over (i.e., the iteratee).
			@see initialize
		*/
		ExprListIterator( const ExprList* l );

		/// Destructor
		~ExprListIterator( );

        /** Initializes the object to iterate over an expression list; the 
				iterator begins at the "before first" position.  This method 
				must be called before the iterator is usable.  (The iteration 
				methods return false if the iterator has not been initialized.)
				This method may be called any number of times, with different 
                expression lists as arguments.
			@param l The expression list to iterate over (i.e., the iteratee).
		*/
	    void Initialize( const ExprList* l );

		/// Positions the iterator to the first element.
    	void ToFirst( );

		/// Positions the iterator to the "after last" position
    	void ToAfterLast( );

		/** Positions the iterator at the n'th expression of the list (assuming 
				0-based index.  
			@param n The index of the expression to retrieve.
			@return true if the iterator was successfully positioned at the
				n'th element, and false otherwise.
		*/
		bool ToNth( int n );

		/** Gets the next expression in the list.
			@return The next expression in the list, or NULL if the iterator
				has crossed the last expression in the list.
		*/
    	const ExprTree* NextExpr( );

		/** Gets the expression currently pointed to.
			@return The expression currently pointed to.
		*/
    	const ExprTree* CurrentExpr( ) const;

		/** Gets the previous expression in the list.
			@return The previous expression in the list, or NULL if the iterator
				has crossed the first expression in the list.
		*/
    	const ExprTree* PrevExpr( );
    
		/** Gets the value of the next expression in the list.
			@param v The value of the expression.
			@param es The EvalState object which caches values of expressions.
				Ordinarily, this parameter will not be supplied by the user, and
				an internal EvalState object will be used. 
			@return false if the iterator has crossed the last expression in the
				list, or true otherwise.
		*/
    	bool NextValue( Value& v, EvalState *es=NULL );

		/** Gets the value of the expression currently pointed to.
			@param v The value of the expression.
			@param es The EvalState object which caches values of expressions.
				Ordinarily, this parameter will not be supplied by the user, and
				an internal EvalState object will be used. 
			@return true if the operation succeeded, false otherwise.
		*/
    	bool CurrentValue( Value& v, EvalState *es=NULL );

		/** Gets the value of the previous expression in the list.
			@param v The value of the expression.
			@param es The EvalState object which caches values of expressions.
				Ordinarily, this parameter will not be supplied by the user, and
				an internal EvalState object will be used. 
			@return false if the iterator has crossed the first expression in 
				the list, or true otherwise.
		*/
    	bool PrevValue( Value& v, EvalState *es=NULL  );

		/** Gets the value of the next expression in the list, and identifies 
				sub-expressions that caused the value.
			@param v The value of the expression.
			@param t The expression composed of the significant sub-expressions.
			@param es The EvalState object which caches values of expressions.
				Ordinarily, this parameter will not be supplied by the user, and
				an internal EvalState object will be used. 
			@return false if the iterator has crossed the last expression in the
				list, or true otherwise.
		*/
    	bool NextValue( Value& v, ExprTree*& t, EvalState *es=NULL );

		/** Gets the value of the expression currently pointed to, and 
				identifies sub-expressions that caused the value.
			@param v The value of the expression.
			@param t The expression composed of the significant sub-expressions.
			@param es The EvalState object which caches values of expressions.
				Ordinarily, this parameter will not be supplied by the user, and
				an internal EvalState object will be used. 
			@return true if the operation succeeded, false otherwise.
		*/
    	bool CurrentValue( Value& v, ExprTree*& t, EvalState *es=NULL );

		/** Gets the value of the previous expression in the list, and 
				identifies sub-expressions that caused that value.
			@param v The value of the expression.
			@param t The expression composed of the significant sub-expressions.
			@param es The EvalState object which caches values of expressions.
				Ordinarily, this parameter will not be supplied by the user, and
				an internal EvalState object will be used. 
			@return false if the iterator has crossed the first expression in 
				the list, or true otherwise.
		*/
    	bool PrevValue( Value& v, ExprTree*& t, EvalState *es=NULL  );

		/** Predicate to check the position of the iterator.
			@return true iff the iterator is before the first element.
		*/
    	bool IsAtFirst( ) const;

		/** Predicate to check the position of the iterator.
			@return true iff the iterator is after the last element.
		*/
    	bool IsAfterLast( ) const;

	private:
		const ExprList                          *l;
		EvalState                               state;
		std::vector<ExprTree*>::const_iterator  itr;

    	bool GetValue( Value& v, const ExprTree *tree, EvalState *es=NULL );
		bool GetValue( Value& v, ExprTree*& t, const ExprTree *tree, EvalState *es=NULL );

};	


END_NAMESPACE // classad

#endif//__EXPR_LIST_H__
