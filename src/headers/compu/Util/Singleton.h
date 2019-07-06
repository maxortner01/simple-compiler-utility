#pragma once

namespace compu
{
namespace Util
{
    template<typename T>
    struct Singleton
    {
        static T &get()
        {
            static T* t;

            if (!t)
            {
                t = new T();
            }

            return *t;
        }
    };
}
}