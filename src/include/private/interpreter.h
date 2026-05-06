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
 #include <Python.h>
 
 #ifdef HAVE_CONFIG_H
	 #include <config.h>
 #endif // HAVE_CONFIG_H

 #include <udjat/defs.h>
 #include <mutex>
 #include <functional>
 #include <memory>

 namespace Udjat {

 	namespace Python {

		std::shared_ptr<PyObject> make_handle(PyObject *self);

		/// @brief The python interpreter
		class UDJAT_API Interpreter : public std::recursive_mutex {
		private:

			PyConfig config;
			PyStatus status;

			Interpreter();

		public:

			static Interpreter & getInstance();

			static void PyDecRef(PyObject *object);
		
			~Interpreter();

			int run(const char *name, const char *script_text);	
			int run(const char *name, const char *script_text, const std::function<bool(uint64_t current, uint64_t total, const void *data, size_t len)> &progress);

			inline int run(const char *script_text) {
				return run(nullptr,script_text);
			}	

			inline int run(const char *script_text, const std::function<bool(uint64_t current, uint64_t total, const void *data, size_t len)> &progress) {
				return run(nullptr,script_text,progress);				
			}
			
			/// @brief Import python source.
			/// @param pysource The python source to import.
			/// @return Object for the loaded source.
			PyObject * import(const char *pysource);

			/// @brief Import python module
			/// @param module_name The module name to import.
			/// @return Object for the loaded module.
			PyObject * module(const char *module_name);

			/// @brief Retrieve last exception.
			/// @param write_to_log Write the error to logfile if true
			/// @return The exception message.
			std::string exception(bool write_to_log = true);

			PyObject * factory(const char *pysource, const char *method, const XML::Node &node);
	
			std::shared_ptr<PyObject> factory(const Udjat::XML::Node &node);


		};

	}

 }
