INCLUDES = -I./src -I../stdlib/src
FLAGS = -g -falign-jumps -falign-functions -falign-labels -falign-loops -fstrength-reduce -fomit-frame-pointer -finline-functions -Wno-unused-function -fno-builtin -Werror -Wno-unused-label -Wno-cpp -Wno-unused-parameter -nostdlib -nostartfiles -nodefaultlibs -Wall -O0 -Iinc -std=gnu99

OBJECTS = ./build/start.asm.o ./build/vix_pong.o ./build/vios.asm.o ./build/vios.o ./build/stdlib.o ./build/stdio.o ./build/string.o ./build/memory.o ./build/start.o

all: ./vix_pong.elf

./vix_pong.elf: $(OBJECTS)
	i686-elf-ld -m elf_i386 -relocatable $(OBJECTS) -o ./vix_pong.elf

./build/start.asm.o:
	mkdir -p build/
	nasm -f elf ../stdlib/src/start.asm -o build/start.asm.o

./build/vix_pong.o:
	mkdir -p build/
	i686-elf-gcc $(INCLUDES) $(FLAGS) -std=gnu99 -c src/vix_pong.c -o build/vix_pong.o

./build/vios.asm.o:
	mkdir -p build/
	nasm -f elf ../stdlib/src/vios.asm -o build/vios.asm.o

./build/vios.o:
	mkdir -p build/
	i686-elf-gcc $(INCLUDES) $(FLAGS) -std=gnu99 -c ../stdlib/src/vios.c -o build/vios.o

./build/stdlib.o:
	mkdir -p build/
	i686-elf-gcc $(INCLUDES) $(FLAGS) -std=gnu99 -c ../stdlib/src/stdlib.c -o build/stdlib.o

./build/stdio.o:
	mkdir -p build/
	i686-elf-gcc $(INCLUDES) $(FLAGS) -std=gnu99 -c ../stdlib/src/stdio.c -o build/stdio.o

./build/string.o:
	mkdir -p build/
	i686-elf-gcc $(INCLUDES) $(FLAGS) -std=gnu99 -c ../stdlib/src/string.c -o build/string.o

./build/memory.o:
	mkdir -p build/
	i686-elf-gcc $(INCLUDES) $(FLAGS) -std=gnu99 -c ../stdlib/src/memory.c -o build/memory.o

./build/start.o:
	mkdir -p build/
	i686-elf-gcc $(INCLUDES) $(FLAGS) -std=gnu99 -c ../stdlib/src/start.c -o build/start.o

clean:
	rm -rf ./build/*.o
	rm -rf ./build
	rm -f ./vix_pong.elf
