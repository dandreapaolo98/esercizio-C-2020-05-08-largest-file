/*
 * esercizio-C-2020-05-08-largest-file.c
 *
 *  Created on: May 7, 2020
 *      Author: marco
 */
/***************TESTO ESERCIZIO***************
esercizio:


parte 1 - trova il file regolare più grande cercandolo all'interno di una directory

parte 2 - trova il file regolare più grande cercandolo all'interno di una directory e ricorsivamente in tutte le sue sotto-directory

scrivere la seguente funzione:

char * find_largest_file(char * directory_name, int explore_subdirectories_recursively, int * largest_file_size);

la funzione restituisce il percorso completo del file regolare più grande cercato nella directory specificata da directory_name.
se explore_subdirectories_recursively != 0, allora cerca ricorsivamente in tutte le sue sotto-directory.
(a parità di dimensioni, considera il primo trovato).

la dimensione del file regolare più grande viene scritta nella variabile il cui indirizzo è dato da largest_file_size.
se non ci sono file regolare, la funzione restituisce NULL (e largest_file_size non viene utilizzato).


provare a fare girare il programma a partire da queste directory:

/home/utente

/
*/

#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <dirent.h>
#include <errno.h>
#include <sys/stat.h>
#include <fcntl.h>

char * find_largest_file(char * directory_name, int explore_subdirectories_recursively, int * largest_file_size);

int main(int argc, char * argv[]) {
	int file_size;
	char * res = find_largest_file("/home/utente", 1, &file_size);
	if(res != NULL)
		printf("file path : %s with size %d bytes", res, file_size);
	else
		printf("nessun file regolare trovato");
	return 0;
}

char * find_largest_file(char * directory_name, int explore_subdirectories_recursively, int * largest_file_size){
	if(directory_name == NULL){
		printf("directory inesistente! bye");
		return EXIT_FAILURE;
	}
	char * abs_file_name;
	DIR * dir_stream_ptr;
	struct dirent *ep;

	errno = 0;

	dir_stream_ptr = opendir(directory_name);

	if (dir_stream_ptr == NULL) {
		printf("cannot open directory %s! bye", directory_name);

		return EXIT_FAILURE;
	}

	*largest_file_size = -1;
	char* file_name;

	while ((ep = readdir(dir_stream_ptr)) != NULL) {
		//ricostruisco il path assoluto del file:
		if(directory_name[strlen(directory_name)-1] != '/')
			file_name = malloc(strlen(directory_name) + strlen(ep->d_name) + 2);
		else
			file_name = malloc(strlen(directory_name) + strlen(ep->d_name) + 1);
		file_name[0] = '\0';
		strcat(file_name, directory_name);
		if(directory_name[strlen(directory_name)-1] != '/')
			strcat(file_name, "/");
		strcat(file_name, ep->d_name);

		struct stat st;
		stat(file_name, &st);

		if(ep->d_type == DT_REG && st.st_size > *largest_file_size){
			abs_file_name = file_name;
			*largest_file_size = st.st_size;
		}

		//controllo se bisogna controllare anche nelle sotto directory (escludendo . e .. )
		if(explore_subdirectories_recursively == 1 && ep->d_type == DT_DIR
				&& strcmp(ep->d_name, ".") != 0 && strcmp(ep->d_name, "..") != 0){
			int temp_size;
			char * sub_largest_file = find_largest_file(file_name, 1, &temp_size);
			if(sub_largest_file != NULL && temp_size > *largest_file_size){
				abs_file_name = sub_largest_file;
				*largest_file_size = temp_size;
			}
		}
	}

	if (errno) {
		perror("readdir() error");
	}

	closedir(dir_stream_ptr);

	if(*largest_file_size == -1)	//se il valore è ancora -1, vuol dire che non è stato trovato nessun file regolare
		return NULL;

	return abs_file_name;
}

