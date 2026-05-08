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

 #include <private/object.h>
 #include <private/state.h>
 #include <Python.h>

 static PyMethodDef methods[] = {
    {
		.ml_name = "trace",
		.ml_meth = (PyCFunction) object_trace,
		.ml_flags = METH_VARARGS,
		.ml_doc =	"Write trace message to log file\n"
    },
    {
		.ml_name = "error",
		.ml_meth = (PyCFunction) object_error,
		.ml_flags = METH_VARARGS,
		.ml_doc =	"Write error message to log file\n"
    },
    {
		.ml_name = "warning",
		.ml_meth = (PyCFunction) object_warning,
		.ml_flags = METH_VARARGS,
		.ml_doc =	"Write warning message to log file\n"
    },
    {
		.ml_name = "info",
		.ml_meth = (PyCFunction) object_info,
		.ml_flags = METH_VARARGS,
		.ml_doc =	"Write informational message to log file\n"
    },
	/*
    {
		.ml_name = "setup",
		.ml_meth = (PyCFunction) state_setup,
		.ml_flags = METH_VARARGS,
		.ml_doc =	"Setup agent from properties object\n\n"
					"setup(properties): Apply 'properties' on agent\n"
    },
	*/

    {
    	NULL
	}
 };

 PyTypeObject state_type = {

	PyVarObject_HEAD_INIT(NULL, 0)

	.tp_name = "udjat.State",
	.tp_doc = "UDJAT State Object",
	.tp_basicsize = sizeof(pyAbstractObject),
	.tp_itemsize = 0,
	.tp_flags = Py_TPFLAGS_HAVE_FINALIZE|Py_TPFLAGS_DEFAULT|Py_TPFLAGS_BASETYPE,

	.tp_new = state_alloc,
	.tp_dealloc = state_dealloc,

	.tp_init = state_init,
	.tp_finalize = state_finalize,

	.tp_str = object_str,
	.tp_getattro = object_getattr,
	.tp_setattro = object_setattr,

	.tp_methods = methods,

 };

