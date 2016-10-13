// ************************************************************************** //
//                                                                            //
//                                                        :::      ::::::::   //
//   ClGlBuffer.hpp                                     :+:      :+:    :+:   //
//                                                    +:+ +:+         +:+     //
//   By: jaguillo <jaguillo@student.42.fr>          +#+  +:+       +#+        //
//                                                +#+#+#+#+#+   +#+           //
//   Created: 2016/10/08 17:53:08 by jaguillo          #+#    #+#             //
//   Updated: 2016/10/13 19:55:53 by jaguillo         ###   ########.fr       //
//                                                                            //
// ************************************************************************** //

#ifndef CLGLBUFFER_HPP
# define CLGLBUFFER_HPP

# include "ft/cl.h"
# include "GlBuffer.hpp"

/*
** Shared buffer between OpenGL and OpenCL
*/
template<typename T, typename ...ATTRIBS>
class	ClGlBuffer : GlBuffer<T, ATTRIBS...>, ClBuffer<T>
{
public:
	class	acquired;

	ClGlBuffer(cl_context c, size_t size, T const *data = nullptr);

	virtual ~ClGlBuffer();

	GLuint			get_gl_handle();
	cl_mem			get_cl_handle();

private:
	cl_mem			_handle;

	static cl_mem	_get_cl_buffer(cl_context c, GLuint gl_handle);

private:
	ClGlBuffer() = delete;
	ClGlBuffer(ClGlBuffer &&src) = delete;
	ClGlBuffer(ClGlBuffer const &src) = delete;
	ClGlBuffer		&operator=(ClGlBuffer &&rhs) = delete;
	ClGlBuffer		&operator=(ClGlBuffer const &rhs) = delete;
};

/*
** Lock-like object to use the buffer with OpenCL
** cl_acquire(queue, buffers...)
*/
template<typename ...T>
class	cl_acquired : public std::tuple<ClBuffer<T>&...>
{
public:
	cl_acquired(cl_command_queue queue, ClBuffer<T>& ...buffs);

	cl_acquired(cl_acquired &&src);

	virtual ~cl_acquired();

private:
	constexpr static unsigned const	BUFF_COUNT = sizeof...(T);

	cl_command_queue	_queue;

	template<size_t ...INDEXES>
	void			_get_handles(cl_mem *dst, std::index_sequence<INDEXES...>);

private:
	cl_acquired() = delete;
	cl_acquired(cl_acquired const &src) = delete;
	cl_acquired			&operator=(cl_acquired &&rhs) = delete;
	cl_acquired			&operator=(cl_acquired const &rhs) = delete;
};

template<typename ...BUFFERS>
auto				cl_acquire(cl_command_queue queue, BUFFERS& ...buffs)
	-> decltype(_cl_acquire(queue, _cl_acquire_cast(buffs)...));

# include "ClGlBuffer.tpp"

#endif
