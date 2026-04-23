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

 /**
  * @brief Declare python interpreter singleton.
  */


 #pragma once
 #include <config.h>
 #include <udjat/defs.h>
 #include <private/interpreter.h>
 #include <mutex>

 #define PY_SSIZE_T_CLEAN
 #include <Python.h>

 namespace Udjat {

 	namespace Python {

		/// @brief The python interpreter
		class UDJAT_API Interpreter : private std::recursive_mutex {
		private:
			PyStatus status;
			PyConfig config;

			Interpreter();
			~Interpreter();

		public:
			static Interpreter & Instance();

			int run(const char *script_text);
			
		};

	}

 }
