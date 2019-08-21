cli_autodata:cli_autodata.c
		gcc -g -o cli_autodata cli_autodata.c lib_storage.so lib_common.so

clean:
	rm -rf cli_autodata
