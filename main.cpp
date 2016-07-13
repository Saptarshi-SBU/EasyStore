#include <unistd.h>
#include <errno.h>

#include "ChunkEngine.h"
#include "MurmurHash2.h"

using namespace std;

struct options {
	string   ifile;
	uint32_t chunk_size;
};

int 
parse(int argc, char **argv, struct options* opt) {

	int c = 0;
	int r = -EINVAL;

	while ((c = getopt(argc, argv, "c:f:")) != -1) {

        	switch(c) {
       		case 'c':
	    	if (optarg) 
              		opt->chunk_size = stoul(optarg, 0, 10);
		r = 0;
		printf("Chunk Size : %lu\n", opt->chunk_size);
       		case 'f':
	    	if (optarg) {
			r = 0;
              		opt->ifile = string(optarg);
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

	struct options opt;
	if (parse(argc, argv, &opt) < 0)
		return -1;

	ChunkEngine* obj = new ChunkEngine(opt.chunk_size);

	ifstream ifs(opt.ifile);
	string filename;
	while (getline(ifs, filename))
		obj->process(filename);
	ifs.close();

	obj->chunk_stat();
        return 0;
}
