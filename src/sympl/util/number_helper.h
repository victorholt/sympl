/**********************************************************
 * Author:  GameSencha, LLC
 * The MIT License (MIT)
 *
 *  Permission is hereby granted, free of charge, to any person obtaining a
 *  copy of this software and associated documentation files (the "Software"),
 *  to deal in the Software without restriction, including without limitation
 *  the rights to use, copy, modify, merge, publish, distribute, sublicense,
 *  and/or sell copies of the Software, and to permit persons to whom the
 *  Software is furnished to do so, subject to the following conditions:
 *
 *  The above copyright notice and this permission notice shall be included in
 *  all copies or substantial portions of the Software.
 *
 *  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 *  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 *  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
 *  THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 *  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 *  FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
 *  DEALINGS IN THE SOFTWARE.
 *
 **********************************************************/
#pragma once

#include <sympl/core/sympl_pch.h>

#include <fmt/format.h>

sympl_nsstart

class NumberHelper
{
public:
    /// Attempts to parse an integer from a string.
    static bool TryParseInt(const char* str, int* output) {
        char *end;
        *output = std::strtol(str, &end, 10);
        return (*end == '\0');
    }
    /// Attempts to parse an integer from a string.
    static bool TryParseLong(const char* str, long long* output) {
        char *end;
        *output = std::strtoll(str, &end, 10);
        return (*end == '\0');
    }
    /// Attempts to parse a float from a string.
    static bool TryParseFloat(const char* str, float* output) {
        char *end;
        *output = std::strtof(str, &end);
        return (*end == '\0');
    }
    /// Attempts to parse an double from a string.
    static bool TryParseDouble(const char* str, double* output) {
        char *end;
        *output = std::strtod(str, &end);
        return (*end == '\0');
    }
    /// Attemps to see if a given string is a date.
    static bool IsDatePattern(const char* str, int maxPeriods = 2) {
        // Go through and find if this is a date.
        char curChar = -1;
        char prevChar = -1;
        size_t totalLen = strlen(str);
        int totalPeriods = 0;

        for (size_t i = 0; i < totalLen; i++) {
            curChar = str[i];
            bool isNum = std::isdigit(curChar) > 0;

            if (!isNum) {
                if (std::isdigit(prevChar) && curChar == '.' && ((i + 1) < totalLen) && totalPeriods < maxPeriods) {
                    totalPeriods++;
                    continue;
                }
                return false;
            }
            prevChar = curChar;
        }

        return true;
    }
    /// Round down a number and return a float.
    static float RoundDown(float number, int decimalPlaces = 2)
    {
        int decimalMul = 10 * decimalPlaces;
        return floorf(number * decimalMul) / decimalMul;
    }
    /// Round down a number and return a float.
    static float RoundNearest(float number, int decimalPlaces = 2)
    {
        int decimalMul = 10 * decimalPlaces;
        return roundf(number * decimalMul) / decimalMul;
    }
    /// Round down a number and return a float.
    static float RoundUp(float number, int decimalPlaces = 2)
    {
        int decimalMul = 10 * decimalPlaces;
        return ceilf(number * decimalMul) / decimalMul;
    }

    /// Round to the nearest precision.
    /// https://www.gamedev.net/topic/310462-c-decimal-rounding/
    template< typename T, const int Ndp >
    static T RoundTo( const T val )
    {
        T raised  = pow( 10, (double)( abs(Ndp) ) );
        T temp 	 = val * raised;
        T rounded = static_cast<T>( floor( temp ) );

        if( temp - rounded >= (T)0.5 ) {
            rounded = static_cast<T>( ceil( temp ) );
        }

        return static_cast<T>(rounded / raised);
    }

    /// http://stackoverflow.com/questions/3383817/limit-floating-point-precision
    static double GetFloatPrecision(double value, double precision)
    {
        return (floor((value * pow(10, precision) + 0.5)) / pow(10, precision));
    }

    static std::string NumberToString (long Number)
    {
        std::ostringstream ss;
        ss << Number;
        return ss.str();
    }

    static void FloatToString(float num, std::string& output, unsigned decimalPlaces = 2) {
        std::string str = fmt::format("{0:f}", num);
        bool        p = false;
        unsigned    pi = 0;

        for (int i = 0; i < str.length(); i++) {
            if (!p && str[i] == '.') {
                p = true;
                output.append(1, '.');
                continue;
            }
            if (p && pi >= decimalPlaces) {
                break;
            } else if (p) {
                pi++;
            }

            output.append(1, str[i]);
        }
    }

    static const char* GetFloatToString(float num, unsigned decimalPlaces = 2) {
        std::string output;// = fmt::format("{0:f}", num).c_str();
        FloatToString(num, output, decimalPlaces);
        return output.c_str();
    }

    static unsigned lastSeed_;
};

sympl_nsend
