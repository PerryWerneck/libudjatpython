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
 #include <udjat/tools/url/handler/python.h>
 #include <private/interpreter.h>
 #include <udjat/tools/url.h>
 #include <udjat/tools/file/text.h>
 #include <udjat/tools/logger.h>

 #include <Python.h>

 namespace Udjat {

	Python::Handler::Factory::Factory(const char *name) : Udjat::URL::Handler::Factory{name} {
	}

	Python::Handler::Factory::~Factory() {
	}

	std::shared_ptr<Udjat::URL::Handler> Python::Handler::Factory::HandlerFactory(const URL &url) const {		
		return std::make_shared<Python::Handler>(url);
	}

	Python::Handler::Handler(const URL &u) : url{u} {

	}

	Python::Handler::~Handler() {
	}

	const char * Python::Handler::c_str() const noexcept {
		return url.c_str();
	}

	int Python::Handler::test(const HTTP::Method, const char *) {

		int rc = 0;

		try {

			File::Text file{url.path().c_str()};
			rc = Python::Interpreter::getInstance().run(file.c_str());

		} catch(const std::exception &e) {

			Logger::String{e.what()}.error();
			rc = -1;

		} catch(...) {

			Logger::String{"Unexpected error running python script"}.error();
			rc = -1;
		}

		return rc;
	}

	int Python::Handler::perform(const HTTP::Method, const char *, const std::function<bool(uint64_t current, uint64_t total, const void *data, size_t len)> &progress) {

		int rc = 0;

		try {

			File::Text file{url.path().c_str()};
			rc = Python::Interpreter::getInstance().run(file.c_str(),progress);

		} catch(const std::exception &e) {

			Logger::String{e.what()}.error();
			rc = -1;

		} catch(...) {

			Logger::String{"Unexpected error running python script"}.error();
			rc = -1;
		}

		return rc;


	}

 }

