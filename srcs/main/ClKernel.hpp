// ************************************************************************** //
//                                                                            //
//                                                        :::      ::::::::   //
//   ClKernel.hpp                                       :+:      :+:    :+:   //
//                                                    +:+ +:+         +:+     //
//   By: jaguillo <jaguillo@student.42.fr>          +#+  +:+       +#+        //
//                                                +#+#+#+#+#+   +#+           //
//   Created: 2016/10/06 16:56:28 by jaguillo          #+#    #+#             //
//   Updated: 2016/10/06 17:19:12 by jaguillo         ###   ########.fr       //
//                                                                            //
// ************************************************************************** //

#ifndef CLKERNEL_HPP
# define CLKERNEL_HPP

# include "ft/cl.h"
# include <tuple>

template<typename ...ARGS>
class	ClKernel
{
public:
	class	work_size;

	ClKernel(cl_program prog, char const *kernel_name);

	virtual ~ClKernel();

	cl_kernel		get_kernel();

	/*
	** Set kernel's INDEX-th argument
	*/
	template<size_t INDEX,
		typename T = typename std::tuple_element<INDEX, std::tuple<ARGS...>>::type>
	void			set_arg(T &&arg);

	/*
	** Set kernel arguments
	*/
	void			set_args(ARGS&& ...args);

	/*
	** Call the kernel with previously set arguments
	*/
	void			operator()(cl_command_queue queue, work_size const &work);

	/*
	** Set kernel arguments (args) and call the kernel
	*/
	void			operator()(cl_command_queue queue,
						work_size const &work, ARGS ...args);

private:
	cl_kernel		_kernel;

	template<size_t INDEX>
	void			_set_args() {}

	template<size_t INDEX, typename HEAD, typename ...TAIL>
	void			_set_args(HEAD &&arg, TAIL&& ...tail);

	template<typename T>
	cl_int			_set_arg(unsigned index, T&& arg);

private:
	ClKernel();
	ClKernel(ClKernel &&k) = delete;
	ClKernel(ClKernel const &src) = delete;
	ClKernel			&operator=(ClKernel &&rhs) = delete;
	ClKernel			&operator=(ClKernel const &rhs) = delete;
};

# include "ClKernel.tpp"

#endif
