#include "pe.h"
#include <string>
#include <iomanip>

void PE_base::set_xy(int x, int y)
{
	assert((x_ == -1) && (y_ == -1)); // set once only
	assert((x != -1) && (y != -1)); // must use a legal location

	x_ = x;
	y_ = y;
}

void PE_base::read_input()
{
	packet_in_ = data_in.read();
}

void PE_base::write_output()
{
	if (out_queue_.empty())
	{
		data_out.write(packet());
	}
	else
	{
		data_out.write(out_queue_.front());
		out_queue_.pop_front();
	}
}

token_type PE_base::multi(token_type t, int w) {
	token_type token;
	switch (w) 
	{
	case 1:
		token.real = t.real * 1 / sqrt(2) + t.image * (-1 / sqrt(2));
		token.image = t.real * (-1 / sqrt(2)) + t.image * 1 / sqrt(2);
		break;
	case 2:
		token.real = t.real * 0 + t.image * (-1);
		token.image = t.real * (-1) + t.image * 0;
		break;
	case 3:
		token.real = t.real * (-1 / sqrt(2)) + t.image * (-1 / sqrt(2));
		token.image = t.real * (-1 / sqrt(2)) + t.image * (-1 / sqrt(2));
		break;
	default:
		printf("multiplication error\n");
	}
	return token;
}

void PE_IO::execute()
{
	start_point = false;
	end_point = false;
	// decide if we are going to fire PI
	int r = rand()%100;
	if (r < 13) {
		fire_PI();
		start_point = true;
	}

	//printf("PO got the packet %.2f + i%.2f",
	//	packet_in_.token.real, packet_in_.token.image);
	//std::cout << "name: " << packet_in_.name << std::endl;

	// fire PO if the incoming packet is valid
	if ((packet_in_.src_x != -1)
		&& (packet_in_.src_y != -1))
	{
		fire_PO();
	}
}

void PE_IO::fire_PI()
{
	
	double real_[8] = { 2, 2, 1, 1, 0, 0, 1, 1 };
	double image_[8] = { 0, 0, 0, 0, 0, 0, 0, 0 };

	count_PI_fire++;
	int P1_x[8] = { 1,1,1,1,1,1,1,1 };
	int P1_y[8] = { 0,0,2,2,0,0,2,2 };

	packet p[8];
	for (int i = 0; i < 8; ++i)
	{
		token_type token(real_[i], image_[i]);
		std::stringstream temp_name;
		temp_name <<"x_"<< i;
		std::string token_name = temp_name.str();

		p[i] = packet(x_, y_, P1_x[i], P1_y[i], token, token_name);
		//out_queue_.push_back(p[i]);
		/*std::cout << "PI: send " << token_name;
		printf(" %.2f + i%.2f to PE(%d, %d)\n", p[i].token.real, p[i].token.image, P1_x[i], P1_y[i]);*/
	}

	out_queue_.push_back(p[0]);
	out_queue_.push_back(p[2]);
	out_queue_.push_back(p[4]);
	out_queue_.push_back(p[6]);
	out_queue_.push_back(p[1]);
	out_queue_.push_back(p[3]);
	out_queue_.push_back(p[5]);
	out_queue_.push_back(p[7]);

	if (queue < out_queue_.size())
		queue = out_queue_.size();
}

void PE_IO::fire_PO()
{
	buf_name.push_back(packet_in_.name);
	count_PO_fire++;
	assert((packet_in_.src_x != -1)
		&& (packet_in_.src_y != -1));

	std::string packet_name = packet_in_.name;
	if (packet_name == "X_0")
	{
		buf[0] = packet_in_.token;
	}
	else if (packet_name == "X_1")
	{
		buf[1] = packet_in_.token;
	}
	else if (packet_name == "X_2")
	{
		buf[2] = packet_in_.token;
	}
	else if (packet_name == "X_3")
	{
		buf[3] = packet_in_.token;
	}
	else if (packet_name == "X_4")
	{
		buf[4] = packet_in_.token;
	}
	else if (packet_name == "X_5")
	{
		buf[5] = packet_in_.token;
	}
	else if (packet_name == "X_6")
	{
		buf[6] = packet_in_.token;
	}
	else if (packet_name == "X_7")
	{
		buf[7] = packet_in_.token;
	}
	if (buf_name.size() == 8)
	{
		end_point = true;
		printf("output begins:\n");
		for (int i = 0; i < 8; ++i)
		{
			std::stringstream temp_name;
			temp_name << "X_" << i;
			std::string token_name = temp_name.str();
			std::cout <<std::setprecision(3)<< token_name << ": " << buf[i].real << "+i" << buf[i].image << std::endl;
		}
		buf_name.clear();
	}

	if (queue < out_queue_.size())
		queue = out_queue_.size();
}

