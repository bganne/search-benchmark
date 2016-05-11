exe := search-k1 search-x86 ../libgpl/libgpl/libgpl.a
all : $(exe)

../libgpl/libgpl/libgpl.a:
	(cd ../libgpl/libgpl/; make -f Makefile.linux)

search-x86: search-bench.c ../libgpl/libgpl/libgpl.a
	gcc -O3 -Wall -Werror -I ../libgpl/include/ -L../libgpl/libgpl  search-bench.c -o search-x86 -lgpl

search-k1: search-bench.c ../libgpl/libgpl/libgpl.a kmemcmp/kmemcmp.h
	k1-gcc -g -O3 -Wall -Werror -march=k1b -I ../libgpl/include/ -D MPPA search-bench.c -o search-k1

PARAMS:=600 16384 32768 65536

run-bench: search-x86 search-k1
	@echo "MHZ  -  keySZ  -  ValueSZ  -  BlkSZ  -  loopNB  -  cache"
	./search-x86 $(PARAMS) 10 0
	./search-x86 $(PARAMS) 2 0
	k1-jtag-runner --exec-file=Cluster0:search-k1 -- $(PARAMS) 10 0
	k1-jtag-runner --exec-file=Cluster0:search-k1 -- $(PARAMS) 2 0
	k1-cluster --march="bostan" --mcore="cluster" --cycle-based -- search-k1 $(PARAMS) 10 0
	k1-cluster --march="bostan" --mcore="cluster" --cycle-based -- search-k1 $(PARAMS) 2 0

clean:
	(cd ../libgpl/libgpl/; make -f Makefile.linux clean)
	rm -f $(exe)

