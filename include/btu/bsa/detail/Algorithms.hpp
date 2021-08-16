/* Copyright (C) 2021 G'k
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/. */
#pragma once

#include <algorithm>
#include <cassert>
#include <cmath>
#include <filesystem>
#include <iterator>
#include <string>
#include <string_view>
#include <type_traits>

namespace btu::bsa {
//Expects a range sorted in descending order
template<class It, class Predicate, class Sum>
[[nodiscard]] inline It merge_if(It first, It last, Predicate const &predicate, Sum const &sum)
{
    if (first == last)
        return last;

    last--;
    while (first != last)
    {
        while (predicate(*first, *last))
        {
            *first = sum(*first, *last);
            last--;
            if (first == last)
                return ++first;
        }
        first++;
    }
    return ++first;
}

template<class It, class Predicate>
[[nodiscard]] inline It merge_if(It first, It last, Predicate &&predicate)
{
    using Type = std::remove_cv_t<std::remove_reference_t<decltype(*first)>>;
    auto plus  = [](Type const &first, Type const &second) { return first + second; };
    return merge_if(first, last, std::forward<Predicate>(predicate), plus);
}

template<class Container, class Predicate>
[[nodiscard]] inline auto merge_if(Container &cont, Predicate &&predicate)
{
    using namespace std;
    return merge_if(begin(cont), end(cont), std::forward<Predicate>(predicate));
}

template<class Container, class Predicate, class Sum>
[[nodiscard]] inline auto merge_if(Container &cont, Predicate &&pred, Sum &&sum)
{
    using namespace std;
    return merge_if(begin(cont), end(cont), std::forward<Predicate>(pred), std::forward<Sum>(sum));
}

template<class Container, class ValueType>
[[nodiscard]] inline auto contains(Container const &cont, ValueType const &val)
{
    using namespace std;
    return find(begin(cont), end(cont), val) != end(cont);
}

template<class Container, class Predicate>
[[nodiscard]] inline auto find_if(Container const &cont, Predicate const &pred)
{
    using namespace std;
    return find_if(begin(cont), end(cont), pred);
}

template<typename Cont, typename Pred>
void erase_if(Cont &cont, Pred const &pred)
{
    using namespace std;
    cont.erase(remove_if(begin(cont), end(cont), pred), end(cont));
}

template<typename CharT>
using StrComparePred = bool (*)(CharT, CharT);

template<typename CharT>
StrComparePred<CharT> str_compare_pred(bool caseSensitive = true)
{
    if (caseSensitive)
        return [](CharT ch1, CharT ch2) { return ch1 == ch2; };

    return [](CharT ch1, CharT ch2) {
        ch1 = std::toupper(ch1);
        ch2 = std::toupper(ch2);

        return ch1 == ch2;
    };
}

template<class CharT>
bool str_compare(std::basic_string_view<CharT> string,
                 std::basic_string_view<CharT> other,
                 bool caseSensitive = true)
{
    auto pred = str_compare_pred<CharT>(caseSensitive);

    using namespace std;
    return string.size() == other.size() && std::equal(cbegin(other), cend(other), cbegin(string), pred);
}

template<class CharT>
std::basic_string<CharT> to_lower(std::basic_string_view<CharT> str)
{
    std::basic_string<CharT> res;
    res.reserve(str.size());
    std::transform(str.begin(), str.end(), std::back_inserter(res), [](auto &&c) { return ::tolower(c); });
    return res;
}

inline std::filesystem::path to_lower(std::filesystem::path const &path)
{
    return to_lower<std::filesystem::path::string_type::value_type>(path.native());
}

} // namespace BethUtil::BSA