void PE_0::execute()
{
	// fire the actor if the incoming packet is valid
	if ((packet_in_.src_x != -1)
		&& (packet_in_.src_y != -1))
	{
		fire(); 
		/*printf("pe0 got the packet %.2f + i%.2f\n", 
			packet_in_.token.real, packet_in_.token.image);*/
	}

}

void PE_0::fire()
{
	assert((packet_in_.src_x != -1)
		&& (packet_in_.src_y != -1));

	count_P_fire++;

	if (packet_in_.name == "z_0")
	{
		buffer.push_back(packet_in_);
	}
	else if (packet_in_.name == "z_4")
	{
		packet z0;
		for (std::list<packet>::iterator it = buffer.begin(); it != buffer.end(); ++it)
		{
			if (it->name == "z_0")
			{
				z0 = *it;
				buffer.erase(it);
				break;
			}
		}
		assert(z0.name == "z_0");

		int PO_x = 1;
		int PO_y = 1;

		token_type token_plus(z0.token.real + packet_in_.token.real, z0.token.image + packet_in_.token.image);
		token_type token_minus(z0.token.real - packet_in_.token.real, z0.token.image - packet_in_.token.image);

		packet p_plus(x_, y_, PO_x, PO_y, token_plus, "X_0");
		packet p_minus(x_, y_, PO_x, PO_y, token_minus, "X_4");

		out_queue_.push_back(p_plus);
		out_queue_.push_back(p_minus);

		/*std::cout << "P0 receive " << packet_in_.name << " from (" << packet_in_.src_x << ", "
			<< packet_in_.src_y << ") and " << z0.name << " from (" << z0.src_x << ", " << z0.src_y << ")" << std::endl;*/
	}
	else
	{
		printf("\nsomething wrong, invalid input at PE(0,0)!\n");
	}	

	if (queue < out_queue_.size())
		queue = out_queue_.size();

}

void PE_1::execute()
{
	// fire the actor if the incoming packet is valid
	
	if ((packet_in_.src_x != -1)
		&& (packet_in_.src_y != -1))
	{
		fire(); 
		/*printf("pe1 got the packet %.2f + i%.2f\n", 
			packet_in_.token.real, packet_in_.token.image);*/
	}
}

