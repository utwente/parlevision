#ifndef PLVUTIL_H
#define PLVUTIL_H

namespace plv
{

    inline static bool isEven(int n)
    {
        return (n&1)==0;
    }

    inline static bool isOdd(int n)
    {
        return !isEven(n);
    }
}

#endif
