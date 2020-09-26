/*
EGE - Extendable Game Engine
Copyright (c) Sppmacd 2020
*/

// It should be included only from Vector.h!
#if !defined(VECTOR_H) || defined(VECTOR_H_END)
#error "VectorOperations should not be included directly"
#endif

#include "Math.h"

#include <cmath>
#include <ege/main/Config.h>
#include <ege/util/PointerUtils.h>

namespace EGE
{

namespace VectorOperations
{

template<class T>
DegreeAngle angleTo(Vector2<T> begin, Vector2<T> end)
{
    T dx = end.x - begin.x;
    T dy = end.y - begin.y;
    return rad2deg(std::atan2(dx, dy));
}

template<class T>
double distanceTo(Vector2<T> begin, Vector2<T> end)
{
    T dx = end.x - begin.x;
    T dy = end.y - begin.y;
    return std::sqrt((double)(dx*dx+dy*dy));
}

template<class T>
double length(Vector2<T> vector)
{
    return std::sqrt((double)(vector.x*vector.x+vector.y*vector.y));
}

template<class T>
Vector2<T> fromPolar(PolarVector2<T> polar)
{
    return Vector2<T>(polar.length * std::sin(deg2rad(polar.angle)), polar.length * std::cos(deg2rad(polar.angle)));
}

template<class T>
PolarVector2<T> toPolar(Vector2<T> cartesian)
{
    return PolarVector2<T>(angleTo(Vector2<T>(), cartesian), distanceTo(Vector2<T>(), cartesian));
}

} // namespace VectorOperations

} // namespace EGE

// Operations
template<class T>
EGE::Vector2<T> operator+(EGE::Vector2<T> _1, EGE::Vector2<T> _2)
{
    return EGE::Vector2<T>(_1.x + _2.x, _1.y + _2.y);
}

template<class T>
EGE::Vector2<T> operator-(EGE::Vector2<T> _1, EGE::Vector2<T> _2)
{
    return EGE::Vector2<T>(_1.x - _2.x, _1.y - _2.y);
}

template<class T>
EGE::Vector2<T> operator*(EGE::Vector2<T> _1, T _2)
{
    return EGE::Vector2<T>(_1.x * _2, _1.y * _2);
}

template<class T>
EGE::Vector2<T> operator/(EGE::Vector2<T> _1, T _2)
{
    ASSERT(_2 != 0);
    return EGE::Vector2<T>(_1.x / _2, _1.y / _2);
}

// Equal / Not equal
template<class T>
bool operator==(EGE::Vector2<T> _1, EGE::Vector2<T> _2)
{
    return _1.x == _2.x && _1.y == _2.y;
}

template<class T>
bool operator!=(EGE::Vector2<T> _1, EGE::Vector2<T> _2)
{
    return !(_1 == _2);
}

// Less / Not less
template<class T>
bool operator<(EGE::Vector2<T> _1, EGE::Vector2<T> _2)
{
    return EGE::VectorOperations::length(_1) < EGE::VectorOperations::length(_2);
}

template<class T>
bool operator>=(EGE::Vector2<T> _1, EGE::Vector2<T> _2)
{
    return !(_1 < _2);
}

// Greater / Not greater
template<class T>
bool operator>(EGE::Vector2<T> _1, EGE::Vector2<T> _2)
{
    return EGE::VectorOperations::length(_1) > EGE::VectorOperations::length(_2);
}

template<class T>
bool operator<=(EGE::Vector2<T> _1, EGE::Vector2<T> _2)
{
    return !(_1 > _2);
}

// Operations
template<class T>
EGE::Vector2<T>& operator+=(EGE::Vector2<T>& _1, EGE::Vector2<T> _2)
{
    _1.x += _2.x;
    _1.y += _2.y;
    return _1;
}

template<class T>
EGE::Vector2<T>& operator-=(EGE::Vector2<T>& _1, EGE::Vector2<T> _2)
{
    _1.x -= _2.x;
    _1.y -= _2.y;
    return _1;
}

template<class T>
EGE::Vector2<T>& operator*=(EGE::Vector2<T>& _1, T _2)
{
    _1.x *= _2;
    _1.y *= _2;
    return _1;
}

template<class T>
EGE::Vector2<T>& operator/=(EGE::Vector2<T>& _1, T _2)
{
    ASSERT(_2 != 0);
    _1.x /= _2;
    _1.y /= _2;
    return _1;
}
