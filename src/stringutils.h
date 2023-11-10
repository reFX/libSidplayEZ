#pragma once

/*
 * This file is part of libsidplayfp, a SID player engine.
 *
 *  Copyright 2013-2023 Leandro Nini
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 */

#include <cctype>
#include <algorithm>
#include <string>

namespace stringutils
{
    /**
     * Compare two characters in a case insensitive way.
     */
	[[ nodiscard ]] inline bool casecompare ( char c1, char c2 ) { return std::tolower ( c1 ) == std::tolower ( c2 ); }

    /**
     * Compare two strings in a case insensitive way. 
     *
     * @return true if strings are equal.
     */
	[[ nodiscard ]] inline bool equal ( const std::string& s1, const std::string& s2 )
    {
        return std::equal ( s1.begin (), s1.end (), s2.begin (), s2.end (), casecompare );
    }

    /**
     * Compare two strings in a case insensitive way.
     *
     * @return true if strings are equal.
     */
	[[ nodiscard ]] inline bool equal ( const char* s1, const char* s2 )
    {
		if ( s1 == s2 )
			return true;

		if ( s1 == 0 || s2 == 0 )
			return false;

		while ( *s1 || *s2 )
		{
			if ( ! casecompare ( *s1, *s2 ) )
				return false;

            ++s1;
			++s2;
		}

		return true;
    }

    /**
     * Compare first n characters of two strings in a case insensitive way.
     *
     * @return true if strings are equal.
     */
	[[ nodiscard ]] inline bool equal ( const char* s1, const char* s2, size_t n )
	{
		if ( s1 == s2 || n == 0 )
			return true;

		if ( s1 == 0 || s2 == 0 )
			return false;

		while ( n-- && ( *s1 || *s2 ) )
		{
			if ( ! casecompare ( *s1, *s2 ) )
				return false;

			++s1;
			++s2;
		}

		return true;
	}

	[[ nodiscard ]] inline std::string toLower ( const std::string& input )
	{
		auto	newStr = input;

		std::transform ( newStr.begin (), newStr.end (), newStr.begin (), [] ( unsigned char c ) { return std::tolower ( c ); } );

		return newStr;
	}

}
