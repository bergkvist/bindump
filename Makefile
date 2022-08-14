flags := -O2 -Wall -Wextra

build/bindump: bindump.c
	mkdir -p build
	cc $^ -o$@ $(flags)

build/hello_world: hello_world.c
	mkdir -p build
	cc $^ -o$@ $(flags)

test: build/bindump build/hello_world
	./build/bindump ./build/hello_world
