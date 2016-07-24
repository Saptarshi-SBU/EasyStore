#ifndef _THREADEDNEARDUPLICATE_H
#define _THREADEDNEARDUPLICATE_H
#include <pthread.h>
#include "NearDuplicate.h"

typedef long long unsigned int ulong64;

struct per_thread {
	pthread_t thread;
	struct param_t {
		int    start;
		size_t rng;
	}prm;
	unordered_multimap<string, ulong64> fpstore_;
	vector<string> org_img_;

	per_thread(int start, size_t span, vector<string> img_) : org_img_(img_) {
        	prm.start = start;
       		prm.rng   = span;
	}

       ~per_thread() {}
};

void* compute_thread(void* arg);

class ThreadedNearDuplicate : public NearDuplicate {

	private :

		int threads_;

		vector<per_thread*> v_args_;

	public :

		ThreadedNearDuplicate(int, int);

		~ThreadedNearDuplicate();

		void merge_sig(void);

		void comp_imgv_sig(void);

		void progress_bar(void);
};

#endif
