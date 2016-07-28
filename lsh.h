#ifndef _LSH_H_
#define _LSH_H_

#include <iostream>
#include <random>
#include <vector>
#include <string>
#include <utility>
#include <iomanip>
#include <chrono>
#include <unordered_map>
#include <typeinfo>
#include <functional>
#include <algorithm>

#include "pHash.h"

using namespace std;

namespace lsh {

	struct hash_name {

		int seed_;

		hash_name(int arg) { seed_ = arg; };

		size_t operator()(const string &name ) const {
			return hash<string>()(name) * seed_;
		}
	};

	static vector<unordered_multimap<string, pair<string, ulong64>, hash_name>*> v_table_;

	static void init_hash_tables(int hash_no) {

		for (int i = 0; i < hash_no; i++) {
			auto p = new unordered_multimap<string, pair<string, ulong64>, struct hash_name>(10, hash_name(i));
			v_table_.push_back(p);
		}
	}

	static void insert_bin_vector(string p, pair<string, ulong64> q, unordered_multimap<string, pair<string, ulong64>, hash_name>* table_) {

		table_->insert({ p, q });
	}

	static vector<ulong64> rp_;

	static void generate_random_set(int nrolls, ulong64 min, ulong64 max) {

		unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();

		std::default_random_engine generator (seed);

		std::uniform_int_distribution<ulong64> distribution(min, max);

		rp_.clear();
		for (int i=0; i<nrolls; ++i) {
			int number = distribution(generator);
			rp_.push_back(number);
		}
	}

	static const int threshold = 30;

	static string generate_bin_vector(ulong64 val) {

		string ans;
		for (auto& i : rp_) {
			int distance = ph_hamming_distance(val, i);
			if (distance > threshold)
				ans.append("1");
			else
				ans.append("0");
		}
		return ans;
	}

	static void process_near_duplicate(vector<string>& dup, int threshold) {

		size_t n = 0;
		vector<pair<string, pair<string, ulong64>>> v;

		for (auto& k : v_table_) {

			for (auto& i : *k) {
				if (k->count(i.first) == 1)
					continue;
				auto it_bounds = k->equal_range(i.first);

				for (auto ii = it_bounds.first; ii != it_bounds.second; ii++)
					v.push_back(make_pair(ii->first, ii->second));

				for (int p = 0; p < v.size(); p++) {
					for (int q = p + 1; q < v.size(); q++) {
						int distance = ph_hamming_distance(v.at(p).second.second, v.at(q).second.second);
						if (find(dup.begin(), dup.end(), v.at(q).second.first) != dup.end())
							continue;
						if (distance < threshold) {
							dup.push_back(v.at(q).second.first);
							cout << "[" << n++ << "]" << "hamming dist\t" << distance << "\t" << v.at(p).second.first << "\t"
                                                                                                                          << v.at(q).second.first << endl;
						}
					}
				}
				v.clear();
			}
		}

		cout << "=======Duplicates=== " << endl;
		for (auto&i : dup)
			cout << i << " " << endl;
	}
};

#endif
