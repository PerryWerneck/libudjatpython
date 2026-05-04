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

 /// @brief Declare python interpreter class.

 #pragma once
 #ifdef HAVE_CONFIG_H
	 #include <config.h>
 #endif // HAVE_CONFIG_H

 #include <udjat/defs.h>
 #include <mutex>
 #include <functional>

 #include <Python.h>
 
 namespace Udjat {

 	namespace Python {

		/// @brief The python interpreter
		class UDJAT_API Interpreter {
		private:

			std::recursive_mutex guard;

			PyConfig config;
			PyStatus status;

			Interpreter();

		public:

			static Interpreter & getInstance();
		
			~Interpreter();

			int run(const char *name, const char *script_text);	
			int run(const char *name, const char *script_text, const std::function<bool(uint64_t current, uint64_t total, const void *data, size_t len)> &progress);

			inline int run(const char *script_text) {
				return run(nullptr,script_text);
			}	

			inline int run(const char *script_text, const std::function<bool(uint64_t current, uint64_t total, const void *data, size_t len)> &progress) {
				return run(nullptr,script_text,progress);				
			}
			
		};

	}

 }
