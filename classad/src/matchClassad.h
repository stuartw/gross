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

#ifndef __MATCH_CLASSAD_H__
#define __MATCH_CLASSAD_H__

#include "classad.h"

BEGIN_NAMESPACE( classad )

/** Special case of a ClassAd which sets up the scope names for bilateral 
  	matching.  The top-level scope is defined as follows:
	<pre>
|   [
|      symmetricMatch   = leftMatchesRight && rightMatchesLeft;
|      leftMatchesRight = adcr.ad.requirements;
|      rightMatchesLeft = adcl.ad.requirements;
|      leftRankValue    = adcl.ad.rank;
|      rightRankValue   = adcr.ad.rank;
|      adcl             =
|          [
|              other    = adcr.ad;
|              my       = ad;       // for condor backwards compatibility
|              target   = other;    // for condor backwards compatibility
|              ad       = 
|                 [
|                     // the ``left'' match candidate goes here
|                 ]
|   	   ];
|      adcr             =
|          [
|              other    = adcl.ad;
|              my       = ad;       // for condor backwards compatibility
|              target   = other;    // for condor backwards compatibility
|              ad       = 
|                 [
|                     // the ``right'' match candidate goes here
|                 ]
|   	   ];
|   ]
	</pre>
*/
class MatchClassAd : public ClassAd
{
	public:
		/// Default constructor
		MatchClassAd();
		/** Constructor which builds the CondorClassad given two ads
		 	@param al The left candidate ad
			@param ar The right candidate ad
		*/
		MatchClassAd( ClassAd* al, ClassAd* ar );
		/// Default destructor
		~MatchClassAd();

		/** Factory method to make a MatchClassad given two ClassAds to be
			matched.
			@param al The ad to be placed in the left context.
			@param ar The ad to be placed in the right context.
			@return A CondorClassad, or NULL if the operation failed.
		*/
		static MatchClassAd *MakeMatchClassAd( ClassAd* al, ClassAd* ar );

		/** Method to initialize a MatchClassad given two ClassAds.  The old
		 	expressions in the classad are deleted.
			@param al The ad to be placed in the left context.
			@param ar The ad to be placed in the right context.
			@return true if the operation succeeded, false otherwise
		*/
		bool InitMatchClassAd( ClassAd* al, ClassAd *ar );

		/** Replaces ad in the left context, or insert one if an ad did not
			previously exist
			@param al The ad to be placed in the left context.
			@return true if the operation succeeded and false otherwise.
		*/
		bool ReplaceLeftAd( ClassAd *al );

		/** Replaces ad in the right context, or insert one if an ad did not
			previously exist
			@param ar The ad to be placed in the right context.
			@return true if the operation succeeded and false otherwise.
		*/
		bool ReplaceRightAd( ClassAd *ar );

		/** Gets the ad in the left context.
			@return The ClassAd, or NULL if the ad doesn't exist.
		*/
		ClassAd *GetLeftAd();

		/** Gets the ad in the right context.
			@return The ClassAd, or NULL if the ad doesn't exist.
		*/
		ClassAd *GetRightAd();

		/** Gets the left context ad. (<tt>.adcl</tt> in the above example)
		 	@return The left context ad, or NULL if the MatchClassAd is not
				valid
		*/
		ClassAd *GetLeftContext( );

		/** Gets the right context ad. (<tt>.adcr</tt> in the above example)
		 	@return The left context ad, or NULL if the MatchClassAd is not
				valid
		*/
		ClassAd *GetRightContext( );

		/** Removes the left candidate from the match classad.  If the 
		    candidate ``lives'' in another data structure, this method
			should be called so that the match classad doesn't delete the
			candidate.
			@return The left candidate ad.
		*/
		ClassAd *RemoveLeftAd( );

		/** Removes the right candidate from the match classad.  If the 
		    candidate ``lives'' in another data structure, this method
			should be called so that the match classad doesn't delete the
			candidate.
			@return The right candidate ad.
		*/
		ClassAd *RemoveRightAd( );

	protected:
		const ClassAd *ladParent, *radParent;
		ClassAd *lCtx, *rCtx, *lad, *rad;
};

END_NAMESPACE // classad

#endif
