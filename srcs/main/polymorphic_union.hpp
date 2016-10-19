// ************************************************************************** //
//                                                                            //
//                                                        :::      ::::::::   //
//   polymorphic_union.hpp                              :+:      :+:    :+:   //
//                                                    +:+ +:+         +:+     //
//   By: jaguillo <jaguillo@student.42.fr>          +#+  +:+       +#+        //
//                                                +#+#+#+#+#+   +#+           //
//   Created: 2016/10/17 18:38:59 by jaguillo          #+#    #+#             //
//   Updated: 2016/10/18 22:06:25 by juloo            ###   ########.fr       //
//                                                                            //
// ************************************************************************** //

#ifndef POLYMORPHIC_UNION_HPP
# define POLYMORPHIC_UNION_HPP

# include <algorithm>

namespace std // TODO: move
{

/*
** Constexpr version of std::all_of
*/
static inline constexpr
bool			all_of(std::initializer_list<bool> lst)
{
	for (bool v : lst) if (!v) return (false);
	return (true);
}

};

/*
** Union where all members inherit from the same BASE class
*/
template<typename BASE, typename ...TYPES>
class	polymorphic_union final
{
public:
	template<typename T>
	polymorphic_union(T const &val);
	template<typename T>
	polymorphic_union(T &&val);

	polymorphic_union(polymorphic_union const &src);
	~polymorphic_union();

	BASE			&operator*();
	BASE const		&operator*() const;
	BASE			*operator->();
	BASE const		*operator->() const;

private:

	static constexpr unsigned const	SIZE = std::max({sizeof(TYPES)...});
	static constexpr unsigned const	ALIGN = std::max({alignof(TYPES)...});

	typedef std::tuple<TYPES...>								types;

	typedef typename std::aligned_storage<SIZE, ALIGN>::type	storage;

	unsigned		_index;
	storage			_data;

	BASE			&_ref;

	static_assert(std::has_virtual_destructor<BASE>::value,
			"BASE type does not have a virtual destructor");
	static_assert(std::all_of({ std::is_base_of<BASE, TYPES>::value... }),
			"All member must inherit BASE class");

private:
	polymorphic_union() = delete;
	polymorphic_union(polymorphic_union &&src) = delete;
	polymorphic_union	&operator=(polymorphic_union &&rhs) = delete;
	polymorphic_union	&operator=(polymorphic_union const &rhs) = delete;
};

# include "polymorphic_union.tpp"

#endif
