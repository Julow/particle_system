// ************************************************************************** //
//                                                                            //
//                                                        :::      ::::::::   //
//   FpsCounter.tpp                                     :+:      :+:    :+:   //
//                                                    +:+ +:+         +:+     //
//   By: jaguillo <jaguillo@student.42.fr>          +#+  +:+       +#+        //
//                                                +#+#+#+#+#+   +#+           //
//   Created: 2016/10/12 17:38:53 by jaguillo          #+#    #+#             //
//   Updated: 2016/10/12 20:04:17 by jaguillo         ###   ########.fr       //
//                                                                            //
// ************************************************************************** //

#ifndef FPSCOUNTER_TPP
# define FPSCOUNTER_TPP

template<typename C, unsigned R>
void		FpsCounter<C, R>::begin(FpsCounter<C, R>::time_point_t const &now)
{
	_frame_begin = now;
}

template<typename C, unsigned R>
bool		FpsCounter<C, R>::end(FpsCounter<C, R>::time_point_t const &now)
{
	static const FpsCounter<C, R>::duration_t	RES_INTERVAL =
			std::chrono::duration<long long, std::ratio<1, R>>(1);
	FpsCounter<C, R>::duration_t const			delta_t = now - _frame_begin;

	_last_update = now;
	if ((_frames[_i].begin + RES_INTERVAL) <= now)
	{
		if (++_i >= BUFF_SIZE)
			_i = 0;
		_frames[_i] = {now, 1, delta_t, delta_t, delta_t};
		return (_frames[BUFF_SIZE - 1].begin > time_point_t());
	}
	_frames[_i].count++;
	_frames[_i].total_t += delta_t;
	if (_frames[_i].min_t > delta_t) _frames[_i].min_t = delta_t;
	else if (_frames[_i].max_t < delta_t) _frames[_i].max_t = delta_t;
	return (false);
}

template<typename C, unsigned R>
float		FpsCounter<C, R>::get_fps() const
{
	unsigned const		last = (_i + 1) % FpsCounter<C, R>::BUFF_SIZE;
	std::chrono::duration<float, std::ratio<1>> const	delta =
								_last_update - _frames[last].begin;
	unsigned			total_count;

	total_count = 0;
	for (frame const &f : _frames)
		total_count += f.count;
	return (total_count / delta.count());
}

template<typename C, unsigned R>
std::tuple<float, float, float>
			FpsCounter<C, R>::get_stats() const
{
	using sec_duration = std::chrono::duration<float, std::ratio<1>>;
	sec_duration	min_t;
	sec_duration	max_t;
	sec_duration	total_t{};
	unsigned		count{};

	min_t = _frames[0].min_t;
	max_t = _frames[0].max_t;
	for (frame const &f : _frames)
	{
		if (min_t > f.min_t) min_t = f.min_t;
		else if (max_t < f.max_t) max_t = f.max_t;
		total_t += f.total_t;
		count += f.count;
	}
	return {min_t.count(), max_t.count(), total_t.count() / count};
}

#endif
