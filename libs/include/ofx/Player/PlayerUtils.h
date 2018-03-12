//
// Copyright (c) 2013 Christopher Baker <https://christopherbaker.net>
//
// SPDX-License-Identifier:    MIT
//


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
