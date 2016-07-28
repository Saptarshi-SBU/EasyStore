#ifndef _NEARDUPLICATE_H
#define _NEARDUPLICATE_H

#include<string>
#include<iostream>
#include<fstream>
#include<unordered_map>

using namespace std;

typedef long long unsigned int ulong64;

class NearDuplicate {

	private :

	int threshold_;

	protected :

	size_t near_duplicate_size = 0;

	unordered_multimap<string, ulong64> fpstore_;

	vector<string> org_img_;

	vector<string> dup_img_;

	void handle(string& file, ifstream& ifile);

	size_t size(ifstream& ifile);

	virtual ulong64 compute_imghash(const char* file);

	public :

	NearDuplicate(int);

       ~NearDuplicate();

	void read_imgv(const char*);

	void comp_imgv_sig(void);

	size_t comp_duplicate_size(void);

	void process_near_duplicate(void);

	void stat_near_duplicate(void);
};

#endif
