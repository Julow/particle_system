// ************************************************************************** //
//                                                                            //
//                                                        :::      ::::::::   //
//   main.cpp                                           :+:      :+:    :+:   //
//                                                    +:+ +:+         +:+     //
//   By: jaguillo <jaguillo@student.42.fr>          +#+  +:+       +#+        //
//                                                +#+#+#+#+#+   +#+           //
//   Created: 2016/10/04 13:50:05 by jaguillo          #+#    #+#             //
//   Updated: 2016/10/07 15:08:40 by jaguillo         ###   ########.fr       //
//                                                                            //
// ************************************************************************** //

#include "ft/cl.h"
#include "ft/gl.h"

#include "ClContextProxy.hpp"
#include "ClKernel.hpp"
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
	if (err != CL_SUCCESS)
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

/*
** ========================================================================== **
*/

cl_mem			get_buffer(cl_context context, cl_mem_flags flags, uint32_t size)
{
	cl_int			err;
	cl_mem			buff;

	buff = clCreateBuffer(context, flags, size, NULL, &err);
	if (err != CL_SUCCESS)
		cl_error(err, "clCreateBuffer");
	return (buff);
}

/*
** ========================================================================== **
*/

#define TEST_SIZE		10000

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

		// { // TEST
		// 	std::cout << "begin test" << std::endl;

		// 	cl_program const	test_prog = get_program(get_context(),
		// 			"__kernel void		test_init(__global float *buff, uint v)"
		// 			"{"
		// 			"	buff[get_global_id(0)] = v;"
		// 			"}"
		// 			""
		// 			"__kernel void		test_mv(__global float *dst,"
		// 			"						__global float const *src)"
		// 			"{"
		// 			"	unsigned id = get_global_id(0);"
		// 			"	dst[id] = src[id];"
		// 			"}"
		// 		);

		// 	ClKernel<cl_mem, cl_uint>	test_init(test_prog, "test_init");
		// 	ClKernel<cl_mem, cl_mem>	test_mv(test_prog, "test_mv");

		// 	cl_mem				buff_a = get_buffer(get_context(), CL_MEM_READ_WRITE, sizeof(float)*TEST_SIZE);
		// 	cl_mem				buff_b = get_buffer(get_context(), CL_MEM_READ_WRITE, sizeof(float)*TEST_SIZE);

		// 	test_init.make_work<1>(TEST_SIZE)(get_queue(), buff_a, 42);
		// 	test_init.make_work<1>(TEST_SIZE)(get_queue(), buff_b, 12);

		// 	test_mv.make_work<1>(TEST_SIZE)(get_queue(), buff_b, buff_a);

		// 	{ // TMP PRINT
		// 		cl_mem const		tmp_buff = buff_b;

		// 		float				tmp[TEST_SIZE];
		// 		cl_int				err;
		// 		err = clEnqueueReadBuffer(get_queue(), tmp_buff, true, 0, sizeof(tmp), &tmp, 0, NULL, NULL);
		// 		if (err != CL_SUCCESS) cl_error(err, "clEnqueueReadBuffer (TEST)");
		// 		for (float f : tmp) std::cout << f << " ";
		// 		std::cout << std::endl;
		// 	}

		// 	clFinish(get_queue());
		// 	std::cout << "end test" << std::endl;
		// }

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
	delete m;
	return (0);
}
