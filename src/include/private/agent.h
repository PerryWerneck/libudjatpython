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

 #include <udjat/agent/abstract.h>
 #include <private/tools.h>
 #include <private/state.h>
 #include <udjat/tools/value.h>

 namespace Udjat::Python {

	class UDJAT_API Agent : public Udjat::Abstract::Agent {
	protected:

		std::shared_ptr<Abstract::State> computeState() override;
		Udjat::Value & get(Udjat::Value &value) const override;

	public:

		class UDJAT_API Factory : public Abstract::Agent::Factory {
		public:
			Factory(const char *name = "python");
			~Factory() override;
			std::shared_ptr<Abstract::Agent> AgentFactory(const XML::Node &node) const override;

		};

		static std::shared_ptr<Abstract::Agent> Factory(const char *pysource, const XML::Node &node);
		static std::shared_ptr<Abstract::Agent> Factory(const char *pysource);

		inline operator PyObject *() const noexcept {
			return value;
		}

		bool refresh(bool ondemand) override;

		std::string to_string() const noexcept override;

		bool assign(const char *value) override;

		std::shared_ptr<Abstract::State> StateFactory(const XML::Node &node) override;
	
		void set_value(PyObject *value);

		inline bool operator==(PyObject *obj) const {
			return Python::compare(this->value,obj);
		}

		inline bool operator!=(PyObject *obj) const {
			return !Python::compare(this->value,obj);
		}

		inline void failed(const char *summary, const char *body) noexcept {
			Udjat::Abstract::Agent::failed(summary,body);
		}

		inline void rename(const char *value) {
			Udjat::Abstract::Agent::rename(value);
		}

		inline void label(const char *value) {
			properties.label = value;
		}

		inline void summary(const char *value) {
			properties.summary = value;
		}

		inline void url(const char *value) {
			properties.url = value;
		}

		inline void icon(const char *value) {
			properties.icon = value;
		}
	
		bool setup(const XML::Node &node) override;

	private:
		PyObject * self = nullptr;
		PyObject * value = nullptr;

		/// @brief Agent states.
		std::vector<std::shared_ptr<Python::State>> states;

		Agent(const char *pysource,const XML::Node &node);
		~Agent() override;

	};

 };

 extern "C" {
 #endif // __cplusplus

 extern UDJAT_PRIVATE PyTypeObject agent_type;

 UDJAT_PRIVATE int		  agent_init(PyObject *self, PyObject *args, PyObject *kwds);
 UDJAT_PRIVATE void		  agent_finalize(PyObject *self);
 UDJAT_PRIVATE int 		  agent_setattr(PyObject *self, PyObject *attr, PyObject *value);
 UDJAT_PRIVATE PyObject * agent_getattr(PyObject *self, PyObject *attr);

 UDJAT_PRIVATE PyObject * agent_setup(PyObject *self, PyObject *args);
 UDJAT_PRIVATE PyObject * agent_failed(PyObject *self, PyObject *args);
 UDJAT_PRIVATE PyObject * agent_get_by_path(PyObject *self, PyObject *args);
 UDJAT_PRIVATE PyObject * agent_invalidate(PyObject *self, PyObject *args);
 UDJAT_PRIVATE PyObject * agent_start(PyObject *self, PyObject *args);
 UDJAT_PRIVATE PyObject * agent_stop(PyObject *self, PyObject *args);
 UDJAT_PRIVATE PyObject * agent_refresh(PyObject *self, PyObject *args);

 #ifdef __cplusplus
	}
 #endif // __cplusplus
