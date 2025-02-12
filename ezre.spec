Name:           edcre
Version:        1.0.9
Summary:        Your description.
Release:        1%{?dist}
License:        GNU GPLv2
URL:            https://alex-free.github.io/ezre
Packager:       Alex Free

%description
EDC/ECC regenerator for edited CD images.

%install
mkdir -p %{buildroot}/usr/bin
cp %{_sourcedir}/edcre %{buildroot}/usr/bin/

%files
/usr/bin/edcre
