#pragma once
#include "Configs.h"

namespace network
{
    struct SWritev
    {
        char* buff;
        int len;

        SWritev() :buff(nullptr), len(0) {}

        void clear() { buff = nullptr, len = 0; }
    };

    class CRingBuff
    {
        using uint32 = unsigned int;

        CRingBuff(const CRingBuff&) = delete;
        CRingBuff& operator=(const CRingBuff&) = delete;

    public:

        explicit CRingBuff(uint32 capacity);

        ~CRingBuff();

        void write(char* buff, uint32 len);

        void read(char* buff, uint32 len);

        void write(uint32 len);

        SWritev* writerv();

        bool empty() { return _size > 0; }

        uint32 writableBytes() { return _capacity - _size; };

        uint32 readableBytes() { return _size; }

        uint32 size() { return _size; }

        uint32 capacity() { return _capacity; }

        void clear();

    private:
        void ensure(uint32 capacity);

        uint32 _capacity;
        uint32 _front;
        uint32 _end;
        uint32 _size;
        char* _buff;
        SWritev* _writerv;
    };
}

