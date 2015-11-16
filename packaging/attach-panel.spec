Name:       attach-panel
Summary:    Attach Panel
Version:    0.1.0
Release:    0
Group:      Application
License:    Apache-2.0
Source0:    %{name}-%{version}.tar.gz

%if "%{?tizen_profile_name}" == "wearable"
ExcludeArch: %{arm} %ix86 x86_64
%endif

BuildRequires: cmake
BuildRequires: edje-tools
BuildRequires: gettext-tools

BuildRequires: pkgconfig(appcore-efl)
BuildRequires: pkgconfig(aul)
BuildRequires: pkgconfig(bundle)
BuildRequires: pkgconfig(capi-base-common)
BuildRequires: pkgconfig(capi-system-info)
BuildRequires: pkgconfig(dlog)
BuildRequires: pkgconfig(ecore)
BuildRequires: pkgconfig(edje)
BuildRequires: pkgconfig(efl-extension)
BuildRequires: pkgconfig(elementary)
BuildRequires: pkgconfig(evas)
BuildRequires: pkgconfig(feedback)
BuildRequires: pkgconfig(isf)
#BuildRequires: pkgconfig(security-privilege-checker)
BuildRequires: pkgconfig(ui-gadget-1)
BuildRequires: pkgconfig(rua)

%description
Description: Attach Panel

%prep
%setup -q

%package devel
Summary:    Attach panel library (devel)
Group:      Application
Requires:   %{name} = %{version}-%{release}

%description devel
Development files needed to build software that needs Attach panel.

%build
export CFLAGS="$CFLAGS -DTIZEN_DEBUG_ENABLE"
export CXXFLAGS="$CXXFLAGS -DTIZEN_DEBUG_ENABLE"
export FFLAGS="$FFLAGS -DTIZEN_DEBUG_ENABLE"

%if 0%{?tizen_build_binary_release_type_eng}
export CFLAGS="$CFLAGS -DTIZEN_ENGINEER_MODE"
export CXXFLAGS="$CXXFLAGS -DTIZEN_ENGINEER_MODE"
export FFLAGS="$FFLAGS -DTIZEN_ENGINEER_MODE"
%endif

%cmake . -DCMAKE_INSTALL_PREFIX=%{_prefix}
make %{?jobs:-j%jobs}

%install
rm -rf %{buildroot}
%make_install

%post

%files
%manifest %{name}.manifest
%{_libdir}/*.so*
%{_datadir}/attach-panel/edje/*.edj
%{_datadir}/attach-panel/sample/*
%{_datadir}/icons/*.png
%{_prefix}/ug/res/locale/*/LC_MESSAGES/*

%files devel
%defattr(-,root,root,-)
%{_includedir}/attach-panel/attach_panel.h
%{_libdir}/pkgconfig/%{name}.pc
