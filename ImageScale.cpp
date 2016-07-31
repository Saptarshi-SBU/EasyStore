#include <iostream>
#include <fstream>
#include <stdexcept>
#include <utility>
#include <dirent.h>
#include <string.h>
#include <cv.h>
#include <highgui.h>

#include "ImageScale.h"

ImgScaler::ImgScaler(int scale) : scale_(scale) {}

ImgScaler::~ImgScaler() {}

void 
ImgScaler::read_images(const char *dir_name) {

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
ImgScaler::scale_images(void) {

	for (auto& i : org_img_) 
		scale_image_single(i, scale_);
}

void 
ImgScaler::scale_image_single(string src, int pscale) {

		try {

			IplImage *source = cvLoadImage(src.c_str());

			IplImage *destination = cvCreateImage
				( cvSize((int)((source->width*pscale)/100) , (int)((source->height*pscale)/100) ),
				  source->depth, source->nChannels );

			cvResize(source, destination);

			org_img_store_.insert(make_pair(src, source->imageSize));
			scl_img_store_.insert(make_pair(src, destination->imageSize));

//			cvSaveImage(dst, destination);

		} catch (exception &ex) {

			cout << ex.what() << endl;
		}
}


void
ImgScaler::stat(void) {
	size_t tot_org_size = 0;
	size_t tot_scl_size = 0;

	for(auto& i : org_img_store_)
		tot_org_size+=i.second;

	for(auto& i : scl_img_store_)
		tot_scl_size+=i.second;

	cout << "#####Scaling Summary########" << endl;
	cout << "total_items : " << org_img_store_.size() << endl;
	cout << "org size : " << tot_org_size << "\tnew size (scaling = " << scale_ << "%) : " << tot_scl_size;
	cout << "\tpercent : " << (tot_scl_size*100)/tot_org_size << "%" << endl;
}
