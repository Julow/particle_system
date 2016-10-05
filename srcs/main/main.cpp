// ************************************************************************** //
//                                                                            //
//                                                        :::      ::::::::   //
//   main.cpp                                           :+:      :+:    :+:   //
//                                                    +:+ +:+         +:+     //
//   By: jaguillo <jaguillo@student.42.fr>          +#+  +:+       +#+        //
//                                                +#+#+#+#+#+   +#+           //
//   Created: 2016/10/04 13:50:05 by jaguillo          #+#    #+#             //
//   Updated: 2016/10/05 19:27:35 by jaguillo         ###   ########.fr       //
//                                                                            //
// ************************************************************************** //

#include "ft/cl.h"
#include "ft/gl.h"

#include "ClContextProxy.hpp"
#include "GlfwWindowProxy.hpp"
#include "f.hpp"

#include <functional>
#include <iostream>
#include <map>
#include <stdexcept>
#include <string>

__attribute__ ((noreturn))
static void		cl_error(cl_int err, char const *str)
{
	throw std::runtime_error("%: %"_f(str, ClContextProxy::strerror(err)));
}

/*
** ========================================================================== **
** Get program
*/

static uint32_t	get_log_size(cl_program program, cl_device_id device)
{
	size_t			size;

	clGetProgramBuildInfo(program, device, CL_PROGRAM_BUILD_LOG, 0, NULL, &size);
	return (size);
}

__attribute__ ((noreturn))
static void		program_build_error(cl_program program, cl_device_id device)
{
	uint32_t const	log_size = get_log_size(program, device);
	char			log[log_size];

	clGetProgramBuildInfo(program, device, CL_PROGRAM_BUILD_LOG,
			log_size, log, NULL);
	clReleaseProgram(program);
	throw std::runtime_error("Failed to build program: %"_f
					(std::string(log, log_size)));
}

cl_program		get_program(cl_context context, char const *str)
{
	cl_program		program;
	cl_int			err;
	cl_device_id	device;

	if ((err = clGetContextInfo(context, CL_CONTEXT_DEVICES,
			sizeof(device), &device, NULL)) != CL_SUCCESS)
		cl_error(err, "clGetContextInfo");
	program = clCreateProgramWithSource(context, 1, &str, NULL, &err);
	cl_error(err, "clCreateProgramWithSource");
	err = clBuildProgram(program, 1, &device, "", NULL, NULL);
	if (err == CL_BUILD_PROGRAM_FAILURE)
		program_build_error(program, device);
	else if (err != CL_SUCCESS)
	{
		clReleaseProgram(program);
		cl_error(err, "clBuildProgram");
	}
	return (program);
}

cl_kernel		get_kernel(cl_program program, char const *kernel_name)
{
	cl_int			err;
	cl_kernel		kernel;

	kernel = clCreateKernel(program, kernel_name, &err);
	cl_error(err, "clCreateKernel");
	return (kernel);
}

cl_mem			get_buffer(cl_context context, cl_mem_flags flags, uint32_t size)
{
	cl_int			err;
	cl_mem			buff;

	buff = clCreateBuffer(context, flags, size, NULL, &err);
	cl_error(err, "clCreateBuffer");
	return (buff);
}

/*
** ========================================================================== **
** Kernel run
*/

template<class T>
void			kernel_arg(cl_kernel kernel, uint32_t index, T arg)
{
	cl_int			err;

	if ((err = clSetKernelArg(kernel, index, sizeof(T), &arg)) != CL_SUCCESS)
		cl_error(err, "clSetKernelArg");
}

template<class ...Args>
void			kernel_args(cl_kernel kernel, Args&& ...args)
{
	uint32_t		i;

	i = 0;
	(void)(int[]){(kernel_arg(kernel, i++, args), 0) ...};
}

class	kernel_run_work
{
public:
	kernel_run_work(size_t global_work_size,
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

template<class ...Args>
void			kernel_run(cl_command_queue queue, cl_kernel kernel,
					kernel_run_work const &work, Args&& ...args)
{
	cl_int			err;

	kernel_args(kernel, std::forward<Args>(args)...);
	if ((err = clEnqueueNDRangeKernel(queue, kernel, 1, work.g_offset(),
			work.g_size(), work.l_size(), 0, NULL, NULL)) != CL_SUCCESS)
		cl_error(err, "clEnqueueNDRangeKernel");
}

/*
** ========================================================================== **
*/

class	Main final : GlfwWindowProxy, ClContextProxy
{
public:
	Main() :
		GlfwWindowProxy(500, 500, "lol"),
		ClContextProxy()
	{}
	virtual ~Main() {}

	void			loop()
	{
		while (!glfwWindowShouldClose(get_window()))
			glfwPollEvents();
	}

private:

private:
	Main(Main &&src) = delete;
	Main(Main const &src) = delete;
	Main			&operator=(Main &&rhs) = delete;
	Main			&operator=(Main const &rhs) = delete;
};

int				main()
{
	Main			*m;

	try
	{
		m = new Main();
	}
	catch (std::runtime_error const &e)
	{
		std::cout << "Init error: " << e.what() << std::endl;
		return (1);
	}
	m->loop();
	return (0);
}
