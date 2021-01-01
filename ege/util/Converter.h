/*
*
*   ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
*
*      ,----  ,----  ,----
*      |      |      |
*      |----  | --,  |----
*      |      |   |  |
*      '----  '---'  '----
*
*     Framework Library for Hexagon
*
*    Copyright (c) Sppmacd 2020 - 2021
*
*    Permission is hereby granted, free of charge, to any person obtaining a copy
*    of this software and associated documentation files (the "Software"), to deal
*    in the Software without restriction, including without limitation the rights
*    to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
*    copies of the Software, and to permit persons to whom the Software is
*    furnished to do so, subject to the following conditions:
*
*    The above copyright notice and this permission notice shall be included in all
*    copies or substantial portions of the Software.
*
*    THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
*    IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
*    FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
*    AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
*    LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
*    OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
*    SOFTWARE.
*
*   ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
*
*/

#pragma once

#include <memory>

namespace EGE
{

class Object;

// Converter is responsible for
// creating object in in()!
template<class I, class O = I>
class Converter
{
public:
    typedef I InputStreamType;
    typedef O OutputStreamType;

    virtual bool in(InputStreamType& input, std::shared_ptr<Object>& object) const = 0;
    virtual bool out(OutputStreamType& output, const Object& object) const = 0;
};

// Common Types
typedef Converter<std::istream, std::ostream> IOStreamConverter;

namespace Internal
{

template<class I, class O>
struct _ConverterInput
{
    std::shared_ptr<Object>& object;
    const Converter<I, O>& converter;

    _ConverterInput(std::shared_ptr<Object>& _o, const Converter<I, O>& _c)
    : object(_o), converter(_c) {}
};

template<class I, class O>
struct _ConverterOutput
{
    Object& object;
    const Converter<I, O>& converter;

    _ConverterOutput(Object& _o, const Converter<I, O>& _c)
    : object(_o), converter(_c) {}
};

}

template<class I, class O>
Internal::_ConverterInput<I, O> objectIn(std::shared_ptr<Object>& _o, const Converter<I, O>& _c)
{
    return Internal::_ConverterInput<I, O>(_o, _c);
}

template<class I, class O>
Internal::_ConverterOutput<I, O> objectOut(Object& _o, const Converter<I, O>& _c)
{
    return Internal::_ConverterOutput<I, O>(_o, _c);
}

template<class I, class O>
bool convertTo(const I& input, O& output, const Converter<I, O>& inputConverter, const Converter<I, O>& outputConverter)
{
    std::shared_ptr<Object> tmp;
    if(!inputConverter.in(input, tmp)) return false;
    if(!outputConverter.out(output, *tmp.get())) return false;
    return true;
}

} //namespace EGE

template<class I, class _I, class O>
I& operator>>(_I& stream, const EGE::Internal::_ConverterInput<I, O>& in)
{
    in.converter.in(stream, in.object);
    return stream;
}

template<class I, class _O, class O>
O& operator<<(_O& stream, const EGE::Internal::_ConverterOutput<I, O>& out)
{
    out.converter.out(stream, out.object);
    return stream;
}
