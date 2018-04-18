#include <time.h>
#include <systemc.h>

#include "router.h"
#include "pe.h"

SC_MODULE(top)
{
public:
	enum {N = 3};

	router *routers[N][N];
	PE_base *pes[N][N];

	sc_signal<packet> router_to_pe[N][N], pe_to_router[N][N];
	sc_signal<packet> router_to_router_east[N-1][N], router_to_router_west[N-1][N];
	sc_signal<packet> router_to_router_north[N][N-1], router_to_router_south[N][N-1];
	sc_signal<packet> terminal_loop_north[N][N], terminal_loop_south[N][N];
	sc_signal<packet> terminal_loop_east[N][N], terminal_loop_west[N][N];
	sc_signal<bool> clock;

	SC_CTOR(top)
	{
		create_pes();
		create_network();
	}

protected:
	void create_pes()
	{
				pes[0][0] = new PE_0("P0");
				pes[0][0]->clock(clock);
				pes[0][0]->set_xy(0, 0);

				pes[1][0] = new PE_1("P1");
				pes[1][0]->clock(clock);
				pes[1][0]->set_xy(1, 0);

				pes[2][0] = new PE_2("P2");
				pes[2][0]->clock(clock);
				pes[2][0]->set_xy(2, 0);

				pes[0][1] = new PE_3("P3");
				pes[0][1]->clock(clock);
				pes[0][1]->set_xy(0, 1);

				pes[1][1] = new PE_IO("PI/PO");
				pes[1][1]->clock(clock);
				pes[1][1]->set_xy(1, 1);

				pes[2][1] = new PE_5("P5");
				pes[2][1]->clock(clock);
				pes[2][1]->set_xy(2, 1);

				pes[0][2] = new PE_6("P6");
				pes[0][2]->clock(clock);
				pes[0][2]->set_xy(0, 2);

				pes[1][2] = new PE_7("P7");
				pes[1][2]->clock(clock);
				pes[1][2]->set_xy(1, 2);

				pes[2][2] = new PE_8("P8");
				pes[2][2]->clock(clock);
				pes[2][2]->set_xy(2, 2);
	}

	void create_network()
	{
		for (int y = 0; y < 3; ++y)
		{
			for (int x = 0; x < 3; ++x)
			{
				char name[100];
				sprintf(name, "router(%d,%d)", x, y);

				// create router
				routers[x][y] = new router(name);
				routers[x][y]->set_xy(x,y);
				routers[x][y]->clock(clock);


				// connect router to west routers
				if (x != 0)
				{
					routers[x][y]->port_out[router::WEST](
						router_to_router_west[x - 1][y]);
					routers[x][y]->port_in[router::WEST](
						router_to_router_east[x - 1][y]);
				}
				else // or make a loop
				{
					routers[x][y]->port_out[router::WEST](
						terminal_loop_west[x][y]);
					routers[x][y]->port_in[router::WEST](
						terminal_loop_west[x][y]);
				}
				if (x != 2) // connect router to east routers
				{
					routers[x][y]->port_out[router::EAST](
						router_to_router_east[x][y]);
					routers[x][y]->port_in[router::EAST](
						router_to_router_west[x][y]);
				}
				else // or make a loop
				{
					routers[x][y]->port_out[router::EAST](
						terminal_loop_east[x][y]);
					routers[x][y]->port_in[router::EAST](
						terminal_loop_east[x][y]);
				}
				// connect router to North routers
				if (y != 0)
				{
					routers[x][y]->port_out[router::NORTH](
						router_to_router_north[x][y-1]);
					routers[x][y]->port_in[router::NORTH](
						router_to_router_south[x][y-1]);
				}
				else // or make a loop
				{
					routers[x][y]->port_out[router::NORTH](
						terminal_loop_north[x][y]);
					routers[x][y]->port_in[router::NORTH](
						terminal_loop_north[x][y]);
				}
				if (y != 2) // connect router to South routers
				{
					routers[x][y]->port_out[router::SOUTH](
						router_to_router_south[x][y]);
					routers[x][y]->port_in[router::SOUTH](
						router_to_router_north[x][y]);
				}
				else // or make a loop
				{
					routers[x][y]->port_out[router::SOUTH](
						terminal_loop_south[x][y]);
					routers[x][y]->port_in[router::SOUTH](
						terminal_loop_south[x][y]);
				}

				// connect router to PE
				routers[x][y]->port_out[router::PE](router_to_pe[x][y]);
				routers[x][y]->port_in[router::PE](pe_to_router[x][y]);
				pes[x][y]->data_in(router_to_pe[x][y]);
				pes[x][y]->data_out(pe_to_router[x][y]);
				// find the location of each PEs and Routers
			}
		}
	}

}; // top

int sc_main(int argc , char *argv[])
{
	srand((int)time(NULL));

	top top_module("top");

	//printf("cycle  0 ================================\n");
	sc_start(0, SC_NS);

	int clock_num = 5000;
	std::list<int> start_time;
	std::list<int> end_time;
	int min_latency = 9999;

	for(int i = 1; i < clock_num + 1; i++){
		
		//printf("cycle %2d ================================\n", i);

		top_module.clock.write(1);
		sc_start(10, SC_NS);
		top_module.clock.write(0);
		sc_start(10, SC_NS);

		if (top_module.pes[1][1]->start_point == true) {
			start_time.push_back(i);
		}
		if (top_module.pes[1][1]->end_point == true) {
			end_time.push_back(i);
		}
		
	}
	assert(start_time.size() >= end_time.size());
	std::list<int>::iterator its = start_time.begin();
	for (std::list<int>::iterator ite = end_time.begin(); ite != end_time.end(); ++ite)
	{
		if (min_latency > *ite - *its)
			min_latency = *ite - *its;
	}

	printf("================Analysis of %d cycles================\n", clock_num);
	int max_queue = 0;
	for (int y = 0; y < 3; ++y)
	{
		for (int x = 0; x < 3; ++x)
		{
			if (max_queue < top_module.routers[x][y]->Total_queue)
				max_queue = top_module.routers[x][y]->Total_queue;
		}
	}
	std::cout << "max queue for routers = " << max_queue << std::endl;
	printf("throughtput = %.3f\n", top_module.pes[1][1]->count_PI_fire / (double)(clock_num));
	//std::cout << "throughput = " << throughput << std::endl;
	std::cout << "min latency = " << min_latency << std::endl;

	return 0;
}
