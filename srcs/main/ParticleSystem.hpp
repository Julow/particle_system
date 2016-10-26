// ************************************************************************** //
//                                                                            //
//                                                        :::      ::::::::   //
//   ParticleSystem.hpp                                 :+:      :+:    :+:   //
//                                                    +:+ +:+         +:+     //
//   By: juloo <juloo@student.42.fr>                +#+  +:+       +#+        //
//                                                +#+#+#+#+#+   +#+           //
//   Created: 2016/10/26 19:12:28 by juloo             #+#    #+#             //
//   Updated: 2016/10/26 19:22:40 by juloo            ###   ########.fr       //
//                                                                            //
// ************************************************************************** //

#ifndef PARTICLESYSTEM_HPP
# define PARTICLESYSTEM_HPP

# include "ClBuffer.hpp"
# include "ClGlBuffer.hpp"
# include "ClKernel.hpp"
# include "ClProgram.hpp"
# include "GlBuffer.hpp"
# include "GlProgram.hpp"
# include "GlUniform.hpp"
# include "particle.cl.hpp"

# include <glm/glm.hpp>

class	ParticleSystem
{
public:
	ParticleSystem(cl_context context, unsigned particle_count);
	virtual ~ParticleSystem();

	void			init(cl_command_queue queue);

	void			set_center(glm::vec3 const &center);
	void			set_matrix(glm::mat4 const &m);
	void			explode(float force);

	void			update(cl_command_queue queue, float delta_t);
	void			render();

private:
	unsigned		_particule_count;

	cl_context		_cl_context;

	ClProgram		_update_program;
	ClKernel<cl_mem, cl_mem>	_init_square_kernel;
	ClKernel<cl_mem, cl_mem>	_init_sphere_kernel;
	ClKernel<cl_mem, cl_mem>	_init_cube_kernel;
	ClKernel<cl_mem, cl_mem, cl_uint>	_init_sphere_rand_kernel;
	ClKernel<cl_mem, cl_mem, cl_uint>	_init_cube_rand_kernel;
	ClKernel<cl_mem, cl_mem, cl_float4, cl_float>	_update_gravity_kernel;
	ClKernel<cl_mem, cl_mem, cl_float4, cl_float>	_update_spring_kernel;
	ClKernel<cl_mem, cl_mem, cl_float4, cl_float>	_explode_kernel;

	GlProgram		_render_program;
	GlUniform<glm::mat4>	_uniform_matrix;

	ClGlBuffer<particle::p_vertex,
			attrib<particle::p_vertex, particle::float4, &particle::p_vertex::pos>,
			attrib<particle::p_vertex, particle::float4, &particle::p_vertex::color>
		>			_particle_vertices;

	ClBuffer<particle::p_info>	_particle_infos;

	cl_float4				_center;

	std::optional<float>	_explode;

private:
	ParticleSystem() = delete;
	ParticleSystem(ParticleSystem &&src) = delete;
	ParticleSystem(ParticleSystem const &src) = delete;
	ParticleSystem	&operator=(ParticleSystem &&rhs) = delete;
	ParticleSystem	&operator=(ParticleSystem const &rhs) = delete;
};

#endif
