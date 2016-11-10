// ************************************************************************** //
//                                                                            //
//                                                        :::      ::::::::   //
//   ParticleSystem.cpp                                 :+:      :+:    :+:   //
//                                                    +:+ +:+         +:+     //
//   By: juloo <juloo@student.42.fr>                +#+  +:+       +#+        //
//                                                +#+#+#+#+#+   +#+           //
//   Created: 2016/10/26 19:13:58 by juloo             #+#    #+#             //
//   Updated: 2016/11/10 13:16:33 by jaguillo         ###   ########.fr       //
//                                                                            //
// ************************************************************************** //

#include "ParticleSystem.hpp"
#include "gen.h"

#include <ctime>

ParticleSystem::ParticleSystem(cl_context context, unsigned particle_count) :
	_particule_count(particle_count),
	_cl_context(context),

	_update_program(_cl_context,
			{prog_particle_init, prog_particle_update}),
	_init_square_kernel(_update_program, "init_square"),
	_init_sphere_kernel(_update_program, "init_sphere"),
	_init_cube_kernel(_update_program, "init_cube"),
	_init_sphere_rand_kernel(_update_program, "init_rand_sphere"),
	_init_cube_rand_kernel(_update_program, "init_rand_cube"),

	_init_springs_kernel(_update_program, "init_springs"),

	_update_gravity_kernel(_update_program, "update_gravity"),
	_update_spring_kernel(_update_program, "update_spring"),
	_explode_kernel(_update_program, "explode"),

	_render_program({
			{GL_VERTEX_SHADER, prog_particle_render_vert},
			{GL_FRAGMENT_SHADER, prog_particle_render_frag},
		}),
	_uniform_matrix(_render_program, "u_matrix"),

	_particle_vertices(_cl_context, _particule_count, nullptr),
	_particle_infos(_cl_context, _particule_count),

	_center{{0.f, 0.f, 0.f, 0.f}}
{}

ParticleSystem::~ParticleSystem()
{}

void			ParticleSystem::init(cl_command_queue queue)
{
	auto	p_vertices = cl_acquire(queue, _particle_vertices);

	// _init_square_kernel.make_work<1>(_particule_count)
	// _init_sphere_kernel.make_work<1>(_particule_count)
	_init_cube_kernel.make_work<1>(_particule_count)
			(queue, std::get<0>(p_vertices).get_handle(), _particle_infos.get_handle());
	// _init_sphere_rand_kernel.make_work<1>(_particule_count)
	// _init_cube_rand_kernel.make_work<1>(_particule_count)
			// (queue, std::get<0>(p_vertices).get_handle(), _particle_infos.get_handle(), std::clock());
	_init_springs_kernel.make_work<1>(_particule_count)
			(queue, std::get<0>(p_vertices).get_handle(), _particle_infos.get_handle(), std::clock());
}

void			ParticleSystem::set_center(glm::vec3 const &center)
{
	_center = {{center.x, center.y, center.z, 0.f}};
}

void			ParticleSystem::set_matrix(glm::mat4 const &m)
{
	_uniform_matrix = m;
}

void			ParticleSystem::explode(float force)
{
	_explode = force;
}

void			ParticleSystem::update(cl_command_queue queue, float delta_t)
{
	auto	p_vertices = cl_acquire(queue, _particle_vertices);

	if (_explode)
	{
		_explode_kernel.make_work<1>(_particule_count)
				(queue, std::get<0>(p_vertices).get_handle(),
					_particle_infos.get_handle(), _center, *_explode);
		_explode = std::nullopt;
	}
	_update_spring_kernel
	// _update_gravity_kernel
			.make_work<1>(_particule_count)
			(queue, std::get<0>(p_vertices).get_handle(), _particle_infos.get_handle(),
					_center, delta_t);
}

void			ParticleSystem::render()
{
	_render_program.use();
	_particle_vertices.gl_bind();
	glDrawArrays(GL_POINTS, 0, _particule_count);
}
