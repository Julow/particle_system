/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   particle.cl                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jaguillo <jaguillo@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2016/10/10 16:57:32 by jaguillo          #+#    #+#             */
/*   Updated: 2016/10/13 12:43:02 by jaguillo         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "particule.cl.h"

void				_init_particle(__global p_vertex *dst_vertex,
						__global p_info *dst_info)
{
	float const		w = (get_global_id(0) + 1) / (float)get_global_size(0);

	dst_vertex->pos = (float4){0, 0, 0, 0};
	dst_vertex->color = mix((float4){0.1f, 0.8f, 0.8f, 1.f},
			(float4){0.9f, 0.8f, 0.2f, 1.f}, w);
	dst_info->velocity = (float4){0.f, 0.f, 0.f, 0.f};
	dst_info->mass = 1.f;
}

__kernel void		init_square(__global p_vertex *vertex,
						__global p_info *info)
{
	uint const		id = get_global_id(0);
	uint const		per_line = sqrt((float)get_global_size(0));

	_init_particle(vertex + id, info + id);
	vertex[id].pos = (float4){
		(id % per_line) / (float)per_line * 2.f - 1.f,
		(id / per_line) / (float)per_line * 2.f - 1.f,
		0.f,
		1.f
	};
}

__kernel void		init_sphere(__global p_vertex *vertex,
						__global p_info *info)
{
	uint const		id = get_global_id(0);
	uint const		global_size = get_global_size(0);
	float const		offset = 2.f / global_size;
	float			y, r, phi;

	_init_particle(vertex + id, info + id);
	y = ((id * offset) - 1.f) + (offset / 2.f);
	r = sqrt(1.f - pow(y, 2.f));
	phi = (id % global_size) * M_PI * (3.f - sqrt(5.f));
	vertex[id].pos = (float4){cos(phi) * r, y, sin(phi) * r, 1.f};
}

__kernel void		init_cube(__global p_vertex *vertex,
						__global p_info *info)
{
	uint const		id = get_global_id(0);
	uint const		per_line = sqrt((float)get_global_size(0));
	uint const		side = id % 3;
	float3			tmp;

	_init_particle(vertex + id, info + id);
	tmp.x = (id % per_line) / (float)per_line * 2.f - 1.f;
	tmp.y = (id / per_line) / (float)per_line * 2.f - 1.f;
	tmp.z = ((id % 6) >= 3) ? -1.f : 1.f;
	if (side == 0)
		vertex[id].pos = (float4){tmp.x, tmp.y, tmp.z, 1.f};
	else if (side == 1)
		vertex[id].pos = (float4){tmp.x, tmp.z, tmp.y, 1.f};
	else
		vertex[id].pos = (float4){tmp.z, tmp.y, tmp.x, 1.f};
}

__kernel void		update(__global p_vertex *vertex, __global p_info *info,
						float4 center, float delta_t)
{
	uint const			id = get_global_id(0);
	float const			G = 3.f;
	float const			center_mass = 1.f;

	float4 const		r_diff = center - vertex[id].pos;
	float const			r_sq = dot(r_diff, r_diff);
	float const			force = info[id].mass * center_mass / (
							max(1.f, r_sq) // TODO: fix
							) * G;

	float const			delta_v = force / info[id].mass * delta_t;
	info[id].velocity += normalize(r_diff) * delta_v;

	vertex[id].pos += info[id].velocity * delta_t;
}

__kernel void		explode(__global p_vertex *vertex, __global p_info *info,
						float4 center, float force)
{
	uint const			id = get_global_id(0);
	float4 const		d = center - vertex[id].pos;
	float const			f = force / max(1.f, dot(d, d));

	info[id].velocity -= normalize(d) * f;
}
