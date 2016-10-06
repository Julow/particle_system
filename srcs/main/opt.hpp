// ************************************************************************** //
//                                                                            //
//                                                        :::      ::::::::   //
//   opt.hpp                                            :+:      :+:    :+:   //
//                                                    +:+ +:+         +:+     //
//   By: juloo <juloo@student.42.fr>                +#+  +:+       +#+        //
//                                                +#+#+#+#+#+   +#+           //
//   Created: 2016/10/07 00:21:51 by juloo             #+#    #+#             //
//   Updated: 2016/10/07 00:23:10 by juloo            ###   ########.fr       //
//                                                                            //
// ************************************************************************** //

#ifndef OPT_HPP
# define OPT_HPP

# include <optional>
# include <type_traits>

template<typename T, typename T2 = void>
struct	is_optional : std::false_type {};

template<typename T>
struct	is_optional<T, typename std::enable_if<
			std::is_same<T, std::optional<typename T::value_type>>::value
		>::type> : std::true_type {};

template<typename T, typename T2 = void>
struct	optional_of
{
	typedef std::optional<T>	type;
};

template<typename T>
struct	optional_of<T, typename std::enable_if<is_optional<T>::value>::type>
{
	typedef T					type;
};

template<typename A, typename F, typename B = typename
			optional_of<decltype((*(F*)nullptr)(*(A*)nullptr))>::type>
B				operator->*(std::optional<A> &v, F f)
{
	return (v ? f(*v) : B(std::nullopt));
}

template<typename A, typename F, typename B = typename
			optional_of<decltype((*(F*)nullptr)(*(A*)nullptr))>::type>
B				operator->*(std::optional<A> &&v, F f)
{
	return (v ? f(*v) : B(std::nullopt));
}

#endif
