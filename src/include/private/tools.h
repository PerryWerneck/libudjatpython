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
 #include <functional>
 #include <mutex>
 #include <memory>
 #include <string>

 namespace Udjat::Python {

	extern UDJAT_PRIVATE std::recursive_mutex guard;

	std::shared_ptr<PyObject> make_handle(PyObject *self);

	/// @brief Retrieve last exception.
	/// @param write_to_log Write the error to logfile if true
	/// @return The exception message.
	UDJAT_PRIVATE std::string exception(bool write_to_log = true);

	UDJAT_PRIVATE PyObject * call(const std::function<PyObject *(void)> &callback) noexcept;

	UDJAT_PRIVATE PyObject * call(PyObject *self, const char *method_name);
	UDJAT_PRIVATE PyObject * call(PyObject *self, const char *method_name, PyObject *arg, ...) __attribute__((__sentinel__));

	UDJAT_PRIVATE PyObject * call(PyObject *self,const std::function<PyObject *(Abstract::Object *object)> &callback);

	/// @brief Run callback, convert exception in python errors.
	/// @param args The arguments for the callback.
	/// @param callback The method to call.
	/// @return The callback return.
	UDJAT_PRIVATE PyObject * call(PyObject *args, const std::function<PyObject *(PyObject *args)> &callback) noexcept;

	/// @brief Check argument count, run callback, convert exception in python errors.
	/// @param args The arguments for the callback.
	/// @param callback The method to call.
	/// @return The callback return.
	UDJAT_PRIVATE PyObject * call(int required_args, PyObject *args, const std::function<PyObject *(PyObject *args)> &callback) noexcept;

	UDJAT_PRIVATE PyObject * call(int required_args, PyObject *args, const std::function<PyObject *(PyObject *args)> &callback) noexcept;
	UDJAT_PRIVATE PyObject * call(int required_args, PyObject *args, const std::function<void (PyObject *args)> &callback) noexcept;

	UDJAT_PRIVATE std::string to_string(PyObject *value) noexcept;
	UDJAT_PRIVATE bool compare(PyObject *a, PyObject *b);


 }

 
 