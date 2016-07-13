#ifndef _CHUNK_ENGINE_H
#define _CHUNK_ENGINE_H

#include<string>
#include<iostream>
#include<fstream>
#include<vector>
#include<unordered_map>

#define CHUNK_SIZE 4096

//#define CHUNK_SIZE 512

//#define CHUNK_SIZE 128

//#define CHUNK_SIZE 64

using namespace std;

class ChunkEngine {

	private :

	uint32_t chunk_size_ = CHUNK_SIZE;

	unordered_map<char*, size_t> bpstore_;

	unordered_multimap<uint64_t, char*> fpstore_;

	protected :

	void get_handle(string& file, ifstream&);

	size_t get_size(ifstream& ifile);

	char* mem_alloc(size_t length);

	void chunk(ifstream& ifile, char * chunkbase, size_t length);

	public :

	ChunkEngine();

	ChunkEngine(uint32_t);

       ~ChunkEngine();

	void process(string& file);

	void chunk_stat(void);
#if 0
	void chunk_cmp(void);
#endif
	int chunk_cmp_single(uint64_t key, char *mem);

};

#endif
