// ************************************************************************** //
//                                                                            //
//                                                        :::      ::::::::   //
//   ClGlBuffer.tpp                                     :+:      :+:    :+:   //
//                                                    +:+ +:+         +:+     //
//   By: jaguillo <jaguillo@student.42.fr>          +#+  +:+       +#+        //
//                                                +#+#+#+#+#+   +#+           //
//   Created: 2016/10/08 17:53:08 by jaguillo          #+#    #+#             //
//   Updated: 2016/10/26 18:50:38 by juloo            ###   ########.fr       //
//                                                                            //
// ************************************************************************** //

#ifndef CLGLBUFFER_TPP
# define CLGLBUFFER_TPP

# ifndef __APPLE__
#  include <CL/cl_gl.h>

# endif

template<typename T, typename ...ATT>
ClGlBuffer<T, ATT...>::ClGlBuffer(cl_context c, size_t size, T const *data) :
	GlBuffer<T, ATT...>(size, data),
	ClBuffer<T>(_get_cl_buffer(c, get_gl_handle()))
{}

template<typename T, typename ...ATT>
ClGlBuffer<T, ATT...>::~ClGlBuffer()
{}

template<typename T, typename ...ATT>
GLuint			ClGlBuffer<T, ATT...>::get_gl_handle()
{
	return (GlBuffer<T, ATT...>::get_handle());
}

template<typename T, typename ...ATT>
cl_mem			ClGlBuffer<T, ATT...>::get_cl_handle()
{
	return (ClBuffer<T>::get_handle());
}

template<typename T, typename ...ATT>
void			ClGlBuffer<T, ATT...>::gl_bind()
{
	return (GlBuffer<T, ATT...>::bind());
}

template<typename T, typename ...ATT>
cl_mem			ClGlBuffer<T, ATT...>::_get_cl_buffer(cl_context c,
					GLuint gl_handle)
{
	cl_int			err;
	cl_mem			buff;

	buff = clCreateFromGLBuffer(c, CL_MEM_READ_WRITE, gl_handle, &err);
	if (buff == NULL)
		throw std::runtime_error("clCreateFromGLBuffer: %"_f
						(ClContextProxy::strerror(err)));
	return (buff);
}

template<typename ...T>
cl_acquired<T...>::cl_acquired(cl_command_queue queue, ClBuffer<T>& ...buffs) :
	std::tuple<ClBuffer<T>&...>(buffs...),
	_queue(queue)
{
	cl_mem		handles[BUFF_COUNT];

	_get_handles(handles, std::make_index_sequence<sizeof...(T)>());
	clEnqueueAcquireGLObjects(_queue, BUFF_COUNT, handles, 0, NULL, NULL);
}

template<typename ...T>
cl_acquired<T...>::cl_acquired(cl_acquired<T...> &&src) :
	std::tuple<ClBuffer<T>&...>(std::forward<cl_acquired>(src)),
	_queue(src._queue)
{}

template<typename ...T>
cl_acquired<T...>::~cl_acquired()
{
	cl_mem		handles[BUFF_COUNT];

	_get_handles(handles, std::make_index_sequence<sizeof...(T)>());
	clEnqueueReleaseGLObjects(_queue, BUFF_COUNT, handles, 0, NULL, NULL);
}

template<typename ...T>
template<size_t ...INDEXES>
void				cl_acquired<T...>::_get_handles(cl_mem *dst,
						std::index_sequence<INDEXES...>)
{
	(void)(int[]){((
			dst[INDEXES] = std::get<INDEXES>(*this).get_handle()
		), 0)...};
}

template<typename T, typename ...ATTRS>
ClBuffer<T>			&_cl_acquire_cast(ClGlBuffer<T, ATTRS...> &buff)
{
	return ((ClBuffer<T>&)(buff)); // TODO: lol
}

template<typename ...T>
cl_acquired<T...>	_cl_acquire(cl_command_queue queue, ClBuffer<T>& ...buffs)
{
	return (cl_acquired<T...>(queue, buffs...));
}

template<typename ...BUFFERS>
auto				cl_acquire(cl_command_queue queue, BUFFERS& ...buffs)
	-> decltype(_cl_acquire(queue, _cl_acquire_cast(buffs)...))
{
	return (_cl_acquire(queue, _cl_acquire_cast(buffs)...));
}

#endif
