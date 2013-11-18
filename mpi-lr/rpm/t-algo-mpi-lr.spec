Name: t-algo-mpi-lr
Version:1.0.0
Release: %(echo $RELEASE)%{?dist}
Summary: mpi lr scripts
Group: Search/Algo
URL:  http://gitlab.taobao.ali.com/shubao.sx/MPI-LR
Packager:shubao.sx
License: commercial
BuildRoot: %{BUILD}/%{name}-%{version}-%{release}
#BuildArch: noarch

AutoReq: no 

BuildRequires: t-algo-openmpi-devel >= 1.0.13
BuildRequires: algo-common-devel >= 0.0.26

Requires: t-algo-openmpi-devel >= 1.0.13
Requires: t_algo_db_common >= 0.0.21

 
%description
CodeUrl:http://gitlab.taobao.ali.com/shubao.sx/MPI-LR

%prep
%preun

%build

%install
mkdir -p ${RPM_BUILD_ROOT}%{_prefix}/

find $OLDPWD  -name '.svn' | xargs rm -rf

#mkdir ${RPM_BUILD_ROOT}%{_prefix}/bin/
#mkdir ${RPM_BUILD_ROOT}%{_prefix}/conf/
#mkdir ${RPM_BUILD_ROOT}%{_prefix}/example/
#mkdir ${RPM_BUILD_ROOT}%{_prefix}/lib/
#
cp -rf $OLDPWD/../bin/      ${RPM_BUILD_ROOT}%{_prefix}
cp -rf $OLDPWD/../conf/     ${RPM_BUILD_ROOT}%{_prefix}
cp -rf $OLDPWD/../dist/   ${RPM_BUILD_ROOT}%{_prefix}/lib
cp -rf $OLDPWD/../example/   ${RPM_BUILD_ROOT}%{_prefix}/


%pre

%post
chmod 755 %{_prefix}/bin/*
ln -sf %{_prefix}/bin/run-lr  /usr/bin/run-lr
ln -sf %{_prefix}/bin/run-lr-auc  /usr/bin/run-lr-auc

%postun

%clean

%files
%defattr(-,ads,users)
%{_prefix}
%config(noreplace)     %{_prefix}/conf/env.inc


%changelog
* Thu May  7 2013 shubao.sx <shubao.sx@taobao.com>
- 1.0.0: init.

