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
 #include <private/modules.h>

 static void cleanup(PyObject *module);

/*---[ Globals ]------------------------------------------------------------------------------------*/

PyObject *get_module_version(PyObject *self, PyObject *args) {PyObject *get_module_revision(PyObject *self, PyObject *args) {
	return PyUnicode_FromString(PACKAGE_REVISION);
}

	return PyUnicode_FromString(PACKAGE_VERSION);
}

PyObject *get_core_version(PyObject *self, PyObject *args) {
	return PyUnicode_FromString("incomplete");
}

static PyMethodDef methods[] = {

	{
		"version",
		get_module_version,
		METH_NOARGS,
		"Get package version"
	},

	{
		"revision",
		get_module_revision,
		METH_NOARGS,
		"Get package revision"

	},


	{
		"CORE",
		get_core_version,
		METH_NOARGS,
		"Get package revision"

	},

	{
		NULL,
		NULL,
		0,
		NULL
	}

};

static struct PyModuleDef definition = {
	PyModuleDef_HEAD_INIT,
	.m_name = "udjat",					// name of module
	.m_doc = NULL,						// module documentation, may be NUL
	.m_size = -1,						// size of per-interpreter state of the module or -1 if the module keeps state in global variables.
	.m_methods = methods,				// Module methods
	.m_free = (freefunc) cleanup
};

PyMODINIT_FUNC PyInit_udjat(void)
{
    PyObject *module = PyModule_Create(&definition);

    if (module) {
        // Add internal modules as submodules of 'udjat'
        PyModule_AddObject(module, "logger", PyInit_logger());
        PyModule_AddObject(module, "config", PyInit_config());
    }

    return module;
}

static void cleanup(PyObject *module) {


}
