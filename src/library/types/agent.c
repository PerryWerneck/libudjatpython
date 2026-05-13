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
 #include <private/agent.h>
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
	{
		.ml_name = "failed",
		.ml_meth = (PyCFunction) agent_failed,
		.ml_flags = METH_VARARGS,
		.ml_doc =	"Set agent to failed state\n\n"
					"failed(message): Set message for failed state\n"
    },
    {
		.ml_name = "get_by_path",
		.ml_meth = (PyCFunction) agent_get_by_path,
		.ml_flags = METH_VARARGS,
		.ml_doc =	"Get agent by path\n\n"
					"get_by_path(path): Get agent by path\n"
    
    },
    {
		.ml_name = "invalidate",
		.ml_meth = (PyCFunction) agent_invalidate,
		.ml_flags = METH_VARARGS,
		.ml_doc =	"Schedule agent update\n\n"
					"invalidate(seconds): Schedule agent update for 'seconds'\n"
    
    },
    {
		.ml_name = "start",
		.ml_meth = (PyCFunction) object_return_none,
		.ml_flags = METH_NOARGS,
		.ml_doc =	"Start agent\n"
    },
    {
		.ml_name = "stop",
		.ml_meth = (PyCFunction) object_return_none,
		.ml_flags = METH_NOARGS,
		.ml_doc =	"Stop agent\n"
    },
    {
		.ml_name = "refresh",
		.ml_meth = (PyCFunction) object_return_none,
		.ml_flags = METH_VARARGS,
		.ml_doc =	"Refresh agent\n"
    },
    {
		.ml_name = "setup",
		.ml_meth = (PyCFunction) agent_setup,
		.ml_flags = METH_VARARGS,
		.ml_doc =	"Setup agent from properties object\n\n"
					"setup(properties): Apply 'properties' on agent\n"
    },

    {
    	NULL
	}
 };

 PyTypeObject agent_type = {

	PyVarObject_HEAD_INIT(NULL, 0)

	.tp_name = "udjat.Agent",
	.tp_doc = "UDJAT Agent Object",
	.tp_basicsize = sizeof(pyAbstractObject),
	.tp_itemsize = 0,
	.tp_flags = Py_TPFLAGS_HAVE_FINALIZE|Py_TPFLAGS_DEFAULT|Py_TPFLAGS_BASETYPE,

	.tp_new = object_alloc,
	.tp_dealloc = object_dealloc,

	.tp_init = agent_init,
	.tp_finalize = agent_finalize,

	.tp_str = object_str,
	.tp_getattro = agent_getattr,
	.tp_setattro = agent_setattr,

	.tp_methods = methods,

 };

