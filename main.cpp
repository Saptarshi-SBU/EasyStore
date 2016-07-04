#include <unistd.h>
#include <errno.h>

#include "ChunkEngine.h"
#include "MurmurHash2.h"

using namespace std;

int 
parse(int argc, char **argv, string& ifile) {

	int c = 0;
	int r = -EINVAL;

	while ((c = getopt(argc, argv, "f:")) != -1) {

        	switch(c) {
       		case 'f':
	    	if (optarg) {
			r = 0;
              		ifile = string(optarg);
	    	} else
            		fprintf(stderr,
                    	"invalid option: file name missing\n");
            	break;
		case '?':
            	fprintf(stderr,
                	    "Unrecognized option: -%c\n", optopt);
		}
	}
	
	return r;
}


int main(int argc, char **argv) {

	ChunkEngine obj;

	string filename;
#if 0
	if (parse(argc, argv, filename) < 0)
		return -1;

	obj.chunk(filename);

	obj.dump_chunk_list();
#endif
	
	ifstream ifs("dat/file_list");
	while (getline(ifs, filename))
		obj.process(filename);
	ifs.close();
	obj.chunk_stat();
	obj.chunk_cmp();
        return 0;
}
