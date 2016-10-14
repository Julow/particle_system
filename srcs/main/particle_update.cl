/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   particle_update.cl                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jaguillo <jaguillo@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2016/10/10 16:57:32 by jaguillo          #+#    #+#             */
/*   Updated: 2016/10/14 14:59:43 by jaguillo         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "particule.cl.h"

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
