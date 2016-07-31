#ifndef _THREADEDIMGSCALE_H
#define _THREADEDIMGSCALE_H
#include <pthread.h>
#include "ImageScale.h"

struct per_scale_thread {
	pthread_t thread;
	struct param_t {
		int    start;
		size_t rng;
		int    pscale;
	}prm;
	unordered_multimap<string, int> orgstore_;
	unordered_multimap<string, int> sclstore_;
	vector<string> org_img_;

	per_scale_thread(int scale, int start, size_t span, vector<string> img_) : org_img_(img_) {
        	prm.start  = start;
       		prm.rng    = span;
		prm.pscale = scale;
	}

       ~per_scale_thread() {}
};

void* scale_func(void* arg);

class ThreadedImgScaler : public ImgScaler {

	private :

		int threads_;

		vector<per_scale_thread*> v_args_;

	public :

		ThreadedImgScaler(int, int);

		~ThreadedImgScaler();

		void scale_image_threaded(void);

		void progress_bar(void);

		void merge(void);
};

#endif
