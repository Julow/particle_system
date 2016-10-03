// ************************************************************************** //
//                                                                            //
//                                                        :::      ::::::::   //
//   test.cpp                                           :+:      :+:    :+:   //
//                                                    +:+ +:+         +:+     //
//   By: jaguillo <jaguillo@student.42.fr>          +#+  +:+       +#+        //
//                                                +#+#+#+#+#+   +#+           //
//   Created: 2016/10/03 10:39:11 by jaguillo          #+#    #+#             //
//   Updated: 2016/10/03 16:33:59 by jaguillo         ###   ########.fr       //
//                                                                            //
// ************************************************************************** //

#include <functional>
#include <map>
#include <stdexcept>
#include <string>

#include <OpenCL/opencl.h>

/*
** ========================================================================== **
** utils
*/

static std::map<cl_int, char const*> const	g_cl_errors = {
	{CL_INVALID_PLATFORM, "CL_INVALID_PLATFORM"},
	{CL_INVALID_VALUE, "CL_INVALID_VALUE"},
	{CL_INVALID_DEVICE, "CL_INVALID_DEVICE"},
	{CL_INVALID_DEVICE_TYPE, "CL_INVALID_DEVICE_TYPE"},
	{CL_DEVICE_NOT_AVAILABLE, "CL_DEVICE_NOT_AVAILABLE"},
	{CL_DEVICE_NOT_FOUND, "CL_DEVICE_NOT_FOUND"},
	{CL_OUT_OF_HOST_MEMORY, "CL_OUT_OF_HOST_MEMORY"},
	{CL_INVALID_PROGRAM, "CL_INVALID_PROGRAM"},
	{CL_INVALID_BINARY, "CL_INVALID_BINARY"},
	{CL_INVALID_BUILD_OPTIONS, "CL_INVALID_BUILD_OPTIONS"},
	{CL_COMPILER_NOT_AVAILABLE, "CL_COMPILER_NOT_AVAILABLE"},
	{CL_BUILD_PROGRAM_FAILURE, "CL_BUILD_PROGRAM_FAILURE"},
	{CL_INVALID_OPERATION, "CL_INVALID_OPERATION"},
	{CL_OUT_OF_RESOURCES, "CL_OUT_OF_RESOURCES"},
};

#define CL_CALL(FUNC, ...)		cl_error(FUNC(__VA_ARGS__), #FUNC)

void					cl_error(cl_int err, char const *str)
{
	if (err != CL_SUCCESS)
		throw std::runtime_error(std::string(str) + ": " + g_cl_errors.at(err));
}

/*
** ========================================================================== **
** Get context
*/

typedef std::function<void(char const*, void const*, size_t)>	t_err_callback;

static void				pfn_notify(char const *err, void const *info,
							size_t cb, void *user_data)
{
	(*static_cast<t_err_callback*>(user_data))(err, info, cb);
}

static cl_platform_id	get_platform(void)
{
	cl_platform_id	p;

	CL_CALL(clGetPlatformIDs, 1, &p, NULL);
	return (p);
}

static std::pair<cl_context, cl_command_queue>
				get_context(cl_platform_id platform, t_err_callback err_callback)
{
	cl_device_id			device;
	cl_int					err;
	cl_context				context;
	cl_command_queue		queue;
	t_err_callback *const	notify_data = new t_err_callback(err_callback);

	clGetDeviceIDs(platform, CL_DEVICE_TYPE_GPU, 1, &device, NULL);
	if ((context = clCreateContext(NULL, 1, &device,
				&pfn_notify, (void*)notify_data, &err)) == NULL
		|| (queue = clCreateCommandQueue(context, device, 0, &err)) == NULL)
	{
		delete notify_data;
		cl_error(err, "clCreateContext");
	}
	return {context, queue};
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
	throw std::runtime_error(std::string("Failed to build program: ")
			+ std::string(log, log_size));
}

cl_program		get_program(cl_context context, char const *str)
{
	cl_program		program;
	cl_int			err;
	cl_device_id	device;

	CL_CALL(clGetContextInfo, context, CL_CONTEXT_DEVICES, sizeof(device), &device, NULL);
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
*/

#include <iostream>

static char const *const	g_test_srcs = {
	"__kernel void	test_init(__global uint *buff, uint init)"
	"{"
	"	buff[get_global_id(0)] = init;"
	"}"
	"__kernel void	test_inc(__global uint *buff, uint inc)"
	"{"
	"	buff[get_global_id(0)] += inc;"
	"}"
};

#define TEST_SIZE	100000000

int				main(void)
{
	cl_context			context;
	cl_command_queue	queue;

	try
	{
		std::tie(context, queue) = get_context(get_platform(),
			[](char const *err, void const*, size_t){
				std::cout << "notify " << err << std::endl;
			});
		{ // TEST
			cl_program const	test_program = get_program(context, g_test_srcs);
			cl_kernel const		test_init_kernel = get_kernel(test_program, "test_init");
			cl_kernel const		test_inc_kernel = get_kernel(test_program, "test_inc");
			uint32_t const		buff_length = TEST_SIZE;
			uint32_t const		buff_size = buff_length * sizeof(uint32_t);
			cl_mem const		test_buff = get_buffer(context, CL_MEM_READ_WRITE, buff_size);
			uint32_t const		init = 0;
			uint32_t const		inc = 42;

			CL_CALL(clSetKernelArg, test_init_kernel, 0, sizeof(cl_mem), &test_buff);
			CL_CALL(clSetKernelArg, test_init_kernel, 1, sizeof(uint32_t), &init);
			CL_CALL(clEnqueueNDRangeKernel, queue, test_init_kernel, 1, NULL,
				(size_t[]){buff_length}, NULL, 0, NULL, NULL);

			CL_CALL(clSetKernelArg, test_inc_kernel, 0, sizeof(cl_mem), &test_buff);
			CL_CALL(clSetKernelArg, test_inc_kernel, 1, sizeof(uint32_t), &inc);
			CL_CALL(clEnqueueNDRangeKernel, queue, test_inc_kernel, 1, NULL,
				(size_t[]){buff_length}, NULL, 0, NULL, NULL);
			CL_CALL(clEnqueueNDRangeKernel, queue, test_inc_kernel, 1, NULL,
				(size_t[]){buff_length}, NULL, 0, NULL, NULL);
			CL_CALL(clEnqueueNDRangeKernel, queue, test_inc_kernel, 1, NULL,
				(size_t[]){buff_length}, NULL, 0, NULL, NULL);

			clFinish(queue);

		}
		CL_CALL(clReleaseContext, context);
	}
	catch (std::runtime_error const &e)
	{
		std::cout << "OpenCL error: " << e.what() << std::endl;
	}
	return (0);
}
