# edcre GNUMakefile by Alex Free
CXX=g++
CXX_FLAGS=-Wall -Werror -Ofast
VER=1.0.3

edcre: clean
	$(CXX) $(CXX_FLAGS) lec.cc -o edcre

clean:
	rm -rf edcre.exe edcre

linux-x86:
	make edcre CXX_FLAGS="-m32 -static -Wall -Werror -Ofast"

linux-x86_64:
	make edcre CXX_FLAGS="-static -Wall -Werror -Ofast"

windows-x86:
	make edcre CXX="i686-w64-mingw32-g++"

windows-x86_64:
	make edcre CXX="x86_64-w64-mingw32-g++"

linux-release:
	rm -rf edcre-$(VER)-$(PLATFORM) edcre-$(VER)-$(PLATFORM).zip
	mkdir edcre-$(VER)-$(PLATFORM)
	cp -rv edcre images readme.md license.txt edcre-$(VER)-$(PLATFORM)
	chmod -R 777 edcre-$(VER)-$(PLATFORM)
	zip -r edcre-$(VER)-$(PLATFORM).zip edcre-$(VER)-$(PLATFORM)
	rm -rf edcre-$(VER)-$(PLATFORM)

windows-release:
	rm -rf edcre-$(VER)-$(PLATFORM) edcre-$(VER)-$(PLATFORM).zip
	mkdir edcre-$(VER)-$(PLATFORM)
	cp -rv edcre.exe images readme.md license.txt edcre-$(VER)-$(PLATFORM)
	chmod -R 777 edcre-$(VER)-$(PLATFORM)
	zip -r edcre-$(VER)-$(PLATFORM).zip edcre-$(VER)-$(PLATFORM)
	rm -rf edcre-$(VER)-$(PLATFORM)

linux-x86-release: linux-x86
	make linux-release PLATFORM=linux_x86_static

linux-x86_64-release: linux-x86_64
	make linux-release PLATFORM=linux_x86_64_static

windows-x86-release: windows-x86
	make windows-release PLATFORM=windows_x86

windows-x86_64-release: windows-x86_64
	make windows-release PLATFORM=windows_x86_64

clean-zip: clean
	rm -rf *.zip

all: linux-x86-release linux-x86_64-release windows-x86-release windows-x86_64-release
