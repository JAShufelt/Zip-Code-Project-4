#include "bplusgenerate.h"
#include "bplustree.h"
#include <vector>
#include <iostream>
#include "buffer.h"

using namespace std;

vector<key> generateKeys(string filename);

int main(int arc, char* argv[])
{
	vector<key> newKeys = generateKeys("us_postal_codes_BLOCKED.txt");
	
	indexRecordNode initialRoot = indexRecordNode(true,0);	//Construct an intitial root node
	initialRoot.parentKeyRBN = -1;	//Set root node's parent to -1 cause it aint got no daddy
	initialRoot.keyVector.push_back(newKeys[0]);	//Add an intial key to the new root
	
	bplustree testTree = bplustree(initialRoot);	//Construct the new tree containing a single node with the first key in it.

	for(int i = 1; i < 1000; i++)
	{
		testTree.insertKey(newKeys[i]);
	}
	
	for(int i = 0; i < testTree.node_list.size(); i++)
	{
		cout << "RBN: " << i << ", Parent RBN: " << testTree.node_list[i].parentKeyRBN <<endl;
		
		for(int j = 0; j < testTree.node_list[i].keyVector.size(); j++)
		{
			cout << testTree.node_list[i].keyVector[j].primaryKey << ",";
		}
		
		cout << "\n\n";
	}
	
	
	
	
	testTree.root = 0;
	testTree.node_list[0].RBN = testTree.root;
	testTree.node_list[0].keyVector[0].RBN = 0;

	return 0;
}

vector<key> generateKeys(string filename)
{
	int RBNtracker = 0;

	Record currentLargest;
	vector<key> allKeys;
	Buffer buff(filename);

	for (int i = 0; i < 22; i++)	//Skip past the header of the Blocked Data File
	{
		buff.read();
	}

	while (!buff.reader.eof())	//Unpack all the records from the data file into a buffer's record vector
	{
		buff.read();
		RBNtracker++;

		if (buff.buffer_storage != "")	//If we haven't met the end of the file yet. 
		{
			buff.unpack(buff.buffer_storage);	//Unpack the block into buff.recordVector

			currentLargest = buff.recordVector[0];
			for (int i = 1; i < buff.recordVector.size(); i++)	//Determine the largest zip code held in the block. 
			{
				if (currentLargest.zipCode < buff.recordVector[i].zipCode)
				{
					currentLargest = buff.recordVector[i];
				}
			}

			key newKey = key(currentLargest.zipCode);
			newKey.childRBN = RBNtracker;
			allKeys.push_back(newKey);
			buff.recordVector.clear();
		}
	}
	cout << buff.buffer_storage;
	return allKeys;
}
