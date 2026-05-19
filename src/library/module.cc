/* SPDX-License-Identifier: LGPL-3.0-or-later */

/*
 * Copyright (C) 2026 Perry Werneck <perry.werneck@gmail.com>
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

 #ifdef HAVE_CONFIG_H
	 #include <config.h>
 #endif // HAVE_CONFIG_H

 #include <udjat/defs.h>
 #include <udjat/module/abstract.h>
 #include <udjat/tools/logger.h>
 #include <udjat/tools/url.h>
 #include <udjat/tools/url/handler/python.h>
 #include <udjat/module/python.h>
 #include <private/modules.h>
 #include <private/agent.h>
 #include <private/tools.h>
 #include <mutex>

 using namespace std;

 namespace Udjat {

	Udjat::Module * Python::Module::Factory(const char *name) {

		class Module : public Python::Module, private Python::Handler::Factory, private Python::Agent::Factory {
		public:
			Module(const char *name, const char *description) : Python::Module{name,description} {
			}

			virtual ~Module() {
			}

		};

		return new Module(name,"Python " PY_VERSION " module");
	}

	Python::Module::Module(const char *name, const char *description) 
		: Udjat::Module(name,(description ? description : "Python " PY_VERSION " module")) {

		Logger::String{"Initializing python " PY_VERSION " module"}.trace(name);			
	}

	Python::Module::~Module() {
	}

 }

