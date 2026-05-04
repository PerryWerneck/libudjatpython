/* SPDX-License-Identifier: LGPL-3.0-or-later */

/*
 * Copyright (C) 2025 Perry Werneck <perry.werneck@gmail.com>
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


 #ifdef HAVE_CONFIG_H
	 #include <config.h>
 #endif // HAVE_CONFIG_H

 #include <udjat/defs.h>
 #include <udjat/loader.h>
 #include <udjat/tools/logger.h>
 #include <iostream>
 #include <private/interpreter.h>

 using namespace Udjat;
 using namespace std;

 #ifdef DEBUG 

 static int call_python_test() {
	return Python::Interpreter::getInstance().run(
		"import sys\n"
		"import logger\n"
		"import config\n"
		"logger.warning(f'----> Program name: {sys.executable}')\n"
		"print(config.get('python','example','default'))\n"
	);
 }

 UDJAT_API int run_udjat_unit_test(const char *name) {

	static const struct {
		const char *name;
		int (*test)();
	} tests[] = {
		{"call_python",call_python_test},
	};

	Logger::String{"Running unit test: ",name}.info();

	if(!name) {
		for(const auto &test : tests) {
			Logger::String{"Running unit test: ",test.name}.info();
			test.test();
		}
	} else {
		for(const auto &test : tests) {
			if(strcasecmp(test.name, name) == 0) {
				Logger::String{"Running unit test: ",test.name}.info();
				return test.test();
			}
		}
	}

	return 0;

 }
 #endif // DEBUG
