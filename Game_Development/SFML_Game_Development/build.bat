@echo off

set root_path=%~dp0

cd %root_path%

if "%1"=="" (
	echo Build type not specified, using Release
	set build_type=Release
) else (
	set build_type=%1
)

if not %build_type%==Release if not %build_type%==Debug (
	echo Invalid build type %build_type%, only Release and Debug are allowed
	exit -1
)


if exist build\%build_type%\ (
	echo build\%build_type% already exists, skipping call to cmake
) else (
	cmake . -DCMAKE_BUILD_TYPE=%build_type% -B build\%build_type%
)
cd build\%build_type%
MSBuild ALL_BUILD.vcxproj -property:Configuration=%build_type% /m
