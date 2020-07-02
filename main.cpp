

#include <stdio.h>
#include "dirent.h"
#include <cstdlib>

#pragma pack(1)
struct filehead {
	int magic;
	int numfiles;
};
struct fileentry {
	int uncomp_size;
	int comp_size;
	int offset;
	char name[9]; //inc zerobyte
};
#pragma pop

void main()
{
	
	DIR* dir;
	struct dirent* ent;
	int num_files = 0;
	filehead header = { 0xDEAD1988,0 };
	struct fileentry* p = NULL;
	int szstruct = sizeof(fileentry);
	if ((dir = opendir(".")) != NULL) {
		while ((ent = readdir(dir)) != NULL) {
			header.numfiles++;
			num_files++;
			printf("%s\n", ent->d_name);
			if (!p)
				p = (fileentry*)malloc(sizeof(struct fileentry));
			else
				p = (fileentry*)realloc(p, sizeof(fileentry) * num_files);
			p[num_files - 1].uncomp_size = 0x10;
			p[num_files - 1].comp_size = 0x20;
			p[num_files - 1].offset = 0xDEAD1988;
			memset(p[num_files - 1].name, 0, 8);
			strncpy(p[num_files - 1].name,ent->d_name,8);
		}
		closedir(dir);

		while (p != NULL)
		{
			printf("%s\n",p->name);
			printf("%0x08\n",p->offset);
			printf("%0x08\n",p->comp_size);
			printf("%0x08\n",p->uncomp_size);
			printf("\n");
			p++;
		}
	}

	else {

	}


}
