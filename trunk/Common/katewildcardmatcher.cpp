#include "stdafx.h"
#include "katewildcardmatcher.h"



namespace
{
bool exactMatch(const TCHAR candidate[], const TCHAR wildcard[], int candidatePosFromRight,
        int wildcardPosFromRight, bool caseSensitive = true)
{
    for (; wildcardPosFromRight >= 0; wildcardPosFromRight--)
    {
        const TCHAR ch = wildcard[wildcardPosFromRight];
        switch (ch)
        {
        case L'*':
            if (candidatePosFromRight == -1)
            {
                break;
            }

            if (wildcardPosFromRight == 0)
            {
                return true;
            }

            // Eat all we can and go back as far as we have to
            for (int j = -1; j <= candidatePosFromRight; j++)
            {
                if (exactMatch(candidate, wildcard, j, wildcardPosFromRight - 1))
                {
                    return true;
                }
            }
            return false;

        case L'?':
            if (candidatePosFromRight == -1)
            {
                return false;
            }

            candidatePosFromRight--;
            break;

        default:
            if (candidatePosFromRight == -1)
            {
                return false;
            }

            const TCHAR candidateCh = candidate[candidatePosFromRight];
            const bool match = caseSensitive
                    ? (candidateCh == ch)
                    : (_totlower(candidateCh) == _totlower(ch));
            if (match)
            {
                candidatePosFromRight--;
            }
            else
            {
                return false;
            }
        }
    }
    return true;
}
}



bool WidecardMatch(const TCHAR candidate[], const TCHAR wildcard[], bool caseSensitive)
{
    size_t nCandidateLength = _tcslen(candidate);
    size_t nWildcardLength = _tcslen(wildcard);
    return exactMatch(candidate, wildcard, nCandidateLength - 1, nWildcardLength - 1, caseSensitive);
}
