/*
References

Writing a Simple Filesystem Using FUSE in C
https://www.maastaar.net/fuse/linux/filesystem/c/2016/05/21/writing-a-simple-filesystem-using-fuse/
 */

#define FUSE_USE_VERSION 26

#include "../libWad/Wad.h"
#include <fuse.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <time.h>
#include <string.h>
#include <stdlib.h>

using namespace std;

static Wad* wadSys;

// Function for getting the attributes of a specific file
static int do_getattr(const char *path, struct stat *st)
{
	memset(st, 0, sizeof(struct stat));

	st->st_uid = 0;

	st->st_gid = 0;

	st->st_atime = 0;

	st->st_mtime = 0;

	st->st_nlink = 2;

	if(wadSys->isDirectory(path))
	{
		st -> st_mode = S_IFDIR | 0555;

		return 0;
	}

	if(wadSys->isContent(path))
	{
		st->st_mode = S_IFREG | 0444;

		st->st_size = wadSys->getSize(path);

		return 0;
	}

	return -ENOENT;
}

// Function for opening a specific file
static int do_open(const char* path, struct fuse_file_info* fi)
{
	return 0;
}

// Function for reading the content of a specific file
static int do_read(const char *path, char *buffer, size_t size, off_t offset, struct fuse_file_info *fi)
{
	(void) fi;

	return wadSys->getContents(path, buffer, size, offset);
}

// Function for listing the files/directories which are available inside a specific directory
static int do_readdir(const char *path, void *buffer, fuse_fill_dir_t filler, off_t offset, struct fuse_file_info *fi)
{
	(void) offset;

	(void) fi;

	filler(buffer, ".", NULL, 0);

	filler(buffer, "..", NULL, 0);

	vector<string> entries;

	int ret = wadSys->getDirectory(path, &entries);

	for(int i = 0; i < entries.size(); i += 1)
	{
		filler(buffer, entries.at(i).c_str(), NULL, 0);
	}

	return 0;
}

// Function for destroying a filesystem
static void do_destroy(void* private_data)
{
	wadSys->clear(wadSys->getRootNode());

	delete wadSys;
}

// Struct for all the FUSE operations
static struct fuse_operations operations = {
    .getattr	= do_getattr,
    .open 	= do_open,
    .read       = do_read,
    .readdir	= do_readdir,
    .destroy 	= do_destroy,
};

// Running our filesystem
int main(int argc, char* argv[])
{
	wadSys = Wad::loadWad(argv[1]);

	char* targv[2];

	targv[0] = argv[0];

	targv[1] = argv[2];

	return fuse_main(2, targv, &operations, NULL);
}

