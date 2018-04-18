#ifndef PE_H
#define PE_H

#include "router.h"

SC_MODULE(PE_base)
{
	sc_in<bool> clock; // port for the global clock

	sc_in<packet> data_in; // from router
	sc_out<packet> data_out; // to router

	void main() // specify the functionality of PE per clock cycle
	{
		read_input();
		execute();
		write_output();
	}

	SC_CTOR(PE_base)
		: x_(-1), y_(-1)
	{
		SC_METHOD(main);
		sensitive << clock.pos();
		queue = 0;
	}

	// use this function to set the coordinates of the PE
	void set_xy(int x, int y);

	virtual ~PE_base() {}
public:
	int count_PI_fire, count_PO_fire, count_P_fire;
	size_t queue;
	bool start_point;
	bool end_point;

protected:
	std::list<packet> out_queue_; // output queue
	std::list<packet> buffer; //store packets for computation
	packet packet_in_; // incoming packet from the router

	int x_, y_; // location of the PE

	virtual token_type multi(token_type t, int w);
	virtual void read_input(); // read a packet from the the router
	virtual void execute() = 0; // abstraction of computations
	virtual void write_output(); // // send a packet to the router
}; // PE_base

// for PI and PO
class PE_IO : public PE_base
{
public:
	PE_IO(const char *name) : PE_base(name) {}

protected:
	std::list<std::string> buf_name;
	token_type buf[8];

	void execute();

	void fire_PI();
	void fire_PO();
}; // class PE_IO

// for P1
class PE_0 : public PE_base
{
public:
	PE_0(const char *name) : PE_base(name) {}

protected:
	void execute();

	void fire();
};

class PE_1 : public PE_base
{
public:
	PE_1(const char *name) : PE_base(name) {}

protected:
	void execute();

	void fire();
};

class PE_2 : public PE_base
{
public:
	PE_2(const char *name) : PE_base(name) {}

protected:
	void execute();

	void fire();
};

class PE_3 : public PE_base
{
public:
	PE_3(const char *name) : PE_base(name) {}

protected:
	void execute();

	void fire();
};

class PE_5 : public PE_base
{
public:
	PE_5(const char *name) : PE_base(name) {}

protected:
	void execute();

	void fire();
};

class PE_6 : public PE_base
{
public:
	PE_6(const char *name) : PE_base(name) {}

protected:
	void execute();

	void fire();
};

class PE_7 : public PE_base
{
public:
	PE_7(const char *name) : PE_base(name) {}

protected:
	void execute();

	void fire();
};

class PE_8 : public PE_base
{
public:
	PE_8(const char *name) : PE_base(name) {}

protected:
	void execute();

	void fire();
};

#endif // PE_H
