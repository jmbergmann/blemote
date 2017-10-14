#pragma once


namespace lib {

template <int Num, int Den = 1>
class Ratio
{
public:
    constexpr static int num = Num;
    constexpr static int den = Den;
};

typedef Ratio<1, 1'000'000'000> Nano;
typedef Ratio<1, 1'000'000> Micro;
typedef Ratio<1, 1'000> Milli;

} // namespace lib
