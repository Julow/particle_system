// ************************************************************************** //
//                                                                            //
//                                                        :::      ::::::::   //
//   ClKernel.hpp                                       :+:      :+:    :+:   //
//                                                    +:+ +:+         +:+     //
//   By: jaguillo <jaguillo@student.42.fr>          +#+  +:+       +#+        //
//                                                +#+#+#+#+#+   +#+           //
//   Created: 2016/10/06 16:56:28 by jaguillo          #+#    #+#             //
//   Updated: 2016/10/07 14:47:43 by jaguillo         ###   ########.fr       //
//                                                                            //
// ************************************************************************** //

#ifndef CLKERNEL_HPP
# define CLKERNEL_HPP

# include "ft/cl.h"

# include <array>
# include <tuple>

// -
# include "f.hpp"
# include <experimental/optional>

namespace std { using namespace experimental; };

template<typename T, size_t N>
struct	array_or_int
{
	typedef std::array<T, N>	type;

	static T const		*get_data(type const &array)
	{ return (array.data()); }
};

template<typename T>
struct	array_or_int<T, 1>
{
	typedef T					type;

	static T const		*get_data(type const &v)
	{ return (&v); }
};
// -

template<typename ...ARGS>
class	ClKernel final
{
public:
	template<size_t D>
	class	work;

	ClKernel(cl_program prog, char const *kernel_name);
	~ClKernel();

	cl_kernel		get_kernel();

	/*
	** Set kernel's INDEX-th argument
	*/
	template<size_t INDEX, typename T = typename
			std::tuple_element<INDEX, std::tuple<ARGS...>>::type>
	void			set_arg(T &&arg);

	/*
	** Set kernel arguments
	*/
	void			set_args(ARGS&& ...args);

	/*
	** Construct a work of dimension D
	*/
	template<size_t D>
	work<D>			make_work(typename work<D>::array_t global_work_size,
						std::optional<typename work<D>::array_t>
								local_work_size = std::nullopt,
						std::optional<typename work<D>::array_t>
								global_work_offset = std::nullopt);

private:
	cl_kernel		_kernel;

	template<size_t INDEX>
	void			_set_args() {}

	template<size_t INDEX, typename HEAD, typename ...TAIL>
	void			_set_args(HEAD &&arg, TAIL&& ...tail);

	template<typename T>
	cl_int			_set_arg(unsigned index, T&& arg);

private:
	ClKernel() = delete;
	ClKernel(ClKernel &&k) = delete;
	ClKernel(ClKernel const &src) = delete;
	ClKernel			&operator=(ClKernel &&rhs) = delete;
	ClKernel			&operator=(ClKernel const &rhs) = delete;
};

/*
** Keep reference to ClKernel object
*/
template<typename ...ARGS>
template<size_t D>
class	ClKernel<ARGS...>::work final
{
public:
	using _array_t = array_or_int<size_t, D>;
	typedef typename array_or_int<size_t, D>::type	array_t;

	work(ClKernel<ARGS...> &k, array_t &&global_work_size,
			std::optional<array_t> &&local_work_size = std::nullopt,
			std::optional<array_t> &&global_work_offset = std::nullopt);
	work(work &&k);
	~work();

	/*
	** Enqueue with arguments 'args'
	*/
	void			operator()(cl_command_queue queue, ARGS ...args);

private:
	ClKernel<ARGS...>			&_kernel;

	array_t						_global_size;
	std::optional<array_t>		_local_size;
	std::optional<array_t>		_global_offset;

private:
	work() = delete;
	work(work const &src) = delete;
	work			&operator=(work &&rhs) = delete;
	work			&operator=(work const &rhs) = delete;
};

# include "ClKernel.tpp"

#endif
