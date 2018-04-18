#ifndef ROUTER_H
#define ROUTER_H

#include <systemc.h>
#include <list>
#include <string.h>

// a empty packet has src_x == src_y == dest_x == dest_y == -1
//typedef int token_type;

struct token_type
{
	double real;
	double image;

	token_type(double re, double im) 
	{
		real = re;
		image = im;
	}
	token_type()
	{
		real = 0;
		image = 0;
	}
};

struct packet
{
	int src_x, src_y;
	int dest_x, dest_y;
	token_type token;
	std::string name;

	packet(int sx = -1, int sy = -1, int dx = -1, int dy = -1,
		token_type t = token_type(-1,-1), std::string na = "NULL")
		: src_x(sx), src_y(sy), dest_x(dx), dest_y(dy), token(t), name(na)
	{
	}

	bool operator==(const packet &x) const
	{
		return (x.src_x == src_x) && (x.src_y == src_y)
			&& (x.dest_x == dest_x) && (x.dest_y == dest_y)
			&& (x.token.real == token.real) && (x.token.image == token.image);
	}
}; // struct packet

std::ostream &operator<<(std::ostream &o, const packet &p);
void sc_trace(sc_trace_file *tf, const packet &p, const std::string &name);

SC_MODULE(router)
{
	sc_in<bool> clock; // port for the global clock

	enum {PE = 0, NORTH, SOUTH, EAST, WEST, PORTS};
	sc_in<packet> port_in[PORTS]; // input ports
	sc_out<packet> port_out[PORTS]; // output ports

	void main(); // specify the functionality of router per clock cycle

	SC_CTOR(router)
		: x_(-1), y_(-1)
	{
		SC_METHOD(main);
		sensitive << clock.pos();
		EAST_queue = 0;
		WEST_queue = 0;
		PE_queue = 0;
		Total_queue = 0;
	}

	// use this function to set the coordinates of the router
	void set_xy(int x, int y);

public:
	int count_RP;
	int count_east, count_west, count_north, count_south;
	int EAST_queue, WEST_queue, PE_queue, Total_queue;

protected:
	std::list<packet> out_queue_[PORTS]; // output queues

	int x_, y_; // location of the router

	void read_packet(int iport); // read a packet from the link
	void write_packet(int iport); // write a packet to the link
	void route_packet_xy(packet p); // route the packet to the output queue
}; // router

#endif // ROUTER_H
