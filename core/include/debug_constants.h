#pragma once
#include "typedefs.h"
#include <functional>
#include <vector>
#include <string>

namespace br
{
    typedef std::vector<std::string> tokens;
    typedef tokens::iterator tokensIterator;

    struct token_callback
    {
        std::string queryString;
        u32 tokenCount;
        std::function<void (const tokensIterator&, const tokensIterator&)> callback;
    };

    typedef std::vector<token_callback> token_callbacks;

    inline tokens tokenize(const std::string& _string, const u32& _whiteSpaceCutoff = 32)
    {
        u32 wordCount = 1;
        for (const unsigned char& c : _string)
            wordCount += c <= _whiteSpaceCutoff;

        u32 i = 0;
        tokens stringTokens(wordCount);
        for (const unsigned char& c : _string)
        {
            if (c <= _whiteSpaceCutoff)
            {
                i++;
                continue;
            }
            stringTokens[i] += c;
        }

        return stringTokens;
    }

    inline const bool parse_tokens(tokens& _tokens, const token_callbacks& _callbacks)
    {
        for (tokensIterator it = _tokens.begin(); it != _tokens.end(); it++)
        {
            const std::string& token = *it;

            for (const token_callback& callback : _callbacks)
            {
                if (callback.queryString == token)
                {
                    tokensIterator first = it;
                    tokensIterator last = it + callback.tokenCount;
                    if (last - 1 == _tokens.end())
                        return false;
                    
                    callback.callback(first, last);

                    it = last;
                    break;
                }
            }

            if (it == _tokens.end())
                break;
        }

        return true;
    }
} 