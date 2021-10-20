#Linux/WSL targets

all: release debug
.PHONY: all

clean:
	rm *.o proj2-release proj2-debug
.PHONY: clean

release: project.c
	gcc -o proj2-release project.c

debug: project.c
	gcc -g -o proj2-debug project.c

run-rel: release
	./proj2-release

run-dbg: debug
	./proj2-debug

# Windows targets using NMAKE/CL

allw: releasew debugw
.PHONY: allw

cleanw:
	del project.obj proj2-release.exe proj2-debug.exe
.PHONY: cleanw

releasew: project.c
	cl /Fe:proj2-release.exe project.c

debugw: project.c
	cl /DEBUG /Fe:proj2-debug.exe project.c

run-relw: releasew
	.\proj2-release.exe

run-dbgw: debugw
	.\proj2-debug.exe