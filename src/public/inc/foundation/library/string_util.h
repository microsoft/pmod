/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* File:string_util.h
****/
#pragma once

#include <foundation/string_t.h>
#include <pal/library/trace_util.h>

#include <stdarg.h>
#include <vector>
#include <algorithm>
#include <cctype>

namespace foundation {

    namespace library {

        class string_util
        {
        public:

            // trim from start
            template <class T>
            static inline T &ltrim(T &s) 
            {
                s.erase(s.begin(), std::find_if(s.begin(), s.end(), std::not1(std::ptr_fun<int, int>(std::isspace))));
                return s;
            }

            // trim from end
            template <class T>
            static inline T &rtrim(T &s) {
                s.erase(std::find_if(s.rbegin(), s.rend(), std::not1(std::ptr_fun<int, int>(std::isspace))).base(), s.end());
                return s;
            }

            // trim from both ends
            template <class T>
            static inline T &trim(T &s)
            {
                return ltrim(rtrim(s));
            }


            inline static std::vector<std::string> split(const std::string &s, const char * delimiters) {
                return _split<std::string>(s, delimiters);
            }

            inline static std::vector<std::string> split(const std::string &s, const char delimiter) {
                return split(s, from_value_type<std::string>(delimiter).c_str());
            }

            inline static std::vector<foundation::string_t> split_t(const foundation::string_t &s, LPCSTR_t delimiters) {
                return _split<foundation::string_t>(s, delimiters);
            }

            inline static std::vector<foundation::string_t> split_t(const foundation::string_t &s, const CHAR_t delimiter) {
                return split_t(s, from_value_type<foundation::string_t>(delimiter).c_str());
            }

            inline static std::vector<std::wstring> split_w(const std::wstring &s, const wchar_t* delimiters) {
                return _split<std::wstring>(s, delimiters);
            }

            inline static std::vector<std::wstring> split_w(const std::wstring &s, const wchar_t delimiter) {
                return split_w(s, from_value_type<std::wstring>(delimiter).c_str());
            }

            template <class T>
            static void find_and_replace(T& str, const T& search, const T& replace)
            {
                typename T::size_type pos = 0;
                while ((pos = str.find(search, pos)) != T::npos) {
                    str.replace(pos, search.size(), replace);
                    pos += replace.size();
                }
            }

            template <class T>
            static bool ends_with_t(T const& str, T const& what) {
                return what.size() <= str.size()
                    && str.find(what, str.size() - what.size()) != str.npos;
            }

            inline static bool ends_with(std::wstring const& str, std::wstring const& what) {
                return ends_with_t(str, what);
            }

            inline static bool ends_with(std::string const& str, std::string const& what) {
                return ends_with_t(str, what);
            }

            template <class T>
            static void to_upper(T& str)
            {
                std::transform(str.begin(), str.end(), str.begin(), ::toupper);
            }

            template <class T>
            static void to_lower(T& str)
            {
                std::transform(str.begin(), str.end(), str.begin(), ::tolower);
            }

            template <class T, class S>
            static T _format(const typename T::value_type * _Format, _In_ va_list args)
            {
                const typename T::value_type *p_replace;

                int nIndex = 0;
                T str = _Format;
                while ((p_replace = va_arg(args, const typename T::value_type *)))
                {
                    S replace_str;
                    replace_str << "{" << nIndex++ << "}";

                    find_and_replace<T>(str, replace_str.str(), p_replace);
                }
                return str;
            }

            static string_t format_t(LPCSTR_t _Format, ...)
            {
                va_list args;
                va_start(args, _Format);     /* Initialize variable arguments. */
                return _format<string_t, ostringstream_t>(_Format, args);
            }

            static std::string format(const char * _Format, ...)
            {
                va_list args;
                va_start(args, _Format);     /* Initialize variable arguments. */
                return _format<std::string, std::ostringstream>(_Format, args);
            }

            static std::wstring format_w(const wchar_t * _Format, ...)
            {
                va_list args;
                va_start(args, _Format);     /* Initialize variable arguments. */
                return _format<std::wstring, std::wostringstream>(_Format, args);
            }

            inline static string_t to_lower_case_convention_t(LPCSTR_t p_str)
            {
                return to_lower_case_convention<string_t>(p_str, U('_'));
            }

            inline static std::wstring to_lower_case_convention_w(const wchar_t *p_str)
            {
                return to_lower_case_convention<std::wstring>(p_str, L'_');
            }

            static string_t format_string_t(const CHAR_t * _Format, ...)
            {
                CHAR_t buffer[2048];

                va_list argList;
                va_start(argList, _Format);     /* Initialize variable arguments. */
                pal::library::_Sprintf_helper(buffer, 2048, _Format, argList);
                return buffer;
            }
#ifdef __UTF16_STRINGS
            static std::string format_string(const char * _Format, ...)
            {
                char buffer[2048];

                va_list argList;
                va_start(argList, _Format);     /* Initialize variable arguments. */
                pal::library::_Sprintf_helper(buffer, 2048, _Format, argList);
                return buffer;
            }
#else
#define  format_string format_string_t
#endif
            private:
                template <class S>
                static S from_value_type(typename S::value_type ch)
                {
                    S s;
                    s += ch;
                    return s;
                }

                template <class S>
                static std::vector<S> &_split(const S& s, const S& delimiter, std::vector<S> &elems)
                {
                    size_t pos = 0;
                    size_t next_pos = 0;
                    while ((next_pos = s.find_first_of(delimiter, pos)) != S::npos)
                    {
                        elems.push_back(s.substr(pos, next_pos - pos));
                        pos = next_pos;
                        while (delimiter.find_first_of(s[pos]) != S::npos)
                        {
                            ++pos;
                        }
                    }
                    S last(s.substr(pos));
                    if (last.length())
                    {
                        elems.push_back(last);
                    }
                    return elems;
                }

                template <class S>
                static std::vector<S> _split(const S &s, const typename S::value_type *delimiters) {
                    std::vector<S> elems;
                    _split(s, S(delimiters), elems);
                    return elems;
                }

                template <class T>
                static T to_lower_case_convention(
                    const typename T::value_type *p_string,
                    const typename T::value_type token)
                {
                    T s;
                    typename T::value_type c;
                    while ((c = *p_string++) != 0)
                    {
                        if (isupper(c))
                        {
                            if (s.length())
                            {
                                s += token;
                            }
                            s += tolower(c);
                        }
                        else
                        {
                            s += c;
                        }
                    }
                    return s;
                }


        };

    }

}

