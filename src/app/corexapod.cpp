/*
 * hexapod.cpp
 *
 *  Created on: Jul 6, 2014
 *      Author: core
 */

#include <csignal>
#include <boost/bind.hpp>
#include <boost/program_options.hpp>

#include "daemon.hpp"
#include "engine.hpp"
#include "parser.hpp"

namespace po = boost::program_options;

hex::Hexapod hexapod("/dev/ttyAMA0", "src/calib.param");
hex::Parser p("src/actions.as", hexapod);
hex::Daemon d(50000, p);
bool finish = false;

void signalHandler(int sig) {
	std::cout << "Terminated." << std::endl;
	p.finish_ = true;
	finish = true;
}

int main(int argc, char **argv) {
	po::options_description desc("Allowed options");
	desc.add_options()
	    ("help", "produce help message")
	    ("calibrate,C", "calibrate servos")
	    ("debug-middle", "enter all servo 1500 position")
	;
	po::variables_map vm;
	po::store(po::parse_command_line(argc, argv, desc), vm);
	po::notify(vm);
	
	std::signal(SIGINT, signalHandler); // add support for destructor

	if (vm.count("help")) {
	    std::cout << desc << "\n";
	    return 1;
	} else if (vm.count("calibrate")) {
		std::cout << "Entering interactive calibrate mode:\n";
		hexapod.calibrate();
	} else if (vm.count("debug-middle")) {
		std::cout << "Setting all servos to mid position\n";
		hexapod.allServoMidPositon();
		while(!finish)
		    sleep(1);
	} else {
		d.spin();
	}
	return 0;
}
