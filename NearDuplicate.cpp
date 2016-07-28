#include <vector>
#include <iostream>
#include <utility>
#include <algorithm>
#include <chrono>

#include "NearDuplicate.h"
#include "pHash.h"

NearDuplicate::NearDuplicate(int thresh) {
	threshold_ = thresh;
}

NearDuplicate::~NearDuplicate() {
	org_img_.clear();
	dup_img_.clear();
	fpstore_.clear();
}

ulong64
NearDuplicate::compute_imghash(const char* file) {
	ulong64 hash = 0;
	ph_dct_imagehash(file, hash);
	return hash;
}

void
NearDuplicate::handle(string& file, ifstream& ifile) {

	ifile.exceptions(ifstream::failbit);

	try {
		ifile.open(file);

	} catch (ifstream::failure& fail) {

		cout << fail.what() << endl;
		throw exception();
	}
}

size_t
NearDuplicate::size(ifstream& ifile) {

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
void
NearDuplicate::read_imgv(const char* dir_name) {

	struct dirent *dir_entry = NULL;

	cout << __func__ << endl;

	org_img_.clear();

	DIR *dir = opendir(dir_name);
    	if (!dir){
        	printf("unable to open directory\n");
        	exit(1);
    	}

	cout << "Reading directory " << dir_name << endl;

	while ((dir_entry = readdir(dir)) != 0) {
		string str;
        	if (strcmp(dir_entry->d_name,".") && strcmp(dir_entry->d_name,"..")) {
            		str.append(dir_name);
            		str.append("/");
            		str.append(dir_entry->d_name);
			org_img_.push_back(str);
		}
	}

}

void
NearDuplicate::comp_imgv_sig(void) {
	unsigned int p = 0;

	cout << __func__ << endl;

	fpstore_.clear();
	for (auto& i : org_img_) {
		ulong64 hash = compute_imghash(i.c_str());
		fpstore_.insert(make_pair(i, hash));
		cout << "[" << p++ << "]" << __func__ << "\t" << i << "\t " << "sig\t" << hash << endl;
#if 0
		if (p == 10)
			break;
#endif
	}
}

size_t
NearDuplicate::comp_duplicate_size(void) {

	size_t near_duplicate_size = 0;

	cout << __func__ << endl;

	for (auto& i : dup_img_) {
		ifstream ifs;
		handle(i, ifs);
		near_duplicate_size+=size(ifs);
	}
	return near_duplicate_size;
}

void
NearDuplicate::process_near_duplicate(void) {
	size_t i = 0;
	unordered_map<string, ulong64>::iterator p;
	unordered_map<string, ulong64>::iterator q;

	dup_img_.clear();

	cout << __func__ << endl;

	auto begin = chrono::steady_clock::now();

	for (p = fpstore_.begin(); p != fpstore_.end(); p++) {
		for (q = next(p, 1); q != fpstore_.end(); q++) {
			int distance = ph_hamming_distance((*p).second, (*q).second);
			if (distance > threshold_)
				continue;
			if (find(dup_img_.begin(), dup_img_.end(), (*q).first) != dup_img_.end())
				continue;
			dup_img_.push_back((*q).first);
			cout << "[" << i++ << "]" << "hamming dist\t" << distance << "\t" << (*p).first << "\t" << (*q).first << endl;
		}
	}

	auto end = chrono::steady_clock::now();

	cout << "\ndelta = " << chrono::duration_cast<std::chrono::seconds>(end - begin).count() << endl;
}

void
NearDuplicate::stat_near_duplicate(void) {

	cout << "total " << fpstore_.size() << " dup items  " << dup_img_.size() << " saved size " << comp_duplicate_size() << endl;
}
