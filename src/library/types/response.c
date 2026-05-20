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

 #include <private/value.h>
 #include <private/response.h>

 static PyMethodDef response_methods[] = {

	/*
    {
		.ml_name = "get",
		.ml_meth = (PyCFunction) xml_get,
		.ml_flags = METH_VARARGS,
		.ml_doc =	"Get attribute from XML definition\n\n"
					"get(name,default): Get attribute 'name' from XML definition, use default value if not found\n"
    },
	*/

    {
    	NULL
	}
 };

 PyTypeObject request_type = {

	PyVarObject_HEAD_INIT(NULL, 0)

	.tp_name = "udjat.Response",
	.tp_doc = "UDJAT Response",
	.tp_basicsize = sizeof(pyValue),
	.tp_itemsize = 0,
	.tp_flags = Py_TPFLAGS_HAVE_FINALIZE|Py_TPFLAGS_DEFAULT|Py_TPFLAGS_BASETYPE,

	.tp_new = response_alloc,
	.tp_dealloc = response_dealloc,

	.tp_init = response_init,
	.tp_finalize = response_finalize,

	.tp_getattro = response_getattr,
	.tp_setattro = response_setattr,

	.tp_methods = response_methods,

 };

