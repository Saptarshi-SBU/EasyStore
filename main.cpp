#include <unistd.h>
#include <errno.h>

#include "ChunkEngine.h"
#include "MurmurHash2.h"
#include "NearDuplicate.h"
#include "ThreadedNearDuplicate.h"

#define DEDUP 0
#define NEARDUPLICATE 0
#define FASTNEARDUPLICATE 1

using namespace std;

struct options {
	string   ifile;
	const char* dir;
	uint32_t chunk_size;
	int threshold;
	int threads;
};

int
parse(int argc, char **argv, struct options* opt) {

	int c = 0;
	int r = 0;

	opt->threads = 1;
	opt->threshold = 12;
	opt->chunk_size = 512;

	while ((c = getopt(argc, argv, "c:f:d:t:p:")) != -1) {

        	switch(c) {
       		case 'c':
	    	if (optarg)
              		opt->chunk_size = stoul(optarg, 0, 10);
		printf("Chunk Size : %lu\n", opt->chunk_size);
       		case 'f':
	    	if (optarg)
              		opt->ifile = string(optarg);
	    	else
            		fprintf(stderr,
                    	"invalid option: file name missing\n");
            	break;
       		case 'd':
	    	if (optarg)
              		opt->dir = optarg;
	    	else
            		fprintf(stderr,
                    	"invalid option: dir name missing\n");
            	break;
       		case 'p':
	    	if (optarg)
              		opt->threads = stoul(optarg, 0, 10);
	    	else
            		fprintf(stderr,
                    	"invalid option: number of threads missing\n");
            	break;
       		case 't':
	    	if (optarg)
              		opt->threshold = stoul(optarg, 0, 10);
	    	else
            		fprintf(stderr,
                    	"invalid option: file name missing\n");
            	break;
		case '?':
            	fprintf(stderr,
                	    "Unrecognized option: -%c\n", optopt);
		r = -EINVAL;
		}
	}

	return r;
}


int main(int argc, char **argv) {

	struct options opt;
	if (parse(argc, argv, &opt) < 0)
		return -1;
#if DEDUP
	ChunkEngine* obj = new ChunkEngine(opt.chunk_size);

	ifstream ifs(opt.ifile);
	string filename;
	while (getline(ifs, filename))
		obj->process(filename);
	ifs.close();

	obj->chunk_stat();
#endif

#if NEARDUPLICATE
	NearDuplicate* nobj = new NearDuplicate(opt.threshold);

	nobj->read_imgv(opt.dir);

	nobj->comp_imgv_sig();

	nobj->process_near_duplicate();

        nobj->stat_near_duplicate();
#endif

#if FASTNEARDUPLICATE
	ThreadedNearDuplicate* tobj = new ThreadedNearDuplicate(opt.threads, opt.threshold);

	tobj->read_imgv(opt.dir);

	tobj->progress_bar();

	tobj->comp_imgv_sig();

	tobj->merge_sig();

	tobj->process_near_duplicate();

        tobj->stat_near_duplicate();

	tobj->process_near_duplicate_aknn(10, 16, opt.threshold);

        tobj->stat_near_duplicate();
#endif
        return 0;
}
