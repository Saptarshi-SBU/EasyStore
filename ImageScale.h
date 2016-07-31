#ifndef _IMGSCALER_H
#define _IMGSCALER_H

#include<string>
#include<iostream>
#include<vector>
#include<unordered_map>

using namespace std;

class ImgScaler {

	private :

	protected :

	int scale_;

	vector<string> org_img_;

	unordered_map<string, int> org_img_store_;

	unordered_map<string, int> scl_img_store_;

	public :

	ImgScaler(int);

       ~ImgScaler();

	void read_images(const char*);

	void scale_images(void);

	void scale_image_single(string, int);

	void stat(void);
};

#endif
