#ifndef CPPA_TUPLE_HPP
#define CPPA_TUPLE_HPP

#include <cstddef>
#include <string>
#include <typeinfo>
#include <type_traits>

#include "cppa/get.hpp"
#include "cppa/actor.hpp"
#include "cppa/cow_ptr.hpp"
#include "cppa/ref_counted.hpp"

#include "cppa/util/at.hpp"
#include "cppa/util/replace_type.hpp"
#include "cppa/util/is_comparable.hpp"
#include "cppa/util/compare_tuples.hpp"
#include "cppa/util/eval_type_list.hpp"
#include "cppa/util/type_list_apply.hpp"
#include "cppa/util/is_legal_tuple_type.hpp"

#include "cppa/detail/tuple_vals.hpp"
#include "cppa/detail/implicit_conversions.hpp"

namespace cppa {

// forward declaration
class any_tuple;
class local_actor;

/**
 * @brief Describes a fixed-length tuple.
 */
template<typename... ElementTypes>
class tuple
{

    //friend class any_tuple;

    template<size_t N, typename... Types>
    friend typename util::at<N, Types...>::type& get_ref(tuple<Types...>&);


 public:

    typedef util::type_list<ElementTypes...> element_types;

 private:

    static_assert(sizeof...(ElementTypes) > 0, "tuple is empty");

    static_assert(util::eval_type_list<element_types,
                                       util::is_legal_tuple_type>::value,
                  "illegal types in tuple definition: "
                  "pointers and references are prohibited");

    typedef detail::tuple_vals<ElementTypes...> vals_t;

    cow_ptr<vals_t> m_vals;

    static bool compare_vals(const detail::tdata<>& v0,
                             const detail::tdata<>& v1)
    {
        return true;
    }

    template<typename Vals0, typename Vals1>
    static bool compare_vals(const Vals0& v0, const Vals1& v1)
    {
        typedef typename Vals0::head_type lhs_type;
        typedef typename Vals1::head_type rhs_type;
        static_assert(util::is_comparable<lhs_type, rhs_type>::value,
                      "Types are not comparable");
        return v0.head == v1.head && compare_vals(v0.tail(), v1.tail());
    }

 public:

    // enable use of tuple as type_list
    typedef typename element_types::head_type head_type;
    typedef typename element_types::tail_type tail_type;

    tuple() : m_vals(new vals_t)
    {
    }

    tuple(const ElementTypes&... args) : m_vals(new vals_t(args...))
    {
    }

    size_t size() const
    {
        return m_vals->size();
    }

    const void* at(size_t p) const
    {
        return m_vals->at(p);
    }

    const uniform_type_info* utype_at(size_t p) const
    {
        return m_vals->utype_info_at(p);
    }

    const cow_ptr<vals_t>& vals() const
    {
        return m_vals;
    }

    template<typename... Args>
    bool equal_to(const tuple<Args...>& other) const
    {
        static_assert(sizeof...(ElementTypes) == sizeof...(Args),
                      "Can't compare tuples of different size");
        return compare_vals(vals()->data(), other.vals()->data());
    }

};

template<size_t N, typename... Types>
const typename util::at<N, Types...>::type&
get(const tuple<Types...>& t)
{
    return get<N>(t.vals()->data());
}

template<size_t N, typename... Types>
typename util::at<N, Types...>::type&
get_ref(tuple<Types...>& t)
{
    return get_ref<N>(t.m_vals->data_ref());
}

template<typename TypeList>
struct tuple_type_from_type_list;

template<typename... Types>
struct tuple_type_from_type_list<util::type_list<Types...>>
{
    typedef tuple<Types...> type;
};

template<typename... Types>
typename tuple_type_from_type_list<
    typename util::type_list_apply<util::type_list<Types...>,
                                   detail::implicit_conversions>::type>::type
make_tuple(const Types&... args)
{
    return { args... };
}

template<typename... LhsTypes, typename... RhsTypes>
inline bool operator==(const tuple<LhsTypes...>& lhs,
                       const tuple<RhsTypes...>& rhs)
{
    return util::compare_tuples(lhs, rhs);
}

template<typename... LhsTypes, typename... RhsTypes>
inline bool operator!=(const tuple<LhsTypes...>& lhs,
                       const tuple<RhsTypes...>& rhs)
{
    return !(lhs == rhs);
}

} // namespace cppa

#endif
