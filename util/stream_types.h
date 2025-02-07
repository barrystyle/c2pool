#pragma once

#include <devcore/logger.h>
#include <cstring>
#include <string>
#include <vector>
#include <list>
#include <btclibs/uint256.h>
#include <btclibs/arith_uint256.h>
#include <optional>
#include "stream.h"
#include "math.h"
using namespace std;

template <typename T>
struct ListType : Maker<T>
{
    vector<T> l;

    ListType() {}

    ListType(T arr[], size_t len)
    {
        l.insert(l.end(), arr, arr + len);
    }

    ListType(vector<T> arr)
    {
        l = vector<T>(arr);
    }

    auto &operator=(vector<T> _value)
    {
        l = _value;
        return *this;
    }

    PackStream &write(PackStream &stream) const
    {
        LOG_TRACE << "ListType Worked!";
        auto len = l.size();
        stream << len;
        for (auto v : l)
        {
            stream << v;
        }

        return stream;
    }

    PackStream &read(PackStream &stream)
    {
        auto len = 0;
        stream >> len;
        for (int i = 0; i < len; i++)
        {
            T temp;
            stream >> temp;
            l.push_back(temp);
        }
        return stream;
    }
};

//In p2pool - VarStrType
struct StrType : Maker<string>
{
    string str;

    auto &operator=(std::string _str)
    {
        str = _str;
        return *this;
    }

    PackStream &write(PackStream &stream) const
    {
        LOG_TRACE << "StrType Worked!";

        char s[str.length() + 1];
        strcpy(s, str.c_str());

        ListType<char> list_s(s, str.length());

        stream << list_s;

        return stream;
    }

    PackStream &read(PackStream &stream)
    {
        ListType<char> list_s;
        stream >> list_s;

        str = string(list_s.l.begin(), list_s.l.end());
        return stream;
    }

    string get()
    {
        return str;
    }
};

//TODO: TEST
template <int SIZE>
struct FixedStrType : Maker<string>
{
    string str;

    FixedStrType()
    {
    }

    FixedStrType(string _str)
    {
        if (_str.length() != SIZE)
        {
            throw std::invalid_argument("Incorrect length str in FixedStrType");
        }
        str = _str;
    }

    PackStream &write(PackStream &stream) const
    {
        LOG_TRACE << "FixedStrType Worked!";

        for (auto c : str)
        {
            stream << c;
        }

        return stream;
    }

    PackStream &read(PackStream &stream)
    {
        char *c_str = new char[SIZE];
        for (int i = 0; i < SIZE; i++)
        {
            stream >> c_str[i];
        }

        str = string(c_str, c_str + SIZE);
        return stream;
    }
};

template <typename INT_T>
struct IntType : Maker<INT_T>
{
    typedef INT_T value_type;
    INT_T value;

    IntType() {}

    IntType(INT_T _value)
    {
        value = _value;
    }

    IntType<INT_T> &set(const INT_T &_value)
    {
        value = _value;
        return *this;
    }

    INT_T get() const
    {
        return value;
    }

    auto &operator=(INT_T _value)
    {
        value = _value;
        return *this;
    }

    PackStream &write(PackStream &stream)
    {
        LOG_TRACE << "IntType Worked!";

        INT_T value2 = value;
        unsigned char *packed = reinterpret_cast<unsigned char *>(&value2);
        int32_t len = sizeof(value2) / sizeof(*packed);

        PackStream s(packed, len);
        stream << s;

        return stream;
    }

    virtual PackStream &read(PackStream &stream)
    {
        unsigned char *packed = new unsigned char[CALC_SIZE(INT_T)];
        //int32_t len = sizeof(value2) / sizeof(*packed);

        for (int i = 0; i < CALC_SIZE(INT_T); i++)
        {
            packed[i] = stream.data[i];
            stream.data.erase(stream.data.begin(), stream.data.begin() + 1);
        }
        auto *_value = reinterpret_cast<INT_T *>(packed);
        value = *_value;

        return stream;
    }
};

template <typename INT_T>
struct ULongIntType : Maker<INT_T>
{
    typedef INT_T value_type;
    INT_T value;

    ULongIntType() {}

    ULongIntType(INT_T _value)
    {
        value = _value;
    }

    ULongIntType<INT_T> &set(const INT_T &_value)
    {
        value = _value;
        return *this;
    }

    INT_T get() const
    {
        return value;
    }

    auto &operator=(INT_T _value)
    {
        value = _value;
        return *this;
    }

    virtual PackStream &write(PackStream &stream)
    {
        LOG_TRACE << "ULongIntType Worked!";

        INT_T value2 = value;
        unsigned char *packed = reinterpret_cast<unsigned char *>(&value2);
        int32_t len = std::distance(value2.begin(), value.end());

        PackStream s(packed, len);
        stream << s;

        return stream;
    }

