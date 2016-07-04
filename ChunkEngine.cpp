#include <cassert>
#include <exception>
#include <utility>
#include <stdio.h>
#include <string.h>


#include "MurmurHash2.h"
#include "ChunkEngine.h"

#define ROUND_UP(x, y) ( x + (y - (x % y)))

#define FP_SEED 0XA0A0A0A0A0A0A0A0

ChunkEngine::ChunkEngine()  { }

ChunkEngine::~ChunkEngine() {

	if (fpstore_.size())
		fpstore_.clear();

	if (bpstore_.empty()) 
		return;

	for (auto& it : bpstore_)
		delete [] it.first;

	bpstore_.clear();
 }


void ChunkEngine::get_handle(string& file, ifstream& ifile) {

	ifile.exceptions(ifstream::failbit);

	try {
		ifile.open(file);

	} catch (ifstream::failure& fail) {

		cout << fail.what() << endl;
		throw exception();
	}
}

size_t ChunkEngine::get_size(ifstream& ifile) {

	size_t length;

	try {
		ifile.seekg(0, ifile.end);
		length = ifile.tellg();
		ifile.seekg(0, ifile.beg);
		return length;

	} catch (ifstream::failure& fail) {

		cout << fail.what() << endl;
		throw exception();
	}
}

char* ChunkEngine::mem_alloc(size_t length) {
	
	char* chunkbase = NULL;

	try {
		size_t size = ROUND_UP(length, chunk_size_);

		chunkbase = new char [size];

		bpstore_.insert(make_pair(chunkbase, size));

		return chunkbase;

	} catch (exception& ex) {

		cout << ex.what() << endl;
		throw ex;
	}
}

void ChunkEngine::chunk(ifstream& ifile, char* chunkbase, size_t length) {
	try {
		uint64_t key;

		size_t round_size = ROUND_UP(length, chunk_size_);

		ifile.read(chunkbase, length);

		for (int b = 0; b < round_size; b+=chunk_size_) {
			key = MurmurHash64A (( const void *) (chunkbase + b), chunk_size_, FP_SEED);
			fpstore_.insert(make_pair(key, chunkbase + b));
		}
	} catch (exception& ex) {

		cout << ex.what() << endl;
		throw ex;
	}
}

void ChunkEngine::process(string& file) {

	size_t   size;
	ifstream ifile;
        char*    chunkbase;

	ifile.exceptions(ifstream::failbit);

	try {
		get_handle(file, ifile); 
		size       = get_size(ifile);
		chunkbase  = mem_alloc(size);
   
		chunk(ifile, chunkbase, size);

	} catch (ifstream::failure& fail) {

		cout << fail.what() << endl;

	} catch (exception& ex) {

		cout << ex.what() << endl;
	}
}

void ChunkEngine::chunk_stat(void) {

	size_t q = 0;

	if (fpstore_.empty())
		return;

	for (auto i : bpstore_)
	     q+= (i.second/chunk_size_);

	assert(fpstore_.size() == q);
	cout << " p : " << fpstore_.size() << " q : " << q << endl; 
}

void ChunkEngine::chunk_cmp(void) {
	size_t p = 0;
	size_t q = 0, d = 0;

	vector<char*> result;

	for (auto& i : fpstore_) {
		q = fpstore_.count(i.first);
		if (q > 1) {
                	auto it_bounds = fpstore_.equal_range(i.first);
                	for (auto it=it_bounds.first; it!=it_bounds.second; it++)
                    		result.push_back(it->second);
			if (0 == memcmp(result.at(0), result.at(1), chunk_size_))
				p++;
                        result.clear();
			d+=q;
                }
        }
	cout << "Match Chunks : " << p << " Duplicate keys : " << d << " Chunk Size : " << chunk_size_ << endl;
}
