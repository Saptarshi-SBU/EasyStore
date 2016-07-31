#include <iostream>
#include <utility>
#include <algorithm>
#include <chrono>
#include <atomic>
#include <cv.h>
#include <highgui.h>
#include <unistd.h>

#include "ThreadedImageScale.h"

#define DEST_DIR "/tmp/"

std::atomic<int> item;

static void*
progress_func(void *argument) {

	int curr = 0;
	struct per_scale_thread *arg = (struct per_scale_thread*) argument;

	do {
		sleep(5);
		curr = (item*100)/arg->org_img_.size();
		cout << "\rProgress :" << curr << "%";
                fflush(stdout);
	} while (curr < 100);
}

ThreadedImgScaler::ThreadedImgScaler(int threads, int scale) : ImgScaler(scale) {
	threads_ = threads;
	item = 0;
}

ThreadedImgScaler::~ThreadedImgScaler() {
	for (auto& i : v_args_) {
		i->orgstore_.clear();
		i->sclstore_.clear();
		i->org_img_.clear();
	}
	v_args_.clear();
}

void*
scale_func(void *argument) {
	cout << __func__ << endl;

	struct per_scale_thread *arg = (struct per_scale_thread*) argument;

	for (int i = arg->prm.start; i < (arg->prm.start + arg->prm.rng); i++) {
		try {

			IplImage *source = cvLoadImage(arg->org_img_.at(i).c_str());

			IplImage *destination = cvCreateImage
				( cvSize((int)((source->width*arg->prm.pscale)/100) , (int)((source->height*arg->prm.pscale)/100) ),
				  source->depth, source->nChannels );

			cvResize(source, destination, cv::INTER_CUBIC);

			arg->orgstore_.insert(make_pair(arg->org_img_.at(i), source->imageSize));
			arg->sclstore_.insert(make_pair(arg->org_img_.at(i), destination->imageSize));

			string dst = string(DEST_DIR);
			dst.append(arg->org_img_.at(i));
			cvSaveImage(dst.c_str(), destination);

			cvReleaseImage(&source);
			cvReleaseImage(&destination);	

		} catch (exception &ex) {

			cout << ex.what() << endl;
		}
		item++;
	}
}

void
ThreadedImgScaler::merge(void) {

	cout << __func__ << endl;

	for (auto& i : v_args_) {
		for (auto& j : i->orgstore_) 
			org_img_store_.insert(j);
	}

	for (auto& i : v_args_) {
		for (auto& j : i->sclstore_) 
			scl_img_store_.insert(j);
	}
}

void
ThreadedImgScaler::progress_bar(void) {

	cout << __func__ << endl;

	per_scale_thread* argp = new per_scale_thread(0, 0, 0, org_img_);

	pthread_create(&argp->thread, NULL, progress_func, (void*)argp);

	v_args_.push_back(argp);
}

void
ThreadedImgScaler::scale_image_threaded(void) {

	cout << __func__ << endl;

	int next = 0;

	size_t span = org_img_.size()/threads_;

	size_t rem  = org_img_.size()%threads_;

	auto begin  = chrono::steady_clock::now();

	for(int i = 0; i < threads_; i++) {
		per_scale_thread* argp = new per_scale_thread(scale_, next, span, org_img_);
		next+=span;
		pthread_create(&argp->thread, NULL, scale_func, (void *)argp);
		v_args_.push_back(argp);
	}
#if 1
	if (rem){
		per_scale_thread* argp = new per_scale_thread(scale_, next, rem, org_img_);
		pthread_create(&argp->thread, NULL, scale_func, (void *)argp);
		v_args_.push_back(argp);
	}
#endif
	for (int i = 0; i < v_args_.size(); i++)
		pthread_join(v_args_.at(i)->thread, NULL);

	auto end = chrono::steady_clock::now();

	cout << "\ndelta = " << chrono::duration_cast<std::chrono::seconds>(end - begin).count() << endl;
}
