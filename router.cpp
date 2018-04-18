#include "router.h"

std::ostream &operator<<(std::ostream &o, const packet &p)
{
	char buf[100];
	sprintf(buf, "(%d,%d)->(%d,%d)",
		p.src_x, p.src_y, p.dest_x, p.dest_y);
	return o << buf << ", " << p.token.real << " + i" << p.token.image;
}

void sc_trace(sc_trace_file *tf, const packet &p, const std::string &name)
{
	sc_trace(tf, p.src_x, name+".src.x");
	sc_trace(tf, p.src_y, name+".src.y");
	sc_trace(tf, p.dest_x, name+".dest.x");
	sc_trace(tf, p.dest_y, name+".dest.y");
	sc_trace(tf, p.token.real, name+".token.real");
	sc_trace(tf, p.token.image, name + ".token.image");
}

void router::main()
{
	assert((x_ != -1) && (y_ != -1)); // to identify PE

	for (int iport = 0; iport < PORTS; ++iport)
		read_packet(iport);

	for (int iport = 0; iport < PORTS; ++iport)
		write_packet(iport);
}

void router::set_xy(int x, int y)
{
	assert((x_ == -1) && (y_ == -1)); // set once only
	assert((x != -1) && (y != -1)); // must use a legal location

	x_ = x;
	y_ = y;
}

void router::read_packet(int iport)
{
	assert(iport < PORTS);

	packet p = port_in[iport].read();

	if ((p.src_x == -1) && (p.src_y == -1))
		return; // empty packet

	route_packet_xy(p);
}

void router::write_packet(int iport)
{
	assert(iport < PORTS);

	if (out_queue_[iport].empty())
	{
		port_out[iport].write(packet()); // write an empty packet
	}
	else
	{
		port_out[iport].write(out_queue_[iport].front());
		out_queue_[iport].pop_front();
	}
}

void router::route_packet_xy(packet p)
{
	if ((p.dest_x == -1) || (p.dest_y == -1))
	{
		printf("router (%d,%d): drop packet with invalid destination"
			" (%d,%d)->(%d,%d)\n",
			x_, y_,
			p.src_x, p.src_y, p.dest_x, p.dest_y);
		return;
	}


	if (p.dest_y == y_)
	{
		if (p.dest_x == x_) // to PE
		{
			count_RP++;
			out_queue_[PE].push_back(p);
		}
		else if (p.dest_x < x_) // left to WEST
		{
			count_west++;
			out_queue_[WEST].push_back(p);
		}
		else // (p.dest_x > x_) right to EAST
		{
			count_east++;
			out_queue_[EAST].push_back(p);
		}
	}
	else if (p.dest_y < y_) // up to NORTH
	{
		count_north++;
		out_queue_[NORTH].push_back(p);
	}
	else // down to SOUTH
	{
		count_south++;
		out_queue_[SOUTH].push_back(p);
	}
	if (Total_queue < out_queue_[PE].size() + out_queue_[WEST].size() + out_queue_[EAST].size() + out_queue_[NORTH].size() + out_queue_[NORTH].size())
		Total_queue = out_queue_[PE].size() + out_queue_[WEST].size() + out_queue_[EAST].size() + out_queue_[NORTH].size() + out_queue_[NORTH].size();
}
