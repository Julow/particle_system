/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   particle.cl                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jaguillo <jaguillo@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2016/10/10 16:57:32 by jaguillo          #+#    #+#             */
/*   Updated: 2016/10/10 18:10:20 by jaguillo         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "particule.cl.h"

__kernel void		init(__global particule *buff)
{
	particule		p;
	uint const		id = get_global_id(0);
	float const		w = (id + 1) / (float)get_global_size(0);

	p.pos = (float4){mix(-1.f, 1.f, w), 0.f, 0.f, 1.f};
	p.color = (float4){w, 0.f, 0.f, 1.f};
	p.velocity = (float4){0.f, 0.f, 0.f, 0.f};
	p.mass = 1.f;
	buff[id] = p;
}

__kernel void		update(__global particule *buff, 
						float4 center, float delta_t)
{
	uint const			id = get_global_id(0);
	float const			G = 10.f;
	float const			center_mass = 100.f;

	float4 const		r_diff = center - buff[id].pos;
	float const			r_sq = dot(r_diff, r_diff);
	float const			force = buff[id].mass * center_mass / (
							max(1.f, r_sq) // TODO: fix
							) * G;

	float const			delta_v = force / buff[id].mass * delta_t;
	buff[id].velocity += normalize(r_diff) * delta_v;

	buff[id].pos += buff[id].velocity * delta_t;
}
