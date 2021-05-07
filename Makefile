
all:

	g++ -o hello_cpp mycan.cpp can_global.cpp ilm8513_elmo.cpp /home/qian/Desktop/000mycan_test/libcontrolcan.so  -lpthread 

clean:
	rm -f *.o hello
