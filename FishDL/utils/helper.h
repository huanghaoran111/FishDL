

namespace FishDL
{
namespace Helper
{

    // --------------- 构建索引序列 start ----------------
    // ref web: https://en.cppreference.com/w/cpp/utility/integer_sequence
    template<int... I> struct IndexSequence;

    namespace 
    {
        template<typename L, typename R> struct concat;

        template<int... L, int... R> 
        struct concat<IndexSequence<L...>, IndexSequence<R...>>
        {
            using type = IndexSequence<L..., (R + sizeof...(L))...>;
        };
        template<int N>
        struct MakeIndexSequence_
        {
            using type = typename concat<
                        typename MakeIndexSequence_<N/2>::type, 
                        typename MakeIndexSequence_<N - N / 2>::type
            >::type;
        };

        template<>
        struct MakeIndexSequence_<1>
        {
            using type = IndexSequence<0>;
        };

        template<>
        struct MakeIndexSequence_<0>
        {
            using type = IndexSequence<>;
        };
    }

    template<int N>
    using MakeIndexSequence = typename MakeIndexSequence_<N>::type;
    // --------------- 构建索引序列 end ----------------


}
}