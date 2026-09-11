//
// Created by 33550 on 2026/9/10.
//
template<int N>
constexpr int factorial()
{
    static_assert(N >= 0 && "error,N must be non-negative");

    int ans{1};
    for (int i{1}; i <= N; ++i) { ans *= i; }
    return ans;
}

int main()
{
    static_assert(factorial<0>() == 1);
    static_assert(factorial<3>() == 6);
    static_assert(factorial<5>() == 120);

    //factorial<-3>(); // should fail to compile

    return 0;
}
