#pragma once
#include "indexRecordNode.h"
#include "key.h"
#include <vector>

using namespace std;

class bplustree
{public:
	
	int availRBN;
	int root;
	vector<indexRecordNode> node_list;
	
	bplustree(indexRecordNode newRoot)
	{
		root = 0;
		availRBN = 1;
		node_list.push_back(newRoot);
	}


	void insertKey(key& newKey);

	void splitNodes(int RBN);
};

