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
	struct filehead header = { 0xDEAD1988,0 };
	struct fileentry* p = NULL;
	uint64_t file_offset = 0;
	
	uint8_t* buffer = NULL;
	size_t buffersize;

	uint64_t szstruct = sizeof(struct fileentry);
	if ((dir = opendir(".")) != NULL) {
		while ((ent = readdir(dir)) != NULL) {
			if (!strcmp(ent->d_name, "."))
				continue;
			if (!strcmp(ent->d_name, ".."))
				continue;
			if (!strcmp(ent->d_name,"arch.mpk"))
				continue;
			if (!strcmp(ent->d_name,"archivetest.exe"))
				continue;

			header.numentries++;
			num_files++;

			printf("%s\n", ent->d_name);

			if (!p)
				p = (struct fileentry*)malloc(sizeof(struct fileentry));
			else
				p = (struct fileentry*)realloc(p, sizeof(struct fileentry) * num_files);

			FILE* filep;
			size_t filesize = 0;;
			filep = fopen(ent->d_name, "rb");
			if (filep == NULL) { fputs("File error", stderr); return; }
			fseek(filep, 0, SEEK_END);
			filesize = ftell(filep);
			rewind(filep);

			buffersize += filesize;
			
			if (!buffer)
				buffer = (uint8_t*)malloc(sizeof(uint8_t) * buffersize);
			else
				buffer = (uint8_t*)realloc(buffer, buffersize);

			if (buffer == NULL) { fputs("Memory error", stderr); return; }
			size_t result = fread(buffer, 1, filesize, filep);
			if (result != filesize) { fputs("Reading error", stderr); return; }
				
			fclose(filep);
				
			(p+num_files-1)->uncomp_size = filesize;
			(p+num_files-1)->offset = file_offset;
			(p+num_files-1)->comp_size = filesize;
			uint8_t* name = (p + num_files - 1)->name;
			strncpy((char*)name,ent->d_name,8);

			file_offset += filesize;
		}
		closedir(dir);
		
		
		FILE* fp= fopen("arch.mpk", "wb");
		fwrite(buffer, buffersize, 1, fp);
		fclose(fp);
		
		free(buffer);


		header.numentries = num_files;

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