void PE_1::fire()
{
	assert((packet_in_.src_x != -1)
		&& (packet_in_.src_y != -1));

	count_P_fire++;

	if (packet_in_.name == "x_0")
	{
		buffer.push_back(packet_in_);
	}
	else if (packet_in_.name == "x_4")
	{
		packet x0;
		for (std::list<packet>::iterator it = buffer.begin(); it != buffer.end(); ++it)
		{
			if (it->name == "x_0")
			{
				x0 = *it;
				buffer.erase(it);
				break;
			}
		}
		assert(x0.name == "x_0");

		token_type token_plus(x0.token.real + packet_in_.token.real, x0.token.image + packet_in_.token.image);
		token_type token_minus(x0.token.real - packet_in_.token.real, x0.token.image - packet_in_.token.image);

		packet p_plus(x_, y_, 0, 1, token_plus, "y_0"); //to PE(0,1)
		packet p_minus(x_, y_, 2, 1, token_minus, "y_1"); // to PE(2,1)

		out_queue_.push_back(p_plus);
		out_queue_.push_back(p_minus);

		/*std::cout << "P1 receive " << packet_in_.name << " from (" << packet_in_.src_x << ", "
			<< packet_in_.src_y << ") and " << x0.name << " from (" << x0.src_x << ", " << x0.src_y << ")" << std::endl;*/
	}
	else if (packet_in_.name == "x_1")
	{
		buffer.push_back(packet_in_);
	}
	else if (packet_in_.name == "x_5")
	{
		packet x1;
		for (std::list<packet>::iterator it = buffer.begin(); it != buffer.end(); ++it)
		{
			if (it->name == "x_1")
			{
				x1 = *it;
				buffer.erase(it);
				break;
			}
		}
		assert(x1.name == "x_1");

		token_type token_plus(x1.token.real + packet_in_.token.real, x1.token.image + packet_in_.token.image);
		token_type token_minus(x1.token.real - packet_in_.token.real, x1.token.image - packet_in_.token.image);

		packet p_plus(x_, y_, 0, 1, token_plus, "y_4");
		packet p_minus(x_, y_, 2, 1, token_minus, "y_5");

		out_queue_.push_back(p_plus);
		out_queue_.push_back(p_minus);

		/*std::cout <<"P1 receive " << packet_in_.name << " from (" << packet_in_.src_x << ", "
			<< packet_in_.src_y << ") and " << x1.name << " from (" << x1.src_x << ", " << x1.src_y << ")" << std::endl;*/
	}
	else
	{
		printf("\nsomething wrong, invalid input at PE(1,0)!\n");
	}


	if (queue < out_queue_.size())
		queue = out_queue_.size();

}

void PE_2::execute()
{
	
	// fire the actor if the incoming packet is valid
	if ((packet_in_.src_x != -1)
		&& (packet_in_.src_y != -1))
	{
		fire(); 
		/*printf("pe2 got the packet %.2f + i%.2f\n",
			packet_in_.token.real, packet_in_.token.image);*/
	}
}

void PE_2::fire()
{
	assert((packet_in_.src_x != -1)
		&& (packet_in_.src_y != -1));

	count_P_fire++;

	//int PO_x, PO_y;

	if (packet_in_.name == "z_2")
	{
		buffer.push_back(packet_in_);
	}
	else if (packet_in_.name == "z_6")
	{
		packet z2;
		for (std::list<packet>::iterator it = buffer.begin(); it != buffer.end(); ++it)
		{
			if (it->name == "z_2")
			{
				z2 = *it;
				buffer.erase(it);
				break;
			}
		}
		assert(z2.name == "z_2");

		int PO_x = 1;
		int PO_y = 1;
		token_type temp = multi(packet_in_.token, 1);

		token_type token_plus(z2.token.real + temp.real, z2.token.image + temp.image);
		token_type token_minus(z2.token.real - temp.real, z2.token.image - temp.image);

		packet p_plus(x_, y_, PO_x, PO_y, token_plus, "X_1");
		packet p_minus(x_, y_, PO_x, PO_y, token_minus, "X_5");

		out_queue_.push_back(p_plus);
		out_queue_.push_back(p_minus);

		/*std::cout <<"P2 receive " << packet_in_.name << " from (" << packet_in_.src_x << ", "
			<< packet_in_.src_y << ") and " << z2.name << " from (" << z2.src_x << ", " << z2.src_y << ")" << std::endl;*/
	}
	else
	{
		printf("\nsomething wrong, invalid input at PE(2,0)!\n");
	}


	if (queue < out_queue_.size())
		queue = out_queue_.size();

}

void PE_3::execute()
{
	
	// fire the actor if the incoming packet is valid
	if ((packet_in_.src_x != -1)
		&& (packet_in_.src_y != -1))
	{
		fire();
		/*printf("pe3 got the packet %.2f + i%.2f\n",
			packet_in_.token.real, packet_in_.token.image);*/
	}
}

