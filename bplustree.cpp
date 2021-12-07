#include "bplustree.h"
#include <iostream>
using namespace std;

void bplustree::insertKey(key &newkey)
{
	int MAX_DEG = 46;
	bool leaf_Found = false;
	bool key_inserted = false;
	indexRecordNode* currentNode = &node_list[root];

	while (!(leaf_Found && key_inserted))		//First navigate to the proper leaf, and then insert a new key
	{
		if (currentNode->leaf_flag == true)
		{
			newkey.parentKeyRBN = currentNode->parentKeyRBN;
			currentNode->keyVector.push_back(newkey);
			leaf_Found = true;
			key_inserted = true;

			if(currentNode->parentKeyRBN != -1)	//If the newly added key is the biggest key in the node, promote it as the parent's primary key.
			{
				if (currentNode->keyVector.size() > 1 && (newkey.primaryKey > currentNode->keyVector[currentNode->keyVector.size()-2].primaryKey))	//-2 to compare latest add key to previous latest added key
				{
					indexRecordNode* currentNode_parentNode = &node_list[currentNode->parentKeyRBN];	//Create a pointer to the parent node

					for(int i = 0; i < currentNode_parentNode->keyVector.size(); i++)	//Look through parent node for the parent key
					{
						if (currentNode->keyVector[currentNode->keyVector.size() - 2].primaryKey == currentNode_parentNode->keyVector[i].primaryKey)
						{
							currentNode_parentNode->keyVector[i].primaryKey = newkey.primaryKey;
							break;
						}
					}

				}
				
			}
			
		}

		else
		{
			for(int i = 0; i < currentNode->keyVector.size(); i++)	//Determine which key holds the leaf for insertion
			{
				if(newkey.primaryKey <= currentNode->keyVector[i].primaryKey)
				{
					currentNode = &node_list[currentNode->keyVector[i].childRBN];
				}
			}
			if(newkey.primaryKey > currentNode->keyVector[currentNode->keyVector.size() - 1].primaryKey)	//Special case if the key is bigger than any currently held key.
			{
				currentNode = &node_list[currentNode->keyVector[currentNode->keyVector.size() - 1].childRBN];
			}

			if(currentNode->leaf_flag == true)
			{
				leaf_Found = true;
			}
		}
	}

	if(currentNode->keyVector.size() > MAX_DEG)
	{
		splitNodes(currentNode->RBN);
		cout << "Limit exceeded " << currentNode->keyVector.size() << endl;
	}
}

