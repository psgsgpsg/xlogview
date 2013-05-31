#pragma once



/***
* Matches a string against a given wildcard.
* The wildcard supports '*' (".*" in regex) and '?' ("." in regex), not more.
*
* From: kde-4.5.4/kdelibs-4.5.4/kate/mode/testing/katewildcardmatcher_test.cpp
*
* @param candidate       Text to match
* @param wildcard        Wildcard to use
* @param caseSensitive   Case-sensitivity flag
* @return                True for an exact match, false otherwise
*/

bool WidecardMatch(const TCHAR candidate[], const TCHAR wildcard[], bool caseSensitive = true);
