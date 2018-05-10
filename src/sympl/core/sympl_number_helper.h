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
#include <sympl/core/sympl_object.h>

sympl_nsstart

class NumberHelper
{
public:
    /// Attempts to parse an integer from a string.
    static bool TryParseInt(const char* str, int* output) {
        std::istringstream iss(str);
        int i;
        iss >> std::noskipws >> i; // noskipws considers leading whitespace invalid
        // Check the entire string was consumed and if either failbit or badbit is set
        *output = i;
        return iss.eof() && !iss.fail();
    }
    /// Attempts to parse an unsigned integer from a string.
    static bool TryParseUnsigned(const char* str, unsigned* output) {
        std::istringstream iss(str);
        unsigned i;
        iss >> std::noskipws >> i; // noskipws considers leading whitespace invalid
        // Check the entire string was consumed and if either failbit or badbit is set
        *output = i;
        return iss.eof() && !iss.fail();
    }
    /// Attempts to parse a float from a string.
    static bool TryParseFloat(const char* str, float* output) {
        /*std::string::size_type sz;
        try { *output = std::stof(str, &sz); }
        catch (std::exception) {
            return false;
        }
        return true;*/

        std::istringstream iss(str);
        float f;
        iss >> std::noskipws >> f; // noskipws considers leading whitespace invalid
        // Check the entire string was consumed and if either failbit or badbit is set
        *output = f;
        return iss.eof() && !iss.fail();
    }
    /// Attempts to parse an double from a string.
    static bool TryParseDouble(const char* str, double* output) {
        std::string::size_type sz;
        try { *output = std::stod(str, &sz); }
        catch (std::exception) {
            return false;
        }
        return true;
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

    // Seeds the random number generator.
    static void SeedRandom(unsigned seed)
    {
        if (seed == 0) {
            struct timeval t1;
            w_gettimeofday(&t1, NULL);
            unsigned newSeed = t1.tv_usec * t1.tv_sec;
            if (newSeed != NumberHelper::lastSeed_) {
                NumberHelper::lastSeed_ = newSeed;
                Urho3D::SetRandomSeed(newSeed);
            }
        } else {
            Urho3D::SetRandomSeed(seed);
        }
    }

    // Generate a random integer.
    static int Random(bool randomSeed = false) {
        if (randomSeed) {
            SeedRandom(0);
        }
        return Urho3D::Rand();
    }
    // Generate a random integer between a and b.
    static int Random(int a, int b, bool randomSeed = false) {
//        return Random(randomSeed) % b + a;
        if (randomSeed) {
            SeedRandom(0);
        }
        return Urho3D::Random(a, b);
    }
    // Generate a random float number.
    static float RandomFloat(bool randomSeed = false) {
        if (randomSeed) {
            SeedRandom(0);
        }
        return Urho3D::Random();
    }
    // Generate a random float number between a and b.
    static float RandomFloat(float a, float b, bool randomSeed = false) {
        if (randomSeed) {
            SeedRandom(0);
        }
        return Urho3D::Random(a, b);
    }

    static void FloatToString(float num, String& output, int decimalPlaces = 2) {
        output = fmt::format("{0:+f}", num).c_str();
        if (!output.Contains('.')) {
            output.Append(".");
            for (int i = 0; i < decimalPlaces; i++) {
                output.Append("0");
            }
        }
    }

    static const char* GetFloatToString(float num, int decimalPlaces = 2) {
        String output = fmt::format("{0:+f}", num).c_str();
        FloatToString(num, output, decimalPlaces);
        return output.CString();
    }

    static unsigned lastSeed_;
};

sympl_nsend