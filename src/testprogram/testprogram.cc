/* SPDX-License-Identifier: LGPL-3.0-or-later */

/*
 * Copyright (C) 2021 Perry Werneck <perry.werneck@gmail.com>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published
 * by the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */

 #include <Python.h>

 #include <config.h>
 #include <udjat/defs.h>
 #include <udjat/loader.h>
 #include <udjat/tools/logger.h>
 #include <udjat/module.h>
 #include <iostream>
 #include <private/interpreter.h>
 #include <private/agent.h>
 #include <private/modules.h>
 #include <udjat/tools/memory.h>

 using namespace Udjat;
 using namespace std;

 int main(int argc, char **argv) {

	Logger::verbosity(9);
	Logger::redirect();

	Python::Interpreter::getInstance().run(
		"import sys; sys.path.append('./testscripts')"
	);

	/*
	return Python::Interpreter::getInstance().run(
		"import sys\n"
		"import logger\n"
		"import config\n"
		"logger.warning(f'----> Program name: {sys.executable}')\n"
		"print(config.get('python','example','default'))\n"
	);
	*/


	auto agent = Python::Agent::Factory("testagent");
	agent->setup(XML::Node());
	agent->start();
	agent->refresh();
	agent->stop();

	/*
	return loader(argc,argv,[](Application &app) -> int {

		udjat_module_init();

		return 0;
	});
	*/

 }



