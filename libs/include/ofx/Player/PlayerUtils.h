 // =============================================================================
//
// Copyright (c) 2013-2016 Christopher Baker <http://christopherbaker.net>
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
// THE SOFTWARE.
//
// =============================================================================


#pragma once


#include "ofBaseTypes.h"


/// \brief Compare two floating point types for equality.
///
/// From C++ FAQ:
///
/// Floating point arithmetic is different from real number arithmetic.
/// Never use `==` to compare two floating point numbers.
///
/// This solution is not completely symmetric, meaning it is possible for
/// `ofIsFloatEqual(x, y) != ofIsFloatEqual(y, x)`. From a practical
/// standpoint, this does not usually occur when the magnitudes of x and y are
/// significantly larger than epsilon, but your mileage may vary.
///
/// \sa https://isocpp.org/wiki/faq/newbie#floating-point-arith
/// \sa https://docs.oracle.com/cd/E19957-01/806-3568/ncg_goldberg.html
/// \tparam The floating point data type.
/// \param a The first floating point type variable to compare.
/// \param b The second floating point type variable to compare.
/// \returns True if `std::abs(x - y) <= std::numeric_limits<Type>::epsilon() * std::abs(x)`.
template<typename Type>
typename std::enable_if<std::is_floating_point<Type>::value, bool>::type ofxIsFloatEqual(const Type& a, const Type& b)
{
    return std::abs(a - b) <= std::numeric_limits<Type>::epsilon() * std::abs(a);
}


namespace ofx {
namespace Player {


} } // namespace ofx::Player
