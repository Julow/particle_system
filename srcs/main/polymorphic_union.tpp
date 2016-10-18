// ************************************************************************** //
//                                                                            //
//                                                        :::      ::::::::   //
//   polymorphic_union.tpp                              :+:      :+:    :+:   //
//                                                    +:+ +:+         +:+     //
//   By: jaguillo <jaguillo@student.42.fr>          +#+  +:+       +#+        //
//                                                +#+#+#+#+#+   +#+           //
//   Created: 2016/10/17 19:44:20 by jaguillo          #+#    #+#             //
//   Updated: 2016/10/18 12:41:08 by jaguillo         ###   ########.fr       //
//                                                                            //
// ************************************************************************** //

#ifndef POLYMORPHIC_UNION_TPP
# define POLYMORPHIC_UNION_TPP

# include <stdexcept>

namespace _polymorphic_union_impl
{

template<size_t INDEX, typename BASE, typename TUPLE>
struct	op
{
	typedef std::tuple_element_t<INDEX-1, TUPLE>	t;

	/*
	** Call copy constructor of the INDEX-th type and return a reference to it
	*/
	static BASE	&copy(void *dst, void const *src, size_t index)
	{
		if ((INDEX-1) == index)
		{
			new(dst) t(*static_cast<t const*>(src));
			return (*static_cast<BASE*>(reinterpret_cast<t*>(dst)));
		}
		else
			return (op<INDEX-1, BASE, TUPLE>::copy(dst, src, index));
	}
};

template<typename BASE, typename TUPLE>
struct	op<0, BASE, TUPLE>
{
	static BASE	&copy(void *, void const *, size_t)
	{
		throw std::domain_error("Invalid type");
	}
};

template<size_t VALUE>
struct	_tuple_index
{
	static constexpr size_t const	value = VALUE;
};

template <typename T, typename TUPLE>
struct	tuple_index {};

template <typename T, typename ...TAIL>
struct	tuple_index<T, std::tuple<T, TAIL...>> : _tuple_index<0> {};

template <typename T, typename HEAD, typename ...TAIL>
struct	tuple_index<T, std::tuple<HEAD, TAIL...>> :
		_tuple_index<tuple_index<T, std::tuple<TAIL...>>::value + 1> {};

};

template<typename BASE, typename ...TYPES>
template<typename T>
polymorphic_union<BASE, TYPES...>::polymorphic_union(T const &val) :
	_index(_polymorphic_union_impl::tuple_index<T, std::tuple<TYPES...>>::value),
	_ref(*static_cast<BASE*>(reinterpret_cast<T*>(&_data)))
{
	new(&_data) T(val);
}

template<typename BASE, typename ...TYPES>
template<typename T>
polymorphic_union<BASE, TYPES...>::polymorphic_union(T &&val) :
	_index(_polymorphic_union_impl::tuple_index<T, std::tuple<TYPES...>>::value),
	_ref(*static_cast<BASE*>(reinterpret_cast<T*>(&_data)))
{
	new(&_data) T(std::forward<T>(val));
}

template<typename BASE, typename ...TYPES>
polymorphic_union<BASE, TYPES...>::polymorphic_union(
				polymorphic_union<BASE, TYPES...> const &src) :
	_index(src._index),
	_ref(_polymorphic_union_impl::op<sizeof...(TYPES), BASE,
			std::tuple<TYPES...>>::copy(&_data, &src._data, _index))
{}

template<typename BASE, typename ...TYPES>
polymorphic_union<BASE, TYPES...>::~polymorphic_union()
{
	BASE &r = **this;
	r.~BASE();
}

template<typename BASE, typename ...TYPES>
BASE		&polymorphic_union<BASE, TYPES...>::operator*()
	{ return (_ref); }

template<typename BASE, typename ...TYPES>
BASE const	&polymorphic_union<BASE, TYPES...>::operator*() const
	{ return (_ref); }

template<typename BASE, typename ...TYPES>
BASE		*polymorphic_union<BASE, TYPES...>::operator->()
	{ return (&_ref); }

template<typename BASE, typename ...TYPES>
BASE const	*polymorphic_union<BASE, TYPES...>::operator->() const
	{ return (&_ref); }

#endif
