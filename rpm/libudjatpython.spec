#
# spec file for package libudjatpython
#
# Copyright (c) <2026> Perry Werneck <perry.werneck@gmail.com>.
#
# All modifications and additions to the file contributed by third parties
# remain the property of their copyright owners, unless otherwise agreed
# upon. The license for this file, and modifications and additions to the
# file, is the same license as for the pristine package itself (unless the
# license for the pristine package is not an Open Source License, in which
# case the license is the MIT License). An "Open Source License" is a
# license that conforms to the Open Source Definition (Version 1.9)
# published by the Open Source Initiative.

# Please submit bugfixes or comments via https://github.com/PerryWerneck/libudjatpython/issues
#

%define module_name python

Summary:		Python bindings for %{udjat_product_name}  
Name:			libudjat%{module_name}
Version:		1.0.0
Release:		0
License:		LGPL-3.0
Source:			%{name}-%{version}.tar.xz

URL:			https://github.com/PerryWerneck/libudjat%{module_name}

Group:			Development/Libraries/C and C++
BuildRoot:		/var/tmp/%{name}-%{version}

BuildRequires:	meson
BuildRequires:	gcc-c++ 

BuildRequires:	pkgconfig(libudjat) >= 2.5.0
BuildRequires:	udjat-rpm-macros 

%description
Python bindings for %{udjat_product_name}

C++ HTTP client classes for use with %{udjat_product_name}

%package -n %{udjat_library}
Summary:	Add python support for %{product_name}

%description -n %{udjat_library}
Python bindings for %{product_name}

C++ HTTP client classes for use with lib%{product_name}

%package devel
Summary:	Development files for %{name}
%udjat_devel_requires

%description devel
Python bindings for %{product_name}

C++ classes to add python support from lib%{product_name}

%lang_package -n %{udjat_library}
%udjat_module_package -n %{module_name}

#---[ Build & Install ]-----------------------------------------------------------------------------------------------

%prep
%autosetup
%meson

%build
%meson_build

%install
%meson_install
%find_lang %{name}-%{udjat_package_major}.%{udjat_package_minor} langfiles

%files -n %{udjat_library}
%defattr(-,root,root)
%{_libdir}/%{name}.so.%{udjat_package_major}.%{udjat_package_minor}

%files -n %{udjat_library}-lang -f langfiles

%files devel
%defattr(-,root,root)

%{_libdir}/*.so
%{_libdir}/*.a
%{_libdir}/pkgconfig/*.pc

%post -n %{udjat_library} -p /sbin/ldconfig

%postun -n %{udjat_library} -p /sbin/ldconfig

%changelog

