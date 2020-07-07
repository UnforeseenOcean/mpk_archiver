

#include <stdio.h>
#include "dirent.h"
#include <cstdlib>
#include <stdint.h>
#include <stdbool.h>

#pragma pack(1)
struct filehead {
	uint32_t magic;
	uint32_t numentries;
};
struct fileentry {
	int64_t uncomp_size;
	int64_t comp_size;
	int64_t offset; //note this is the offset in the datastream *after* 
	                //sizeof(filehead)+sizeof(fileentry)*filehead.numentries)
	uint8_t name[9]; //inc zerobyte
};
#pragma pop

void main()
{
	
	DIR* dir;
	struct dirent* ent;
	uint64_t num_files = 0;
	filehead header = { 0xDEAD1988,0 };
	struct fileentry* p = NULL;
	uint64_t szstruct = sizeof(fileentry);
	if ((dir = opendir(".")) != NULL) {
		while ((ent = readdir(dir)) != NULL) {
			if (!strcmp(ent->d_name, "."))
				continue;
			if (!strcmp(ent->d_name, ".."))
				continue;

			header.numentries++;
			num_files++;

			printf("%s\n", ent->d_name);

			if (!p)
				p = (fileentry*)malloc(sizeof(struct fileentry));
			else
				p = (fileentry*)realloc(p, sizeof(fileentry) * num_files);
			memset(p, 0, sizeof(fileentry));
			(p+num_files-1)->uncomp_size = 0x10;
			(p+num_files-1)->offset = 0xDEAD1988;
			(p+num_files-1)->comp_size = 0x20;
			uint8_t* name = (p + num_files - 1)->name;
			strncpy((char*)name,ent->d_name,8);
		}
		closedir(dir);
		int i = 0;
		do
		{
			printf("%s\n",(p+ i)->name);
			printf("0x%04X\n", (p + i)->offset);
			printf("0x%04X\n", (p + i)->comp_size);
			printf("0x%04X\n", (p + i)->uncomp_size);
			printf("\n");
			i++;
		}
		while (i != header.numentries);

		free(p);
	}

	else {

	}


}
