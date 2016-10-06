// ************************************************************************** //
//                                                                            //
//                                                        :::      ::::::::   //
//   ClKernel.tpp                                       :+:      :+:    :+:   //
//                                                    +:+ +:+         +:+     //
//   By: jaguillo <jaguillo@student.42.fr>          +#+  +:+       +#+        //
//                                                +#+#+#+#+#+   +#+           //
//   Created: 2016/10/06 16:57:00 by jaguillo          #+#    #+#             //
//   Updated: 2016/10/06 17:20:34 by jaguillo         ###   ########.fr       //
//                                                                            //
// ************************************************************************** //

#ifndef CLKERNEL_TPP
# define CLKERNEL_TPP

# include "f.hpp"
# include <stdexcept>

template<typename ...ARGS>
class	ClKernel<ARGS...>::work_size
{
public:
	work_size(size_t global_work_size,
				size_t local_work_size = -1,
				size_t global_work_offset = -1) :
		_g_size(global_work_size),
		_g_offset(global_work_offset),
		_l_size(local_work_size)
	{}

	size_t const	*g_size(void) const { return (&_g_size); }
	size_t const	*g_offset(void) const
		{ return ((_g_offset < 0) ? nullptr : (size_t const*)&_g_offset); }
	size_t const	*l_size(void) const
		{ return ((_l_size < 0) ? nullptr : (size_t const*)&_l_size); }

private:
	size_t		_g_size;
	ssize_t		_g_offset;
	ssize_t		_l_size;
};

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
void			ClKernel<ARGS...>::operator()(cl_command_queue queue,
					work_size const &work)
{
	cl_int			err;

	if ((err = clEnqueueNDRangeKernel(queue, _kernel, 1, work.g_offset(),
			work.g_size(), work.l_size(), 0, NULL, NULL)) != CL_SUCCESS)
		throw std::runtime_error("clEnqueueNDRangeKernel: %"_f
				(ClContextProxy::strerror(err)));
}

template<typename ...ARGS>
void			ClKernel<ARGS...>::operator()(cl_command_queue queue,
					work_size const &work, ARGS ...args)
{
	set_args(std::forward<ARGS>(args)...);
	(*this)(queue, work);
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

#endif
