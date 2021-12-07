#pragma once
#include "key.h"
#include <vector>

using namespace std;

class indexRecordNode
{public:
	bool leaf_flag;
	int parentKeyRBN;
	int RBN;
	
	vector<key> keyVector;

	indexRecordNode(bool leaf, int self_RBN)
	{
		leaf_flag = leaf;
		RBN = self_RBN;
	}

};