void PE_3::fire()
{
	assert((packet_in_.src_x != -1)
		&& (packet_in_.src_y != -1));

	count_P_fire++;

	//int PO_x, PO_y;

	if (packet_in_.name == "y_0")
	{
		buffer.push_back(packet_in_);
	}
	else if (packet_in_.name == "y_2")
	{
		packet y0;
		for (std::list<packet>::iterator it = buffer.begin(); it != buffer.end(); ++it)
		{
			if (it->name == "y_0")
			{
				y0 = *it;
				buffer.erase(it);
				break;
			}
		}
		assert(y0.name == "y_0");

		token_type token_plus(y0.token.real + packet_in_.token.real, y0.token.image + packet_in_.token.image);
		token_type token_minus(y0.token.real - packet_in_.token.real, y0.token.image - packet_in_.token.image);

		packet p_plus(x_, y_, 0, 0, token_plus, "z_0");
		packet p_minus(x_, y_, 0, 2, token_minus, "z_1");

		out_queue_.push_back(p_plus);
		out_queue_.push_back(p_minus);

		/*std::cout <<"P3 receive " << packet_in_.name << " from (" << packet_in_.src_x << ", "
			<< packet_in_.src_y << ") and " << y0.name << " from (" << y0.src_x << ", " << y0.src_y << ")" << std::endl;*/
	}
	else if (packet_in_.name == "y_4")
	{
		buffer.push_back(packet_in_);
	}
	else if (packet_in_.name == "y_6")
	{
		packet y4;
		for (std::list<packet>::iterator it = buffer.begin(); it != buffer.end(); ++it)
		{
			if (it->name == "y_4")
			{
				y4 = *it;
				buffer.erase(it);
				break;
			}
		}
		assert(y4.name == "y_4");

		token_type token_plus(y4.token.real + packet_in_.token.real, y4.token.image + packet_in_.token.image);
		token_type token_minus(y4.token.real - packet_in_.token.real, y4.token.image - packet_in_.token.image);

		packet p_plus(x_, y_, 0, 0, token_plus, "z_4");
		packet p_minus(x_, y_, 0, 2, token_minus, "z_5");

		out_queue_.push_back(p_plus);
		out_queue_.push_back(p_minus);

		/*std::cout <<"P3 receive " << packet_in_.name << " from (" << packet_in_.src_x << ", "
			<< packet_in_.src_y << ") and " << y4.name << " from (" << y4.src_x << ", " << y4.src_y << ")" << std::endl;*/
	}
	else
	{
		printf("\nsomething wrong, invalid input at PE(0,1)!\n");
	}


	if (queue < out_queue_.size())
		queue = out_queue_.size();

}

void PE_5::execute()
{
	
	// fire the actor if the incoming packet is valid
	if ((packet_in_.src_x != -1)
		&& (packet_in_.src_y != -1))
	{
		fire();
		/*printf("pe5 got the packet %.2f + i%.2f\n",
			packet_in_.token.real, packet_in_.token.image);*/
	}
}

void PE_5::fire()
{
	assert((packet_in_.src_x != -1)
		&& (packet_in_.src_y != -1));

	count_P_fire++;

	//int PO_x, PO_y;

	if (packet_in_.name == "y_1")
	{
		buffer.push_back(packet_in_);
	}
	else if (packet_in_.name == "y_3")
	{
		packet y1;
		for (std::list<packet>::iterator it = buffer.begin(); it != buffer.end(); ++it)
		{
			if (it->name == "y_1")
			{
				y1 = *it;
				buffer.erase(it);
				break;
			}
		}
		assert(y1.name == "y_1");

		token_type temp = multi(packet_in_.token, 2);

		token_type token_plus(y1.token.real + temp.real, y1.token.image + temp.image);
		token_type token_minus(y1.token.real - temp.real, y1.token.image - temp.image);

		packet p_plus(x_, y_, 2, 0, token_plus, "z_2");
		packet p_minus(x_, y_, 2, 2, token_minus, "z_3");

		out_queue_.push_back(p_plus);
		out_queue_.push_back(p_minus);

		/*std::cout <<"P5 receive " << packet_in_.name << " from (" << packet_in_.src_x << ", "
			<< packet_in_.src_y << ") and " << y1.name << " from (" << y1.src_x << ", " << y1.src_y << ")" << std::endl;*/
	}
	else if (packet_in_.name == "y_5")
	{
		buffer.push_back(packet_in_);
	}
	else if (packet_in_.name == "y_7")
	{
		packet y5;
		for (std::list<packet>::iterator it = buffer.begin(); it != buffer.end(); ++it)
		{
			if (it->name == "y_5")
			{
				y5 = *it;
				buffer.erase(it);
				break;
			}
		}
		assert(y5.name == "y_5");
		token_type temp = multi(packet_in_.token, 2);

		token_type token_plus(y5.token.real + temp.real, y5.token.image + temp.image);
		token_type token_minus(y5.token.real - temp.real, y5.token.image - temp.image);

		packet p_plus(x_, y_, 2, 0, token_plus, "z_6");
		packet p_minus(x_, y_, 2, 2, token_minus, "z_7");

		out_queue_.push_back(p_plus);
		out_queue_.push_back(p_minus);

		/*std::cout <<"P5 receive " << packet_in_.name << " from (" << packet_in_.src_x << ", "
			<< packet_in_.src_y << ") and " << y5.name << " from (" << y5.src_x << ", " << y5.src_y << ")" << std::endl;*/
	}
	else
	{
		printf("\nsomething wrong, invalid input at PE(2,1)!\n");
	}


	if (queue < out_queue_.size())
		queue = out_queue_.size();

}

