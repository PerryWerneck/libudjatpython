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

 #include <Python.h>

 static void cleanup(PyObject *module);
 static PyObject *debug_method(PyObject *self, PyObject *args);
 static PyObject *trace_method(PyObject *self, PyObject *args);
 static PyObject *error_method(PyObject *self, PyObject *args);
 static PyObject *warning_method(PyObject *self, PyObject *args);
 static PyObject *info_method(PyObject *self, PyObject *args);

 static PyMethodDef methods[] = {
	{
		"debug",
		debug_method,
		METH_NOARGS,
		"Write debug message to the log"
	},

	{
		"trace",
		trace_method,
		METH_VARARGS,
		"Write trace message to the log"
	},

	{
		"error",
		error_method,
		METH_VARARGS,
		"Write error message to the log"
	},

	{
		"warning",
		warning_method,
		METH_VARARGS,
		"Write warning message to the log"
	},

	{
		"info",
		info_method,
		METH_VARARGS,
		"Write information message to the log"
	},


	{NULL, NULL, 0, NULL}        // Sentinel
 };	

 static struct PyModuleDef logger_module = {
	PyModuleDef_HEAD_INIT,
	.m_name = "logger",					// name of module
	.m_doc = NULL,						// module documentation, may be NUL
	.m_size = -1,						// size of per-interpreter state of the module or -1 if the module keeps state in global variables.
	.m_methods = methods,				// Module methods
	.m_free = (freefunc) cleanup
 };	

 PyMODINIT_FUNC PyInit_logger(void) {
	return PyModuleDef_Init(&logger_module);
 }
 
 void cleanup(PyObject *) {
 } 

 PyObject *debug_method(PyObject *self, PyObject *args) {
	return Python::call(1,args,[](PyObject *args){
		const char *msg = "";
		if (!PyArg_ParseTuple(args, "s", &msg))
            throw throw system_error(ENOENT,system_category(),_("The log message should be a string"));
		Logger::Message{msg}.write(Logger::Debug);
	});
 }

 PyObject *trace_method(PyObject *self, PyObject *args) {
	return Python::call(1,args,[](PyObject *args){
		const char *msg = "";
		if (!PyArg_ParseTuple(args, "s", &msg))
            throw throw system_error(ENOENT,system_category(),_("The log message should be a string"));
		Logger::Message{msg}.trace();
	});
 }

 PyObject *error_method(PyObject *self, PyObject *args) {
	return Python::call(1,args,[](PyObject *args){
		const char *msg = "";
		if (!PyArg_ParseTuple(args, "s", &msg))
            throw throw system_error(ENOENT,system_category(),_("The log message should be a string"));
		Logger::Message{msg}.error();
	});
 }

 PyObject *warning_method(PyObject *self, PyObject *args) {
	return Python::call(1,args,[](PyObject *args){
		const char *msg = "";
		if (!PyArg_ParseTuple(args, "s", &msg))
            throw throw system_error(ENOENT,system_category(),_("The log message should be a string"));
		Logger::Message{msg}.warning();
	});
 }

 PyObject *info_method(PyObject *self, PyObject *args) {
	return Python::call(1,args,[](PyObject *args){
		const char *msg = "";
		if (!PyArg_ParseTuple(args, "s", &msg))
            throw throw system_error(ENOENT,system_category(),_("The log message should be a string"));
		Logger::Message{msg}.info();
	});
 }
