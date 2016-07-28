#include <iostream>
#include <utility>
#include <algorithm>
#include <chrono>
#include <atomic>

#include "pHash.h"
#include "lsh.h"
#include "ThreadedNearDuplicate.h"

std::atomic<int> no;

ulong64
compute_hash(const char* file) {
	ulong64 hash = 0;
	ph_dct_imagehash(file, hash);
	return hash;
}

void*
progress_func(void *argument) {

	int curr = 0;
	struct per_thread *arg = (struct per_thread*) argument;

	do {
		sleep(5);
		curr = (no*100)/arg->org_img_.size();
		cout << "\rProgress :" << curr << "%";
                fflush(stdout);
	} while (curr < 100);
}

ThreadedNearDuplicate::ThreadedNearDuplicate(int threads, int thresh) : NearDuplicate(thresh) {
	threads_ = threads;
	no = 0;
}

ThreadedNearDuplicate::~ThreadedNearDuplicate() {
	for (auto& i : v_args_)
		i->fpstore_.clear();
	v_args_.clear();
}

void*
compute_thread(void *argument) {
	cout << __func__ << endl;

	struct per_thread *arg = (struct per_thread*) argument;

	for (int i = arg->prm.start; i < (arg->prm.start + arg->prm.rng); i++) {
		ulong64 hash = compute_hash(arg->org_img_.at(i).c_str());
		arg->fpstore_.insert(make_pair(arg->org_img_.at(i), hash));
		no++;
	}
}

void
ThreadedNearDuplicate::merge_sig(void) {

	cout << __func__ << endl;

	ulong64 min = ULONG_MAX;
	ulong64 max = 0;

	for (auto& i : v_args_) {
		for (auto& j : i->fpstore_) {
			if (j.second > max)
				max = j.second;
			if (j.second < min)
				min = j.second;
			fpstore_.insert(j);
		}
	}

	cout << "Max " << max << endl;
	cout << "Min " << min << endl;
}

void
ThreadedNearDuplicate::max_min_range(ulong64& min, ulong64& max) {

	cout << __func__ << endl;
}

void
ThreadedNearDuplicate::progress_bar(void) {

	cout << __func__ << endl;

	per_thread* argp = new per_thread(0, 0, org_img_);

	pthread_create(&argp->thread, NULL, progress_func, (void*)argp);

	v_args_.push_back(argp);
}

void
ThreadedNearDuplicate::comp_imgv_sig(void) {

	cout << __func__ << endl;

	int next = 0;

	size_t span = org_img_.size()/threads_;

	size_t rem  = org_img_.size()%threads_;

	auto begin  = chrono::steady_clock::now();

	for(int i = 0; i < threads_; i++) {
		per_thread* argp = new per_thread(next, span, org_img_);
		next+=span;
		pthread_create(&argp->thread, NULL, compute_thread, (void *)argp);
		v_args_.push_back(argp);
	}
#if 1
	if (rem){
		per_thread* argp = new per_thread(next, rem, org_img_);
		pthread_create(&argp->thread, NULL, compute_thread, (void *)argp);
		v_args_.push_back(argp);
	}
#endif
	for (int i = 0; i < v_args_.size(); i++)
		pthread_join(v_args_.at(i)->thread, NULL);

	auto end = chrono::steady_clock::now();

	cout << "\ndelta = " << chrono::duration_cast<std::chrono::seconds>(end - begin).count() << endl;
}

void
ThreadedNearDuplicate::process_near_duplicate_aknn(int hash_no, int bins, int threshold) {

	unordered_map<string, ulong64>::iterator p;

	cout << __func__ << endl;

	lsh::init_hash_tables(hash_no);

	dup_img_.clear();

	auto begin  = chrono::steady_clock::now();

	for (int k = 0; k < hash_no; k++) {
                lsh::generate_random_set(bins, min_, max_);
#if DEBUG
                cout << "########Hash Table" << "[" << k << "]" << "Signature########" << endl;
                for (auto& p : lsh::rp_)
                        cout << p << " ";
                cout << endl;
#endif
		for (p = fpstore_.begin(); p != fpstore_.end(); p++) {
                        string sig  = lsh::generate_bin_vector((*p).second);
#if DEBUG
                        cout << std::left << setw(30) << (*p).first << setw(25) << (*p).second << setw(25) << sig << endl;
#endif
                        lsh::insert_bin_vector(sig, make_pair((*p).first, (*p).second), lsh::v_table_.at(k));
                }
	}

	lsh::process_near_duplicate(dup_img_, threshold);

	auto end = chrono::steady_clock::now();

	cout << "\ndelta = " << chrono::duration_cast<std::chrono::seconds>(end - begin).count() << endl;
}
