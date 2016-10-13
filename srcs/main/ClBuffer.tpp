// ************************************************************************** //
//                                                                            //
//                                                        :::      ::::::::   //
//   ClBuffer.tpp                                       :+:      :+:    :+:   //
//                                                    +:+ +:+         +:+     //
//   By: jaguillo <jaguillo@student.42.fr>          +#+  +:+       +#+        //
//                                                +#+#+#+#+#+   +#+           //
//   Created: 2016/10/13 12:05:05 by jaguillo          #+#    #+#             //
//   Updated: 2016/10/13 13:48:50 by jaguillo         ###   ########.fr       //
//                                                                            //
// ************************************************************************** //

#ifndef CLBUFFER_TPP
# define CLBUFFER_TPP

template<typename T>
ClBuffer<T>::ClBuffer(cl_context context, size_t size, bool read_only)
{
	cl_int			err;

	_handle = clCreateBuffer(context,
			read_only ? CL_MEM_READ_ONLY : CL_MEM_READ_WRITE,
			size * sizeof(T), nullptr, &err);
	if (_handle == nullptr)
		throw std::runtime_error("clCreateBuffer: %"_f
						(ClContextProxy::strerror(err)));
}

template<typename T>
ClBuffer<T>::ClBuffer(cl_mem handle) :
	_handle(handle)
{}

template<typename T>
ClBuffer<T>::ClBuffer(ClBuffer &&src) :
	_handle(src._handle)
{}

template<typename T>
ClBuffer<T>::~ClBuffer()
{
	clReleaseMemObject(_handle);
}

template<typename T>
cl_mem		ClBuffer<T>::get_handle()
{
	return (_handle);
}


#endif
