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

 #include <config.h>

 #include <udjat/defs.h>
 #include <private/modules.h>
 #include <private/tools.h>
 #include <udjat/tools/intl.h>
 #include <functional>
 #include <errno.h>

 #include <Python.h>

 namespace Udjat {

	PyObject * Python::call(PyObject *args, const std::function<PyObject *(PyObject *args)> &callback) noexcept {

		try {

			return callback(args);

		} catch(const std::exception &e) {

			PyErr_SetString(PyExc_RuntimeError, e.what());

		} catch(...) {

			PyErr_SetString(PyExc_RuntimeError, _("Unexpected error in python callback."));

		}

		return NULL;
	}

	PyObject * Python::call(int required_args, PyObject *args, const std::function<PyObject *(PyObject *args)> &callback) noexcept {

		if(PyTuple_Size(args) != required_args) {
			PyErr_SetString(PyExc_RuntimeError, strerror(EINVAL));
			return NULL;
		}

		try {

			return callback(args);

		} catch(const std::exception &e) {

			PyErr_SetString(PyExc_RuntimeError, e.what());

		} catch(...) {

			PyErr_SetString(PyExc_RuntimeError, _("Unexpected error in python callback."));

		}

		return NULL;
	}

	PyObject * Python::call(int required_args, PyObject *args, const std::function<void (PyObject *args)> &callback) noexcept {

		if(PyTuple_Size(args) != required_args) {
			PyErr_SetString(PyExc_RuntimeError, strerror(EINVAL));
			return NULL;
		}

		try {

			callback(args);
			Py_RETURN_NONE;

		} catch(const std::exception &e) {

			PyErr_SetString(PyExc_RuntimeError, e.what());

		} catch(...) {

			PyErr_SetString(PyExc_RuntimeError, _("Unexpected error in python callback."));

		}

		return NULL;
	}

 }
