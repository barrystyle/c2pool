#include <devcore/logger.h>
#include <cstring>
#include <vector>
#include <uint256.h>
#include <optional>
#include "stream.h"
using namespace std;

template <typename T>
struct ListType
{
    vector<T> l;

    ListType() {}

    ListType(T arr[], size_t len)
    {
        l.insert(l.end(), arr, arr + len);
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
};

struct StrType
{
    string str;

    PackStream &write(PackStream &stream) const
    {
        LOG_TRACE << "StrType Worked!";

        char s[str.length() + 1];
        strcpy(s, str.c_str());

        ListType<char> list_s(s, str.length());

        stream << list_s;

        return stream;
    }
};

template <typename INT_T>
struct IntType
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

    PackStream &write(PackStream &stream) const
    {
        LOG_TRACE << "IntType Worked!";

        INT_T value2 = value;
        unsigned char *packed = reinterpret_cast<unsigned char *>(&value2);
        int32_t len = sizeof(value2) / sizeof(*packed);

        PackStream s(packed, len);
        stream << s;

        return stream;
    }
};

#define INT8 uint8_t
#define INT16 uint16_t
#define INT32 uint32_t
#define INT64 uint64_t
#define INT256 uint256

#define IntType(bytes) IntType<INT##bytes>

template <StreamEnumType ENUM_T, StreamObjType PACK_TYPE = IntType(32)>
struct EnumType
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

    PackStream &write(PackStream &stream) const
    {
        LOG_TRACE << "EnumType Worked!";

        PACK_TYPE v((typename PACK_TYPE::value_type) value);
        stream << v;

        return stream;
    }
};

template <StreamObjType ObjType>
class PossibleNoneType
{
private:
    ObjType none_value;

public:
    optional<ObjType> value;

    PackStream &write(PackStream &stream) const
    {
        cout << "NonValueType Worked!" << endl;

        if (value.has_value())
        {
            value.value.write(stream);
        }
        else
        {
            none_value.write(stream);
        }
        return stream;
    }
};