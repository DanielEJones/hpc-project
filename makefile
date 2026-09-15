serial:
	cc -Wall -Werror src/file_actions.c src/hash_table.c src/main_serial.c src/pdf_actions.c src/toy_hash.c -o main

parallel:
	cc -Wall -Werror src/file_actions.c src/hash_table.c src/main_parallel.c src/pdf_actions.c src/toy_hash.c -o main -fopenmp