void bplustree::splitNodes(int RBN)
{
	int MAX_DEG = 46;
	int HALF = MAX_DEG / 2;
	
	indexRecordNode* currentNode = &node_list[RBN];	//Current node points to the RBN of the RBN passed to the splitNodes function.
	
	if (currentNode->keyVector.size() > MAX_DEG)	//Current node needs to be split.
	{
		//Splitting the root while it is a leaf
		if (currentNode->RBN == root && currentNode->leaf_flag == true)	//If the node we are splitting is the root AKA has no parent
		{
			cout << "Going to split da root";
			indexRecordNode newLeftNode(false, currentNode->RBN);	//Construct new left node to take the place of the old node
			indexRecordNode newRightNode(false, availRBN);			//Construct new right node, will be placed in next available RBN
			availRBN++;

			if (currentNode->leaf_flag == true)	//Check if the node we are splitting is a leaf. 
			{
				newLeftNode.leaf_flag = true;
				newRightNode.leaf_flag = true;
			}


			for (int i = 0; i < HALF; i++)	//Store the left half of the values into the newLeftNode
			{
				newLeftNode.keyVector.push_back(currentNode->keyVector[i]);
			}

			for (int i = HALF; i < currentNode->keyVector.size(); i++)	//Store the right half of the values into the newRightNode
			{
				newRightNode.keyVector.push_back(currentNode->keyVector[i]);
			}

			indexRecordNode newParent(false, availRBN);	//Initialize new parent in a free RBN
			availRBN++;

			key newLeftLargest = key(newLeftNode.keyVector[0].primaryKey);	//Initialize newLeftLargest to be the first key in the newLeftNode
			for (int i = 0; i < newLeftNode.keyVector.size(); i++)				//Increment through newLeftNode and determine largest key.
			{
				if (newLeftLargest.primaryKey < newLeftNode.keyVector[i].primaryKey)
				{
					newLeftLargest.primaryKey = newLeftNode.keyVector[i].primaryKey;
				}
			}

			key newRightLargest = key(newRightNode.keyVector[0].primaryKey);
			for (int i = 0; i < newRightNode.keyVector.size(); i++)
			{
				if (newRightLargest.primaryKey < newRightNode.keyVector[i].primaryKey)
				{
					newRightLargest.primaryKey = newRightNode.keyVector[i].primaryKey;
				}
			}

			root = newParent.RBN;
			newParent.parentKeyRBN = -1;
			
			newLeftNode.parentKeyRBN = newParent.RBN;
			newRightNode.parentKeyRBN = newParent.RBN;

			newLeftLargest.childRBN = newLeftNode.RBN;
			newRightLargest.childRBN = newRightNode.RBN;

			newLeftLargest.RBN = newParent.RBN;
			newRightLargest.RBN = newParent.RBN;

			newLeftLargest.parentKeyRBN = -1;
			newRightLargest.parentKeyRBN = -1;

			newParent.keyVector.push_back(newLeftLargest);
			newParent.keyVector.push_back(newRightLargest);

			for (int i = 0; i < newRightNode.keyVector.size(); i++)
			{
				newRightNode.keyVector[i].parentKeyRBN = newParent.RBN;
			}

			for (int i = 0; i < newLeftNode.keyVector.size(); i++)
			{
				newLeftNode.keyVector[i].parentKeyRBN = newParent.RBN;
			}

			for(int i = 0; i < newRightNode.keyVector.size(); i++)
			{
				newRightNode.keyVector[i].RBN = newRightNode.RBN;
			}

			node_list[newLeftNode.RBN] = newLeftNode;
			node_list.push_back(newRightNode);
			node_list.push_back(newParent);

		}
		
		//Splitting a leaf node that is not the root (All the keys in this node will not not need to worry about their children's parent pointers)
		else if (currentNode->RBN != root && currentNode->leaf_flag == true)
		{
			indexRecordNode newLeftNode(false, currentNode->RBN);	//Construct new left node to take the place of the old node
			indexRecordNode newRightNode(false, availRBN);			//Construct new right node, will be placed in next available RBN
			availRBN++;

			if (currentNode->leaf_flag == true)	//Check if the node we are splitting is a leaf. 
			{
				newLeftNode.leaf_flag = true;
				newRightNode.leaf_flag = true;
			}

			for (int i = 0; i < HALF; i++)	//Store the left half of the values into the newLeftNode
			{
				newLeftNode.keyVector.push_back(currentNode->keyVector[i]);
			}

			for (int i = HALF; i < currentNode->keyVector.size(); i++)	//Store the right half of the values into the newRightNode
			{
				newRightNode.keyVector.push_back(currentNode->keyVector[i]);
			}
		
			key newLeftLargest = key(newLeftNode.keyVector[0].primaryKey);	//Initialize newLeftLargest to be the largest key in the newLeftNode
			for (int i = 0; i < newLeftNode.keyVector.size(); i++)				//Increment through newLeftNode and determine largest key.
			{
				if (newLeftLargest.primaryKey < newLeftNode.keyVector[i].primaryKey)
				{
					newLeftLargest.primaryKey = newLeftNode.keyVector[i].primaryKey;
				}
			}

			key newRightLargest = key(newRightNode.keyVector[0].primaryKey);	//Initialize newRightLargest to be the largest key in the newRightNode
			for (int i = 0; i < newRightNode.keyVector.size(); i++)
			{
				if (newRightLargest.primaryKey < newRightNode.keyVector[i].primaryKey)
				{
					newRightLargest.primaryKey = newRightNode.keyVector[i].primaryKey;
				}
			}

			newLeftNode.parentKeyRBN = currentNode->parentKeyRBN;	//Assign new left node's parent, the original node's parents
			newRightNode.parentKeyRBN = currentNode->parentKeyRBN;	//Assign new right node's parent, the original node's parents

			key* currentNode_parentKey;
			for (int i = 0; i < node_list[currentNode->parentKeyRBN].keyVector.size(); i++)	//Locate the currentNode's parent key
			{
				if(node_list[currentNode->parentKeyRBN].keyVector[i].childRBN == currentNode->RBN)
				{
					currentNode_parentKey = &node_list[currentNode->parentKeyRBN].keyVector[i];
					break;
				}
			} 
			
			currentNode_parentKey->primaryKey = newLeftLargest.primaryKey;	//Replace the old parent key with the largest key of the new left node
			
			node_list[currentNode->RBN] = newLeftNode;	//Replace original node with new left node

			key newRightParentKey = key(newRightLargest.primaryKey);	//Construct a new parent key for the new right node
			newRightParentKey.childRBN = newRightNode.RBN;				//Assign new right node's parent key's child pointer RBN to new right node's RBN
			newRightParentKey.RBN = currentNode_parentKey->RBN;
			
			node_list[newRightNode.parentKeyRBN].keyVector.push_back(newRightParentKey);
			node_list.push_back(newRightNode);
			
			splitNodes(newRightNode.parentKeyRBN);
		}

		//Splitting a node that is root but not a leaf
		else if (currentNode->RBN = root && currentNode->leaf_flag == false)
		{
			indexRecordNode newLeftNode(false, currentNode->RBN);	//Construct new left node to take the place of the old node
			indexRecordNode newRightNode(false, availRBN);			//Construct new right node, will be placed in next available RBN
			availRBN++;

			for (int i = 0; i < HALF; i++)	//Store the left half of the values into the newLeftNode
			{
				newLeftNode.keyVector.push_back(currentNode->keyVector[i]);
			}

			for (int i = HALF; i < currentNode->keyVector.size(); i++)	//Store the right half of the values into the newRightNode
			{
				newRightNode.keyVector.push_back(currentNode->keyVector[i]);
			}

			indexRecordNode newParent(false, availRBN);	//Initialize new parent in a free RBN
			availRBN++;

			key newLeftLargest = key(newLeftNode.keyVector[0].primaryKey);	//Initialize newLeftLargest to be the first key in the newLeftNode
			for (int i = 0; i < newLeftNode.keyVector.size(); i++)				//Increment through newLeftNode and determine largest key.
			{
				if (newLeftLargest.primaryKey < newLeftNode.keyVector[i].primaryKey)
				{
					newLeftLargest.primaryKey = newLeftNode.keyVector[i].primaryKey;
				}
			}

			key newRightLargest = key(newRightNode.keyVector[0].primaryKey);
			for (int i = 0; i < newRightNode.keyVector.size(); i++)
			{
				if (newRightLargest.primaryKey < newRightNode.keyVector[i].primaryKey)
				{
					newRightLargest.primaryKey = newRightNode.keyVector[i].primaryKey;
				}
			}

			root = newParent.RBN;
			newParent.parentKeyRBN = -1;

			newLeftNode.parentKeyRBN = newParent.RBN;
			newRightNode.parentKeyRBN = newParent.RBN;

			newLeftLargest.childRBN = newLeftNode.RBN;
			newRightLargest.childRBN = newRightNode.RBN;

			newLeftLargest.RBN = newParent.RBN;
			newRightLargest.RBN = newParent.RBN;

			newLeftLargest.parentKeyRBN = -1;
			newRightLargest.parentKeyRBN = -1;

			for (int i = 0; i < newRightNode.keyVector.size(); i++)
			{
				newRightNode.keyVector[i].parentKeyRBN = newParent.RBN;
			}

			for (int i = 0; i < newLeftNode.keyVector.size(); i++)
			{
				newLeftNode.keyVector[i].parentKeyRBN = newParent.RBN;
			}

			for (int i = 0; i < newRightNode.keyVector.size(); i++)
			{
				newRightNode.keyVector[i].RBN = newRightNode.RBN;
			}

			for (int i = 0; i < newRightNode.keyVector.size(); i++)
			{
				node_list[newRightNode.keyVector[i].childRBN].parentKeyRBN = newRightNode.RBN;
			}

			newParent.keyVector.push_back(newLeftLargest);
			newParent.keyVector.push_back(newRightLargest);

			node_list[newLeftNode.RBN] = newLeftNode;
			node_list.push_back(newRightNode);
			node_list.push_back(newParent);

		}

		//Splitting a node that is not a leaf and is not a root
		else if (currentNode->RBN != root && currentNode ->leaf_flag == false)
		{
			indexRecordNode newLeftNode(false, currentNode->RBN);	//Construct new left node to take the place of the old node
			indexRecordNode newRightNode(false, availRBN);			//Construct new right node, will be placed in next available RBN
			availRBN++;

			for (int i = 0; i < HALF; i++)	//Store the left half of the values into the newLeftNode
			{
				newLeftNode.keyVector.push_back(currentNode->keyVector[i]);
			}

			for (int i = HALF; i < currentNode->keyVector.size(); i++)	//Store the right half of the values into the newRightNode
			{
				newRightNode.keyVector.push_back(currentNode->keyVector[i]);
			}

			key newLeftLargest = key(newLeftNode.keyVector[0].primaryKey);	//Initialize newLeftLargest to be the largest key in the newLeftNode
			for (int i = 0; i < newLeftNode.keyVector.size(); i++)				//Increment through newLeftNode and determine largest key.
			{
				if (newLeftLargest.primaryKey < newLeftNode.keyVector[i].primaryKey)
				{
					newLeftLargest.primaryKey = newLeftNode.keyVector[i].primaryKey;
				}
			}

			key newRightLargest = key(newRightNode.keyVector[0].primaryKey);	//Initialize newRightLargest to be the largest key in the newRightNode
			for (int i = 0; i < newRightNode.keyVector.size(); i++)
			{
				if (newRightLargest.primaryKey < newRightNode.keyVector[i].primaryKey)
				{
					newRightLargest.primaryKey = newRightNode.keyVector[i].primaryKey;
				}
			}

			newLeftNode.parentKeyRBN = currentNode->parentKeyRBN;	//Assign new left node's parent, the original node's parents
			newRightNode.parentKeyRBN = currentNode->parentKeyRBN;	//Assign new right node's parent, the original node's parents

			key* currentNode_parentKey;
			for (int i = 0; i < node_list[currentNode->parentKeyRBN].keyVector.size(); i++)	//Locate the currentNode's parent key
			{
				if (node_list[currentNode->parentKeyRBN].keyVector[i].childRBN == currentNode->RBN)
				{
					currentNode_parentKey = &node_list[currentNode->parentKeyRBN].keyVector[i];
					break;
				}
			}

			for (int i = 0; i < newRightNode.keyVector.size(); i++)
			{
				node_list[newRightNode.keyVector[i].childRBN].parentKeyRBN = newRightNode.RBN;
			}
			
			currentNode_parentKey->primaryKey = newLeftLargest.primaryKey;	//Replace the old parent key with the largest key of the new left node
			node_list[currentNode->RBN] = newLeftNode;	//Replace original node with new left node

			key newRightParentKey = key(newRightLargest.primaryKey);	//Construct a new parent key for the new right node
			newRightParentKey.childRBN = newRightNode.RBN;				//Assign new right node's parent key's child pointer RBN to new right node's RBN
			newRightParentKey.RBN = currentNode_parentKey->RBN;

			
			node_list[newRightNode.parentKeyRBN].keyVector.push_back(newRightParentKey);
			node_list.push_back(newRightNode);

			splitNodes(newRightNode.parentKeyRBN);

		}
	
	}
}