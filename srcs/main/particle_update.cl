/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   particle_update.cl                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jaguillo <jaguillo@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2016/10/10 16:57:32 by jaguillo          #+#    #+#             */
/*   Updated: 2016/10/16 19:24:49 by jaguillo         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "particule.cl.h"

__kernel void		update(__global p_vertex *vertex, __global p_info *info,
						float4 center, float delta_t)
{
	uint const			id = get_global_id(0);

	float4 const		vel_0 = info[id].velocity;
	float4 const		pos_0 = vertex[id].pos;

	float const			G = 10.f;
	float const			center_mass = 3.f;

	float const			mid_delta_t = delta_t / 2.f;
	float4 const		pos_mid = pos_0 + vel_0 * mid_delta_t;

	float4 const		r_mid = center - pos_mid;
	float const			r_mid_sq = dot(r_mid, r_mid);
	float4 const		acc_mid = G * center_mass / (r_mid_sq + 0.5f) * r_mid;

	float4 const		vel_1 = vel_0 + acc_mid * delta_t;
	float4				pos_1 = pos_0 + (vel_0 + vel_1) * mid_delta_t;

	pos_1.w = 1.f;
	vertex[id].pos = pos_1;
	info[id].velocity = vel_1;
}

__kernel void		explode(__global p_vertex *vertex, __global p_info *info,
						float4 center, float force)
{
	uint const			id = get_global_id(0);
	float4 const		d = center - vertex[id].pos;
	float const			f = force / max(1.f, dot(d, d));

	info[id].velocity -= normalize(d) * f;
}