void PE_6::execute()
{
	
	// fire the actor if the incoming packet is valid
	if ((packet_in_.src_x != -1)
		&& (packet_in_.src_y != -1))
	{
		fire();
		/*printf("pe6 got the packet %.2f + i%.2f\n",
			packet_in_.token.real, packet_in_.token.image);*/
	}
}

void PE_6::fire()
{
	assert((packet_in_.src_x != -1)
		&& (packet_in_.src_y != -1));

	count_P_fire++;

	//int PO_x, PO_y;

	if (packet_in_.name == "z_1")
	{
		buffer.push_back(packet_in_);
	}
	else if (packet_in_.name == "z_5")
	{
		packet z1;
		for (std::list<packet>::iterator it = buffer.begin(); it != buffer.end(); ++it)
		{
			if (it->name == "z_1")
			{
				z1 = *it;
				buffer.erase(it);
				break;
			}
		}
		assert(z1.name == "z_1");

		int PO_x = 1;
		int PO_y = 1;

		token_type temp = multi(packet_in_.token, 2);

		token_type token_plus(z1.token.real + temp.real, z1.token.image + temp.image);
		token_type token_minus(z1.token.real - temp.real, z1.token.image - temp.image);

		packet p_plus(x_, y_, PO_x, PO_y, token_plus, "X_2");
		packet p_minus(x_, y_, PO_x, PO_y, token_minus, "X_6");

		out_queue_.push_back(p_plus);
		out_queue_.push_back(p_minus);

		/*std::cout <<"P6 receive " << packet_in_.name << " from (" << packet_in_.src_x << ", "
			<< packet_in_.src_y << ") and " << z1.name << " from (" << z1.src_x << ", " << z1.src_y << ")" << std::endl;*/

	}
	else
	{
		printf("\nsomething wrong, invalid input at PE(0,2)!\n");
	}


	if (queue < out_queue_.size())
		queue = out_queue_.size();

}

void PE_7::execute()
{
	
	// fire the actor if the incoming packet is valid
	if ((packet_in_.src_x != -1)
		&& (packet_in_.src_y != -1))
	{
		fire();
		/*printf("pe7 got the packet %.2f + i%.2f\n",
			packet_in_.token.real, packet_in_.token.image);*/
	}
}