    virtual PackStream &read(PackStream &stream)
    {
        unsigned char *packed = new unsigned char[value_type::WIDTH];
        //int32_t len = sizeof(value2) / sizeof(*packed);

        for (int i = 0; i < value_type::WIDTH; i++)
        {
            packed[i] = stream.data[i];
            stream.data.erase(stream.data.begin(), stream.data.begin() + 1);
        }
        auto *_value = reinterpret_cast<INT_T *>(packed);
        value = *_value;

        return stream;
    }
};

#define INT8 IntType<uint8_t>
#define INT16 IntType<uint16_t>
#define INT32 IntType<uint32_t>
#define INT64 IntType<uint64_t>
#define INT160 ULongIntType<uint160>
#define INT128 ULongIntType<uint128>
#define INT256 ULongIntType<uint256>

#define IntType(bytes) INT##bytes

struct VarIntType : Maker<uint64_t>
{
    typedef uint64_t value_type;
    uint64_t value;

    VarIntType() {}

    VarIntType(uint64_t _v)
    {
        value = _v;
    }

    PackStream &write(PackStream &stream)
    {
        stream << value;

        return stream;
    }

    PackStream &read(PackStream &stream)
    {
        stream >> value;

        return stream;
    }
};

template <StreamEnumType ENUM_T, StreamObjType PACK_TYPE = IntType(32)>
struct EnumType : Maker<ENUM_T>
{
    ENUM_T value;

    EnumType() {}

    EnumType(ENUM_T _value)
    {
        value = _value;
    }

    void set(ENUM_T _value)
    {
        value = _value;
    }

    PackStream &write(PackStream &stream)
    {
        LOG_TRACE << "EnumType Worked!";

        PACK_TYPE v((typename PACK_TYPE::value_type)value);
        stream << v;

        return stream;
    }

    PackStream &read(PackStream &stream)
    {
        PACK_TYPE v;
        stream >> v;

        value = (ENUM_T)v.value;

        return stream;
    }
};

template <StreamObjType ObjType>
class PossibleNoneType : public Maker<ObjType>
{
private:
    ObjType none_value;

public:
    optional<ObjType> value;

    ObjType get() const
    {
        if (value.has_value())
        {
            return value.value();
        }
        else
        {
            return none_value;
        }
    }

    auto &operator=(ObjType obj)
    {
        value = obj;
        return *this;
    }

    PackStream &write(PackStream &stream)
    {
        cout << "NonValueType Worked!" << endl;

        if (value.has_value())
        {
            value.value().write(stream);
        }
        else
        {
            none_value.write(stream);
        }
        return stream;
    }

    PackStream &read(PackStream &stream)
    {
        ObjType *_value;
        stream >> *_value;

        value = make_optional(*_value);
        return stream;
    }
};

//TODO: test

struct FloatingInteger
{
    IntType(32) bits;

    FloatingInteger()
    {
    }

    FloatingInteger(IntType(32) _bits)
    {
        bits = _bits;
    }

    uint256 target()
    {
        arith_uint256 res(bits.value && 0x00ffffff);

        res << (8 * ((bits.value >> 24) - 3));

        return ArithToUint256(res);
    }

    //TODO: test
    static FloatingInteger from_target_upper_bound(uint256 target)
    {
        std::string str_n = c2pool::math::natural_to_string(target);
        list<unsigned char> n;
        n.insert(n.end(), str_n.begin(), str_n.end());

        if (n.size() > 0 && *n.begin() >= 128)
        {
            n.push_front('\0');
        }

        list<unsigned char> bits2;
        bits2.push_back((unsigned char)n.size());
        {
            list<unsigned char> temp_bits(n);
            for (int i = 0; i < 3; i++)
                temp_bits.push_back('\0');
            auto vi = temp_bits.begin();
            std::advance(vi, 3);
            bits2.insert(bits2.end(), temp_bits.begin(), vi);
        }
        bits2.reverse();

        IntType(32) unpacked_bits;

        unsigned char *bits = new unsigned char[bits2.size()];
        std::copy(bits2.begin(), bits2.end(), bits);
        PackStream stream(bits, bits2.size());
        stream >> unpacked_bits;

        return FloatingInteger(unpacked_bits);
    }
};

//TODO: test
struct FloatingIntegerType
{
    FloatingInteger bits;

    PackStream &write(PackStream &stream)
    {
        stream << bits.bits;

        return stream;
    }

    PackStream &read(PackStream &stream)
    {
        IntType(32) _bits;
        stream >> _bits;
        bits = FloatingInteger(_bits);

        return stream;
    }
};