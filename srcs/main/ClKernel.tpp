// ************************************************************************** //
//                                                                            //
//                                                        :::      ::::::::   //
//   ClKernel.tpp                                       :+:      :+:    :+:   //
//                                                    +:+ +:+         +:+     //
//   By: jaguillo <jaguillo@student.42.fr>          +#+  +:+       +#+        //
//                                                +#+#+#+#+#+   +#+           //
//   Created: 2016/10/06 16:57:00 by jaguillo          #+#    #+#             //
//   Updated: 2016/10/07 14:33:35 by jaguillo         ###   ########.fr       //
//                                                                            //
// ************************************************************************** //

#ifndef CLKERNEL_TPP
# define CLKERNEL_TPP

# include "f.hpp"
# include <stdexcept>

template<typename ...ARGS>
template<size_t D>
ClKernel<ARGS...>::work<D>::work(ClKernel<ARGS...> &k,
			array_t &&global_work_size,
			std::optional<array_t> &&local_work_size,
			std::optional<array_t> &&global_work_offset) :
	_kernel(k),
	_global_size(std::forward<array_t>(global_work_size)),
	_local_size(local_work_size ?
		std::make_optional(std::forward<array_t>(*local_work_size)) :
		std::nullopt),
	_global_offset(global_work_offset ?
		std::make_optional(std::forward<array_t>(*global_work_offset)) :
		std::nullopt)
{}

template<typename ...ARGS>
template<size_t D>
ClKernel<ARGS...>::work<D>::work(work &&k) :
	_kernel(k._kernel),
	_global_size(k._global_size),
	_local_size(k._local_size),
	_global_offset(k._global_offset)
{}

template<typename ...ARGS>
template<size_t D>
ClKernel<ARGS...>::work<D>::~work()
{}

template<typename ...ARGS>
template<size_t D>
void			ClKernel<ARGS...>::work<D>::operator()(cl_command_queue queue,
					ARGS ...args)
{
	cl_int			err;

	_kernel.set_args(std::forward<ARGS>(args)...);
	if ((err = clEnqueueNDRangeKernel(queue, _kernel.get_kernel(), D,
		(_global_offset ? _array_t::get_data(*_global_offset) : nullptr),
		_array_t::get_data(_global_size),
		(_local_size ? _array_t::get_data(*_local_size) : nullptr),
		0, NULL, NULL)) != CL_SUCCESS)
	throw std::runtime_error("clEnqueueNDRangeKernel: %"_f
			(ClContextProxy::strerror(err)));
}

template<typename ...ARGS>
ClKernel<ARGS...>::ClKernel(cl_program prog, char const *kernel_name)
{
	cl_int			err;

	_kernel = clCreateKernel(prog, kernel_name, &err);
	if (err != CL_SUCCESS)
		throw std::runtime_error("clCreateKernel: %"_f
				(ClContextProxy::strerror(err)));
}

template<typename ...ARGS>
ClKernel<ARGS...>::~ClKernel()
{
	clReleaseKernel(_kernel);
}

template<typename ...ARGS>
cl_kernel		ClKernel<ARGS...>::get_kernel()
{
	return (_kernel);
}

template<typename ...ARGS>
template<size_t INDEX, typename T>
void			ClKernel<ARGS...>::set_arg(T &&arg)
{
	cl_int			err;

	err = _set_arg(INDEX, std::forward<T>(arg));
	if (err != CL_SUCCESS)
		throw std::runtime_error("clSetKernelArg: %"_f
				(ClContextProxy::strerror(err)));
}

template<typename ...ARGS>
void			ClKernel<ARGS...>::set_args(ARGS&& ...args)
{
	_set_args<0>(std::forward<ARGS>(args)...);
}

template<typename ...ARGS>
template<size_t INDEX, typename HEAD, typename ...TAIL>
void			ClKernel<ARGS...>::_set_args(HEAD &&arg, TAIL&& ...tail)
{
	set_arg<INDEX>(std::forward<HEAD>(arg));
	_set_args<INDEX+1>(std::forward<TAIL>(tail)...);
}

template<typename ...ARGS>
template<typename T>
cl_int			ClKernel<ARGS...>::_set_arg(unsigned index, T&& arg)
{
	return (clSetKernelArg(_kernel, index, sizeof(arg), &arg));
}

template<typename ...ARGS>
template<size_t D>
ClKernel<ARGS...>::work<D>	ClKernel<ARGS...>::make_work(
				typename work<D>::array_t global_work_size,
				std::optional<typename work<D>::array_t> local_work_size,
				std::optional<typename work<D>::array_t> global_work_offset)
{
	return (work<D>(*this,
			std::forward<typename work<D>::array_t>(global_work_size),
			std::forward<std::optional<typename work<D>::array_t>>(local_work_size),
			std::forward<std::optional<typename work<D>::array_t>>(global_work_offset)));
}

#endif