void PE_7::fire()
{
	assert((packet_in_.src_x != -1)
		&& (packet_in_.src_y != -1));

	count_P_fire++;
	//int PO_x, PO_y;

	if (packet_in_.name == "x_2")
	{
		buffer.push_back(packet_in_);
	}
	else if (packet_in_.name == "x_6")
	{
		packet x2;
		for (std::list<packet>::iterator it = buffer.begin(); it != buffer.end(); ++it)
		{
			if (it->name == "x_2")
			{
				x2 = *it;
				buffer.erase(it);
				break;
			}
		}
		assert(x2.name == "x_2");

		token_type token_plus(x2.token.real + packet_in_.token.real, x2.token.image + packet_in_.token.image);
		token_type token_minus(x2.token.real - packet_in_.token.real, x2.token.image - packet_in_.token.image);

		packet p_plus(x_, y_, 0, 1, token_plus, "y_2");
		packet p_minus(x_, y_, 2, 1, token_minus, "y_3");

		out_queue_.push_back(p_plus);
		out_queue_.push_back(p_minus);

		/*std::cout <<"P7 receive " << packet_in_.name << " from (" << packet_in_.src_x << ", "
			<< packet_in_.src_y << ") and " << x2.name << " from (" << x2.src_x << ", " << x2.src_y << ")" << std::endl;*/
	}
	else if (packet_in_.name == "x_3")
	{
		buffer.push_back(packet_in_);
	}
	else if (packet_in_.name == "x_7")
	{
		packet x3;
		for (std::list<packet>::iterator it = buffer.begin(); it != buffer.end(); ++it)
		{
			if (it->name == "x_3")
			{
				x3 = *it;
				buffer.erase(it);
				break;
			}
		}
		assert(x3.name == "x_3");

		token_type token_plus(x3.token.real + packet_in_.token.real, x3.token.image + packet_in_.token.image);
		token_type token_minus(x3.token.real - packet_in_.token.real, x3.token.image - packet_in_.token.image);

		packet p_plus(x_, y_, 0, 1, token_plus, "y_6");
		packet p_minus(x_, y_, 2, 1, token_minus, "y_7");

		out_queue_.push_back(p_plus);
		out_queue_.push_back(p_minus);

		/*std::cout <<"P7 receive " << packet_in_.name << " from (" << packet_in_.src_x << ", "
			<< packet_in_.src_y << ") and " << x3.name << " from (" << x3.src_x << ", " << x3.src_y << ")" << std::endl;*/
	}
	else
	{
		printf("\nsomething wrong, invalid input at PE(1, 2)!\n");
	}


	if (queue < out_queue_.size())
		queue = out_queue_.size();

}

void PE_8::execute()
{
	
	// fire the actor if the incoming packet is valid
	if ((packet_in_.src_x != -1)
		&& (packet_in_.src_y != -1))
	{
		fire();
		/*printf("pe8 got the packet %.2f + i%.2f\n",
			packet_in_.token.real, packet_in_.token.image);*/
	}
}

void PE_8::fire()
{
	assert((packet_in_.src_x != -1)
		&& (packet_in_.src_y != -1));

	count_P_fire++;

	//int PO_x, PO_y;

	if (packet_in_.name == "z_3")
	{
		buffer.push_back(packet_in_);
	}
	else if (packet_in_.name == "z_7")
	{
		packet z3;
		for (std::list<packet>::iterator it = buffer.begin(); it != buffer.end(); ++it)
		{
			if (it->name == "z_3")
			{
				z3 = *it;
				buffer.erase(it);
				break;
			}
		}
		assert(z3.name == "z_3");

		int PO_x = 1;
		int PO_y = 1;
		token_type temp = multi(packet_in_.token, 3);

		token_type token_plus(z3.token.real + temp.real, z3.token.image + temp.image);
		token_type token_minus(z3.token.real - temp.real, z3.token.image - temp.image);

		packet p_plus(x_, y_, PO_x, PO_y, token_plus, "X_3");
		packet p_minus(x_, y_, PO_x, PO_y, token_minus, "X_7");

		out_queue_.push_back(p_plus);
		out_queue_.push_back(p_minus);

		/*std::cout <<"P8 receive " << packet_in_.name << " from (" << packet_in_.src_x << ", "
			<< packet_in_.src_y << ") and " << z3.name << " from (" << z3.src_x << ", " << z3.src_y << ")" << std::endl;*/

	}
	else
	{
		printf("\nsomething wrong, invalid input at PE(0,0)!\n");
	}


	if (queue < out_queue_.size())
		queue = out_queue_.size();

}
