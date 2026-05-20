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

 #pragma once
 #include <Python.h>

 #ifdef HAVE_CONFIG_H
	#include <config.h>
 #endif // HAVE_CONFIG_H

 #include <udjat/defs.h>

 #ifdef __cplusplus

 #include <udjat/action.h>
 #include <private/tools.h>
 #include <cstdint>
 #include <string>
 #include <udjat/tools/request.h>
 #include <udjat/tools/response.h>

 namespace Udjat::Python {

	class UDJAT_API Action : public Udjat::Action {
	public:

		class Factory : public Udjat::Action::Factory {
		public:
			Factory(const char *name);
			~Factory() override;

			std::shared_ptr<Udjat::Action> ActionFactory(const char *pysource, const XML::Node &node) const;
			std::shared_ptr<Udjat::Action> ActionFactory(const XML::Node &node) const override;

		};

		enum Property : uint8_t {
			Name,

			PropertyCount
		};

		Action(PyObject *s, const XML::Node &node);
		~Action() override;

		const char *name() const noexcept override;

		bool setProperty(const char *key, const char *value) override;
		bool getProperty(const char *key, std::string &value) const override;

		int call(Udjat::Request &request, Udjat::Response &response, bool except = true) override;

	private:
		PyObject * self = nullptr;
		std::string properties[PropertyCount];

	};

 };

 extern "C" {
 #endif // __cplusplus

 extern UDJAT_PRIVATE PyTypeObject action_type;

 UDJAT_PRIVATE int		  action_init(PyObject *self, PyObject *args, PyObject *kwds);
 UDJAT_PRIVATE void		  action_finalize(PyObject *self);
 UDJAT_PRIVATE int 		  action_setattr(PyObject *self, PyObject *attr, PyObject *value);
 UDJAT_PRIVATE PyObject * action_getattr(PyObject *self, PyObject *attr);
 UDJAT_PRIVATE PyObject * action_call(PyObject *self, PyObject *args);

 #ifdef __cplusplus
	}
 #endif // __cplusplus
