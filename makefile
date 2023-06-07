all: testfork testfork-inst testpipe testpipe-inst stackviz

testfork: testfork.c
	# compile testfork.c without instrumenting
	gcc -o testfork -g -Wall testfork.c

testfork-inst: testfork.c inst.o
	# compile testfork.c with instrumentation
	gcc -o testfork-inst -finstrument-functions -g -Wall -pthread testfork.c inst.o

testpipe: testpipe.c
	# compile testpipe.c without instrumenting
	gcc -o testpipe -g -Wall testpipe.c

testpipe-inst: testpipe.c inst.o
	# compile testpipe.c with instrumentation
	gcc -o testpipe-inst -finstrument-functions -g -Wall -pthread testpipe.c inst.o

inst.o: inst.c
	# compile inst.c
	gcc -c -g -Wall -pthread inst.c

stackviz: stackviz.c
	# compile stackviz.c
	gcc -o stackviz -g -Wall stackviz.c

clean:
	# remove the object files, the binary files, and the log file
	rm -f *.o testfork testfork-inst testpipe testpipe-inst stackviz inst stack.bin


