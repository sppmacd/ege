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

#include "Dump.h"

#include <iomanip>

namespace EGE
{

void hexDump(const void* data, size_t size, HexDumpSettings settings)
{
    std::ostream& _str = settings.stream;

    for(size_t s = 0; s < size / settings.width + 1; s++)
    {
        _str << std::hex << std::setfill('0') << std::setw(8) << s * settings.width << "   ";

        // data as HEX DUMP
        for(size_t t = 0; t < settings.width; t++)
        {
            size_t off = s * settings.width + t;
            if(off < size)
            {
                unsigned char _data = ((unsigned char*)data)[off] & 0xFF;
                _str << std::hex << std::setfill('0') << std::setw(2) << (int)_data << " ";
            }
            else
            {
                _str << "   ";
            }
        }

        _str << "  ";

        // and as CHARACTERS
        for(size_t t = 0; t < settings.width; t++)
        {
            size_t off = s * settings.width + t;
            if(off < size)
            {
                unsigned char _data = ((unsigned char*)data)[off] & 0xFF;
                if(_data < 32 || _data >= 127)
                    _data = '.';
                _str << _data << " ";
            }
        }
        _str << std::dec << std::endl;
    }
}

namespace PrintObject
{

enum class IndentMode
{
    None,
    Normal,
    Object,
    LastObject
};

static void _indent(std::vector<IndentMode> modes)
{
    for(IndentMode mode: modes)
    {
        switch(mode)
        {
            case IndentMode::None:
                std::cerr << "   "; // "   "
                break;
            case IndentMode::Normal:
                std::cerr << "\u2502  "; // "|  "
                break;
            case IndentMode::Object:
                std::cerr << "\u251c\u2500 "; // "|--"
                break;
            case IndentMode::LastObject:
                std::cerr << "\u2514\u2500 "; // "L--"
                break;
        }
    }
}

static void _printPair(std::string name, SharedPtr<Object> object, std::vector<IndentMode> depth, bool isLast);

static void _printName(std::string name)
{
    std::cerr << "\e[1;32m" << name << "\e[m = ";
}

static void _printValue(SharedPtr<Object> object, std::vector<IndentMode> depth, bool isLast)
{
    if(depth.size() > 10)
    {
        std::cerr << "\e[31m...\e[m " << std::endl;
        return;
    }

    if(!object)
    {
        std::cerr << "\e[31mnull\e[m " << std::endl;
        return;
    }

    if(object->isInt())
    {
        std::cerr << "\e[35m" << object->asInt() << "\e[m" << std::endl;
    }
    else if(object->isFloat())
    {
        std::cerr << "\e[95m" << object->asFloat() << "\e[m" << std::endl;
    }
    else if(object->isString())
    {
        std::cerr << "\e[33m" << object->toString() << "\e[m" << std::endl;
    }
    else if(object->isList())
    {
        auto _vector = object->asList();
        if(!_vector.empty())
        {
            std::cerr << "\e[94m<List: " << _vector.size() << " entries>\e[m" << std::endl;
            size_t counter = 0;
            for(auto pr: _vector)
            {
                std::vector<IndentMode> depth2 = depth;

                if(counter == _vector.size() - 1 || counter >= 10)
                {
                    if(!depth2.empty())
                    {
                        if(isLast)
                            depth2.back() = IndentMode::None;
                        else
                            depth2.back() = IndentMode::Normal;
                    }
                    depth2.push_back(IndentMode::LastObject);
                    if(counter >= 10)
                    {
                        _indent(depth2);
                        std::cerr << "\e[31m...\e[m (" << _vector.size() - 10 << " remaining)" << std::endl;
                        break;
                    }
                }
                else
                {
                    if(!depth2.empty())
                    {
                        if(isLast)
                            depth2.back() = IndentMode::None;
                        else
                            depth2.back() = IndentMode::Normal;
                    }
                    depth2.push_back(IndentMode::Object);
                }

                _indent(depth2);
                _printValue(pr, depth2, counter == _vector.size() - 1);
                counter++;
            }
        }
        else
        {
            std::cerr << "\e[91m<Empty List>\e[m" << std::endl;
        }
    }
    else if(object->isMap())
    {
        auto _map = object->asMap();
        if(!_map.empty())
        {
            std::cerr << "\e[94m<Map: " << _map.size() << " entries>\e[m" << std::endl;
            size_t counter = 0;
            for(auto pr: _map)
            {
                std::vector<IndentMode> depth2 = depth;

                if(counter == _map.size() - 1 || counter > 10)
                {
                    if(!depth2.empty())
                    {
                        if(isLast)
                            depth2.back() = IndentMode::None;
                        else
                            depth2.back() = IndentMode::Normal;
                    }
                    depth2.push_back(IndentMode::LastObject);
                    if(counter > 10)
                    {
                        _indent(depth2);
                        std::cerr << "\e[31m...\e[m (" << _map.size() - 10 << " remaining)" << std::endl;
                        break;
                    }
                }
                else
                {
                    if(!depth2.empty())
                    {
                        if(isLast)
                            depth2.back() = IndentMode::None;
                        else
                            depth2.back() = IndentMode::Normal;
                    }
                    depth2.push_back(IndentMode::Object);
                }

                _printPair(pr.first, pr.second, depth2, counter == _map.size() - 1);
                counter++;
            }
        }
        else
        {
            std::cerr << "\e[91m<Empty Map>\e[m" << std::endl;
        }
    }
    else if(object->isBool())
    {
        std::cerr << "\e[36m" << object->toString() << "\e[m" << std::endl;
    }
    else
    {
        std::cerr << "\e[31m???\e[m" << std::endl;
    }
}

static void _printPair(std::string name, SharedPtr<Object> object, std::vector<IndentMode> depth, bool isLast)
{
    _indent(depth);
    _printName(name);
    _printValue(object, depth, isLast);
}

}

void printObject(SharedPtr<Object> object)
{
    PrintObject::_printPair("root", object, {}, true);
}

}
