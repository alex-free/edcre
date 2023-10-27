# edcre GNUMakefile by Alex Free
CXX=g++
CXX_FLAGS=-Wall -Werror -Ofast
VER=1.0.5

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
	rm -rf edcre-v$(VER)-$(PLATFORM) edcre-v$(VER)-$(PLATFORM).zip
	mkdir edcre-v$(VER)-$(PLATFORM)
	cp -rv edcre images readme.md license.txt edcre-v$(VER)-$(PLATFORM)
	chmod -R 777 edcre-v$(VER)-$(PLATFORM)
	zip -r edcre-v$(VER)-$(PLATFORM).zip edcre-v$(VER)-$(PLATFORM)
	rm -rf edcre-v$(VER)-$(PLATFORM)

windows-release:
	rm -rf edcre-v$(VER)-$(PLATFORM) edcre-v$(VER)-$(PLATFORM).zip
	mkdir edcre-v$(VER)-$(PLATFORM)
	cp -rv edcre.exe images readme.md license.txt edcre-v$(VER)-$(PLATFORM)
	chmod -R 777 edcre-v$(VER)-$(PLATFORM)
	zip -r edcre-v$(VER)-$(PLATFORM).zip edcre-v$(VER)-$(PLATFORM)
	rm -rf edcre-v$(VER)-$(PLATFORM)

linux-x86-release: linux-x86
	make linux-release PLATFORM=linux-x86_static

linux-x86_64-release: linux-x86_64
	make linux-release PLATFORM=linux-x86_64_static

windows-x86-release: windows-x86
	make windows-release PLATFORM=windows-x86

windows-x86_64-release: windows-x86_64
	make windows-release PLATFORM=windows-x86_64

clean-zip: clean
	rm -rf *.zip

all: linux-x86-release linux-x86_64-release windows-x86-release windows-x86_64-release
