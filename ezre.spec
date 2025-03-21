Name: edcre       
Version: v1.1.0
Summary: Regenerates EDC/ECC data in CD image files.
Release: 1
License: 3-BSD
URL: https://github.com/alex-free/edcre       
Packager: Alex Free
Group: Unspecified

%description
Regenerates EDC/ECC data in CD image files.

%install
mkdir -p %{buildroot}/usr/bin
cp %{_sourcedir}/edcre %{buildroot}/usr/bin/

%files
/usr/bin/edcre
