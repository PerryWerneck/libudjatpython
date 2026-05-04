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

 #ifdef HAVE_CONFIG_H
	 #include <config.h>
 #endif // HAVE_CONFIG_H

 #include <udjat/defs.h>
 #include <udjat/tools/configuration.h>
 #include <udjat/tools/logger.h>
 #include <private/tools.h>
 #include <stdexcept>

 #include <Python.h>

 using namespace Udjat;
 using namespace std;

 static void cleanup(PyObject *module);

 static PyObject *get_string(PyObject *self, PyObject *args) noexcept;
 static PyObject *get_value(PyObject *self, PyObject *args) noexcept;

 static PyMethodDef methods[] = {
	{
		"get",
		get_string,
		METH_VARARGS,
		"Get value from configuration file"
	},

	{
		"get_string",
		get_string,
		METH_VARARGS,
		"Get string from configuration file"
	},

	{
		"get_long",
		get_value,
		METH_VARARGS,
		"Get long value from configuration file"
	},

	{NULL, NULL, 0, NULL}        // Sentinel
 };	

 static struct PyModuleDef module = {
	PyModuleDef_HEAD_INIT,
	.m_name = "config",					// name of module
	.m_doc = NULL,						// module documentation, may be NUL
	.m_size = 0,						// size of per-interpreter state of the module or -1 if the module keeps state in global variables.
	.m_methods = methods,				// Module methods
	.m_free = (freefunc) cleanup
 };	

 PyMODINIT_FUNC PyInit_config(void) {
	debug("----- Registering module '",module.m_name,"'");
	return PyModuleDef_Init(&module);
 }
 
 void cleanup(PyObject *) {
 } 

 PyObject *get_string(PyObject *self, PyObject *args) noexcept {

	return Python::call(args,[](PyObject *args) -> PyObject * {

		const char *group ="python";
		const char *name = NULL;
		const char *def = NULL;

		switch(PyTuple_Size(args)) {
		case 1:
			if(!PyArg_ParseTuple(args, "s", &name)) {
				throw runtime_error("Invalid argument");
			}
			break;

		case 2:
			if(!PyArg_ParseTuple(args, "ss", &group,&name)) {
				throw runtime_error("Invalid argument");
			}
			break;

		case 3:
			if(!PyArg_ParseTuple(args, "sss", &group,&name,&def)) {
				throw runtime_error("Invalid argument");
			}
			break;

		default:
			throw runtime_error("Invalid argument");

		}

		return PyUnicode_FromString(
			Config::get(group,name,def).c_str()
		);

	});
 }

 PyObject *get_value(PyObject *self, PyObject *args) noexcept {

	return Python::call(args,[](PyObject *args) -> PyObject * {

		const char *group ="python";
		const char *name = NULL;
		const char *def = "0";

		switch(PyTuple_Size(args)) {
		case 1:
			if(!PyArg_ParseTuple(args, "s", &name)) {
				throw runtime_error("Invalid argument");
			}
			break;

		case 2:
			if(!PyArg_ParseTuple(args, "ss", &group,&name)) {
				throw runtime_error("Invalid argument");
			}
			break;

		case 3:
			if(!PyArg_ParseTuple(args, "sss", &group,&name,&def)) {
				throw runtime_error("Invalid argument");
			}
			break;

		default:
			throw runtime_error("Invalid argument");

		}
		
		return PyLong_FromLong(
			Config::get(group,name,atoi(def))
		);

	});
 }
