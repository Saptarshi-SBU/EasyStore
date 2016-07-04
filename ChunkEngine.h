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

using namespace std;

class ChunkEngine {

	private :

	const uint32_t chunk_size_ = CHUNK_SIZE;

	unordered_map<char*, size_t> bpstore_;

	unordered_multimap<uint64_t, char*> fpstore_;

	protected :

	void get_handle(string& file, ifstream&);

	size_t get_size(ifstream& ifile);

	char* mem_alloc(size_t length);

	void chunk(ifstream& ifile, char * chunkbase, size_t length);

	public :

	ChunkEngine();

       ~ChunkEngine();

	void process(string& file);

	void chunk_stat(void);

	void chunk_cmp(void);
};

#endif
