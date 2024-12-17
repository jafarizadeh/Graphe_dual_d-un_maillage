all: build execute 
output:=dual_graph_of_mesh_st1_final.c
	
build:
	gcc $(output) ./lib/hashmap.h -o exefile
execute:
	./exefile

.PHONY: clean

clean:
	del exefile.exe

