#include <iostream>
#include <vector>
#include <string>
#include <queue>

using namespace std;

// Class for the AVL Tree
class AVLTree
{
public:
	// Node properties
	struct Node
	{
		string studentName;

		string gatorID;

		Node* left = nullptr;

		Node* right = nullptr;

		int balanceFactor = 0;
	};

	// Initializing the root node to null
	Node* root = nullptr;

	// Vector to store tree and assist w/ inorder functions
	vector<Node*> treeVector;

	// Function prototypes

	// Insert function
	bool insert(string& studentName, string& gatorID);

	// Remove function
	bool remove(string& gatorID);

	// Search Gator ID function
	Node* searchGatorID(Node* node, string& gatorID);

	// Search student name function
	vector<Node*> searchStudentName(Node* node, string& studentName);

	// Print Inorder function
	void printInorder(Node* root);

	// Print Preorder function
	void printPreorder(Node* root);

	// Print Postorder function
	void printPostorder(Node* root);

	// Print level count function
	void printLevelCount(Node* root);

	// Remove inorder function
	bool removeInorder(int n);

	// Pushing tree into vector for inorder functions function
	void treeIntoVector(Node* node);

	// Balancing the node function
	Node* balanceThe(Node* node);

	// Helper functions

	// Updating the balance factor function
	void updateBalanceFactor(Node* node);

	// Getter for the node's height
	int getHeight(Node* node);

	// Getter for the minimum value of a tree
	Node* getMinimum(Node* node);

	// Getter for the maximum value of a tree
	Node* getMaximum(Node* node);

private:
	// More function prototypes

	// Left rotation function
	Node* rotateLeft(Node* node);

	// Right rotation function
	Node* rotateRight(Node* node);

	// Left right rotation function
	Node* rotateLeftRight(Node* node);

	// Right left rotation function
	Node* rotateRightLeft(Node* node);

	// Helper functions

	// Insert helper function
	Node* insertHelper(Node* node, string& studentName, string& gatorID);

	// Remove helper function
	Node* removeHelper(Node* node, string& gatorID);
};

// Insert function
bool AVLTree::insert(string& studentName, string& gatorID)
{
	// Ensuring that it doesn't already exist as IDs are unique to each student
	if (searchGatorID(root, gatorID) != nullptr)
	{
		return false;
	}
	else
	{
		// Using the insert helper function
		root = insertHelper(root, studentName, gatorID);

		// Balancing the root
		root = balanceThe(root);

		// Updating the root's balance factor
		updateBalanceFactor(root);

		return true;
	}
}

// Remove function
bool AVLTree::remove(string& gatorID)
{
	// Ensuring that it already doesn't exist, no need to remove something that doesn't exist
	if (searchGatorID(root, gatorID) == nullptr)
	{
		return false;
	}
	else
	{
		// Using the remove helper function
		root = removeHelper(root, gatorID);

		// Balancing the root
		root = balanceThe(root);

		// Updating the root's balance factor
		updateBalanceFactor(root);

		return true;
	}
}

// Search Gator ID function
AVLTree::Node* AVLTree::searchGatorID(Node* node, string& gatorID)
{
	// Null check
	if (node == nullptr)
	{
		return nullptr;
	}
	// Go to the left if less since it's a BST
	else if (gatorID < node->gatorID)
	{
		// Repeat using recursion
		return searchGatorID(node->left, gatorID);
	}
	// Go to the right if more since it's a BST
	else if (gatorID > node->gatorID)
	{
		// Repeat using recursion
		return searchGatorID(node->right, gatorID);
	}
	else
	{
		return node;
	}
}

// Search student name function
vector<AVLTree::Node*> AVLTree::searchStudentName(Node* node, string& studentName)
{
	// Vector to store the names, students might have the same names
	vector<AVLTree::Node*> names;

	// Making sure the vector is empty before it is filled with a tree
	treeVector.clear();
	
	// Push the tree into the vector
	treeIntoVector(root);

	// Traverse through the vector containing the tree, if a name is the same, push it into the vector that will store the names
	for (int i = 0; i < treeVector.size(); i++)
	{
		if (treeVector.at(i)->studentName == studentName)
		{
			names.push_back(treeVector.at(i));
		}
	}

	// Returning the names
	return names;
}

// Print Inorder function
void AVLTree::printInorder(Node* root)
{
	// Null check
	if (root == nullptr)
	{
		return;
	}
	// Inorder -> Left, Root, Right
	else
	{
		// Left
		printInorder(root->left);
		
		// Root
		cout << root->studentName;

		if (root != getMaximum(this->root))
		{
			cout << ", ";
		}
		else
		{
			cout << endl;
		}

		// Right
		printInorder(root->right);
	}
}

// Print Preorder function
void AVLTree::printPreorder(Node* root)
{
	// Null check
	if (root == nullptr)
	{
		return;
	}
	// Preorder -> Root, Left, Right
	else
	{
		// Root
		cout << root->studentName;

		if (root != getMaximum(this->root))
		{
			cout << ", ";
		}
		else
		{
			if (root->left == nullptr)
			{
				cout << endl;
			}
			else
			{
				cout << ", " << root->left->studentName << endl;
			}

			return;
		}

		// Left
		printPreorder(root->left);

		// Right
		printPreorder(root->right);
	}
}

// Print Postorder function
void AVLTree::printPostorder(Node* root)
{
	// Null check
	if (root == nullptr)
	{
		return;
	}
	// Postorder -> Left, Right, Root
	else
	{
		// Left
		printPostorder(root->left);

		// Right
		printPostorder(root->right);
		
		// Root
		cout << root->studentName;

		if (root != this->root)
		{
			cout << ", ";
		}
		else
		{
			cout << endl;
		}
	}
}

// Print level count function
void AVLTree::printLevelCount(Node* root)
{
	// Variable to track total number of levels
	int numberOfLevels = 0;

	// Storing levels of the tree into a queue
	queue<Node*> treeLevels;

	// If not null, push left and right nodes into the queue and increase the number of levels by 1
	if (root != nullptr)
	{
		if (root->left != nullptr)
		{
			treeLevels.push(root->left);
		}

		if (root->right != nullptr)
		{
			treeLevels.push(root->right);
		}

		numberOfLevels++;
	}
	else
	{
		cout << numberOfLevels << endl;

		return;
	}

	// While the queue isn't empty, pop them and add 1 to the number of levels variable for each
	while (!treeLevels.empty())
	{
		int stopHere = treeLevels.size();

		// Go through the queue and push the front node's left and right nodes into the queue and then pop it
		for (int i = 0; i < stopHere; i++)
		{
			if (treeLevels.front()->left != nullptr)
			{
				treeLevels.push(treeLevels.front()->left);
			}

			if (treeLevels.front()->right != nullptr)
			{
				treeLevels.push(treeLevels.front()->right);
			}

			treeLevels.pop();
		}

		// Add 1 to the total number of levels
		numberOfLevels++;
	}

	cout << numberOfLevels << endl;
}

// Remove inorder function
bool AVLTree::removeInorder(int n)
{
	// Pushing the tree into the vector
	treeIntoVector(root);

	// If empty or too many, don't do it
	if (n < 0 || n > treeVector.size() - 1)
	{
		return false;
	}

	return remove(treeVector.at(n)->gatorID);
}

// Pushing tree into vector for inorder functions function
void AVLTree::treeIntoVector(Node* node)
{
	// Null check
	if (node == nullptr)
	{
		return;
	}
	// Using inorder traversal to push the tree into the vector -> Left, Root, Right
	else
	{
		// Left
		treeIntoVector(node->left);

		// Root
		treeVector.push_back(node);

		// Right
		treeIntoVector(node->right);
	}
}

// Balancing the node function
AVLTree::Node* AVLTree::balanceThe(Node* node)
{
	// Null check
	if (node == nullptr)
	{
		return node;
	}

	// Make sure the balance factor is up to date
	updateBalanceFactor(root);

	// Repeat on left node using recursion
	node->left = balanceThe(node->left);

	// Repeat on right node using recursion
	node->right = balanceThe(node->right);

	// Make sure the balance factor is up to date
	updateBalanceFactor(root);

	// Ensuring that the node is balanced (-1,  0, or 1) and if not, balance it
	if (abs(node->balanceFactor) > 1)
	{
		// LL Case = Right rotation
		if (node->balanceFactor == 2 && node->left->balanceFactor == 1)
		{
			node = rotateRight(node);
		}
		// RR Case = Left rotation
		else if (node->balanceFactor == -2 && node->right->balanceFactor == -1)
		{
			node = rotateLeft(node);
		}
		// LR Case = Left Right rotation
		else if (node->balanceFactor == 2 && node->left->balanceFactor == -1)
		{
			node = rotateLeftRight(node);
		}
		// RL Case = Right Left rotation
		else if (node->balanceFactor == -2 && node->right->balanceFactor == 1)
		{
			node = rotateRightLeft(node);
		}
	}

	return node;
}

// Updating the balance factor function
void AVLTree::updateBalanceFactor(Node* node)
{
	// Null check
	if (node == nullptr)
	{
		return;
	}
	// Doing root, then left, then right = preorder traversal
	else
	{
		// Getting balance factor of root by subtracting left and right subtrees
		node->balanceFactor = getHeight(node->left) - getHeight(node->right);

		// Repeat on left using recursion
		updateBalanceFactor(node->left);

		// Repeat on right using recursion
		updateBalanceFactor(node->right);
	}
}

// Getter for the node's height
int AVLTree::getHeight(Node* node)
{
	// Null check
	if (node == nullptr)
	{
		return 0;
	}

	// No children = height of 1
	if (node->left == nullptr && node->right == nullptr)
	{
		return 1;
	}
	// If left node null, go right
	else if (node->left == nullptr)
	{
		// Take account of previous node by adding 1
		return getHeight(node->right) + 1;
	}
	// If right node null, go left
	else if (node->right == nullptr)
	{
		// Take account of previous node by adding 1
		return getHeight(node->left) + 1;
	}
	else
	{
		return max(getHeight(node->left), getHeight(node->right)) + 1;
	}
}

// Getter for the minimum value of a tree
AVLTree::Node* AVLTree::getMinimum(Node* node)
{
	// If it's left is null, the current node is the minimum
	if (node->left == nullptr)
	{
		return node;
	}
	// Else, continue to the left until the left node is null using recursion
	else
	{
		return getMinimum(node->left);
	}
}

// Getter for the maximum value of a tree
AVLTree::Node* AVLTree::getMaximum(Node* node)
{
	// If it's null, the current node is the maximum
	if (node->right == nullptr)
	{
		return node;
	}
	// Else, continue to the right until the right node is null using recursion
	else
	{
		return getMaximum(node->right);
	}
}

// Rotations

// Left rotation function for Right Right cases
AVLTree::Node* AVLTree::rotateLeft(Node* node)
{
	// Manipulating node pointers

	// Set the grandchild node to be the node's right node's left node
	Node* gChild = node->right->left;

	// Set the parent node to be the node's right node
	Node* parent = node->right;

	// Set the node's right node to be the grandchild node
	node->right = gChild;

	// Set the parent node's left node to be the node
	parent->left = node;

	return parent;
}

// Right rotation function for Left Left cases
AVLTree::Node* AVLTree::rotateRight(Node* node)
{
	// Manipulating node pointers

	// Set the grandchild node to be the node's left node's right node
	Node* gChild = node->left->right;
	
	// Set the parent node to be the node's left node
	Node* parent = node->left;

	// Set the node's left node to be the grandchild node
	node->left = gChild;

	// Set the parent's right node to be the node
	parent->right = node;

	return parent;
}

// Left right rotation function for Left Right cases
AVLTree::Node* AVLTree::rotateLeftRight(Node* node)
{
	// Manipulating node pointers, just doing left and right rotations back to back

	// Set the grandchild node to be the node's left node
	Node* gChild = node->left;

	// Set the updated left node to be the grandchild node's right node
	Node* updatedLeft = gChild->right;

	// Set the grandchild node's right node to be the updated left node's left node
	gChild->right = updatedLeft->left;

	// Set the node's left node to be the updated left node
	node->left = updatedLeft;

	// Set the node's left node's left node to be the grandchild node
	node->left->left = gChild;

	// After the left rotation above is done, we do a right rotation
	return rotateRight(node);
}

// Right left rotation function for Right Left cases
AVLTree::Node* AVLTree::rotateRightLeft(Node* node)
{
	// Manipulating node pointers, just doing right and left rotations back to back

	// Set the grandchild node to be the node's right node's left node
	Node* gChild = node->right->left;

	// Set the node's right node's left node to be the grandchild node's right node
	node->right->left = gChild->right;

	// Set the grandchild node's right node to be the node's right node
	gChild->right = node->right;

	// Set the node's right node to be the grandchild node
	node->right = gChild;

	// After the right rotation above is done, we do a left rotation
	return rotateLeft(node);
}

// Insert helper function
AVLTree::Node* AVLTree::insertHelper(Node* node, string& studentName, string& gatorID)
{
	// Null check, if null, whip out a new node
	if (node == nullptr)
	{
		// Making a new node
		node = new Node;

		node->studentName = studentName;

		node->gatorID = gatorID;

		// Return the freshly baked node
		return node;
	}
	// Go to the left if less because this is a BST
	else if (gatorID < node->gatorID)
	{
		// Repeat using recursion until the ID is no longer less than the node on it's left
		node->left = insertHelper(node->left, studentName, gatorID);
	}
	// Go to the right if more because this is a BST
	else
	{
		// Repeat using recursion until the ID is no longer more than the node on it's right
		node->right = insertHelper(node->right, studentName, gatorID);
	}

	return node;
}

// Remove helper function
AVLTree::Node* AVLTree::removeHelper(Node* node, string& gatorID)
{
	// Null check, if null, return the null
	if (node == nullptr)
	{
		return node;
	}
	// Go to the left if less because this is a BST
	if (gatorID < node->gatorID)
	{
		// Repeat using recursion until the ID is no longer less than the node on it's left
		node->left = removeHelper(node->left, gatorID);
		
		return node;
	}
	// Go to the right if more because this is a BST
	else if (gatorID > node->gatorID)
	{
		// Repeat using recursion until the ID is no longer more than the node on it's right
		node->right = removeHelper(node->right, gatorID);

		return node;
	}
	// For when gatorID and node->gatorID are equal to each other
	else
	{
		// Set a temporary node to be the node
		Node* temporaryNode = node;

		// If it's left node is null, delete the node, set the node to be the temporary node's right node
		if (temporaryNode->left == nullptr)
		{
			delete node;

			node = temporaryNode->right;

			return node;
		}
		// If it's right node is null, delete the node, set the node to be the temporary node's left node
		else if (temporaryNode->right == nullptr)
		{
			delete node;

			node = temporaryNode->left;

			return node;
		}
		// Else, remove the node's right node's minimum value node
		else
		{
			temporaryNode = getMinimum(node->right);

			node->studentName = temporaryNode->studentName;

			node->gatorID = temporaryNode->gatorID;

			node->right = removeHelper(node->right, temporaryNode->gatorID);

			return node;
		}
	}
}

// Ensuring the student name is good and usable
bool checkStudentName(string& studentName)
{
	// Ensuring that the student name is not blank and is only made up of letters
	for (int i = 0; i < studentName.size(); i++)
	{
		if (!isblank(studentName[i]) && !isalpha(studentName[i]))
		{
			return false;
		}
	}

	return true;
}

// Ensuring the Gator ID is good and usable
bool checkGatorID(string& gatorID)
{
	// Ensuring the Gator ID's size is 8 as it should be
	if (gatorID.size() != 8)
	{
		return false;
	}

	// Ensuring that the Gator ID is only made up of digits
	for (int i = 0; i < gatorID.size(); i++)
	{
		if (!isdigit(gatorID[i]))
		{
			return false;
		}
	}

	return true;
}

// Main method
int main()
{
	// Instantiating the AVL Tree
	AVLTree tree;

	// Variable to store the student's name
	string studentName;

	// Variable to store the student's Gator ID
	string gatorID;

	// Number of commands that the user will enter
	int numberOfUserCommands = 0;

	// Variable to store the command that the user will enter
	string userCommand;

	// Getting the number of commands that the user will enter
	cin >> numberOfUserCommands;

	// Program will stop once the number of user commands is reached
	for (int i = 0; i < numberOfUserCommands; i++)
	{
		cin >> userCommand;

		if (userCommand == "insert")
		{
			string str;

			// Parsing the input
			getline(cin, str, '\"');
			getline(cin, studentName, '\"');
			
			// Storing the gatorID
			cin >> gatorID;

			// Checking the input and printing the appropriate message
			if (checkStudentName(studentName) && checkGatorID(gatorID))
			{
				if (tree.insert(studentName, gatorID))
				{
					cout << "successful" << endl;
				}
				else
				{
					cout << "unsuccessful" << endl;
				}
			}
			else
			{
				cout << "unsuccessful" << endl;
			}
		}
		else if (userCommand == "remove")
		{
			// Storing the gatorID
			cin >> gatorID;

			// Checking the input and printing the appropriate message
			if (checkGatorID(gatorID))
			{
				if (tree.remove(gatorID))
				{
					cout << "successful" << endl;
				}
				else
				{
					cout << "unsuccessful" << endl;
				}
			}
			else
			{
				cout << "unsuccessful" << endl;
			}
		}
		else if (userCommand == "search")
		{
			string itemToBeSearched;

			string str;

			// Parsing the input
			getline(cin, str, ' ');
			getline(cin, itemToBeSearched);

			// Checking a character from the input, if it's a letter, search for name, else search for gatorID
			if (isalpha(itemToBeSearched[1]))
			{
				//  Getting the student name
				studentName = itemToBeSearched.substr(1, itemToBeSearched.size() - 2);

				// Ensuring the student name is valid
				if (checkStudentName(studentName))
				{
					// Searching for the student name and storing it in a vector
					vector<AVLTree::Node*> names = tree.searchStudentName(tree.root, studentName);

					// If the vector is not empty, the Gator IDs will be printed, else print the unsuccessful message
					if (!names.empty())
					{
						for (int j = 0; j < names.size(); j++)
						{
							cout << names.at(j)->gatorID << endl;
						}
					}
					else
					{
						cout << "unsuccessful" << endl;
					}
				}
				else
				{
					cout << "unsuccessful" << endl;
				}
			}
			// Searching for Gator ID if there is a number and not letter
			else
			{
				// Storing the Gator ID
				gatorID = itemToBeSearched;

				// Ensuring the Gator ID is valid
				if (checkGatorID(gatorID))
				{
					// Searching for the Gator ID and storing it in a vector
					AVLTree::Node* nodes = tree.searchGatorID(tree.root, gatorID);

					// If the vector is not empty, the student name will be printed (as each student has a unique Gator ID), else print the unsuccessful message
					if (nodes != nullptr)
					{
						cout << nodes->studentName << endl;
					}
					else
					{
						cout << "unsuccessful" << endl;
					}
				}
				else
				{
					cout << "unsuccessful" << endl;
				}
			}
		}
		else if (userCommand == "printInorder")
		{
			tree.printInorder(tree.root);
		}
		else if (userCommand == "printPreorder")
		{
			tree.printPreorder(tree.root);
		}
		else if (userCommand == "printPostorder")
		{
			tree.printPostorder(tree.root);
		}
		else if (userCommand == "printLevelCount")
		{
			tree.printLevelCount(tree.root);
		}
		else if (userCommand == "removeInorder")
		{
			int n = 0;

			cin >> n;

			// Printing the appropriate message after the operation
			if (tree.removeInorder(n))
			{
				cout << "successful" << endl;
			}
			else
			{
				cout << "unsuccessful" << endl;
			}
		}
	}

	// End the program
	return 0;
}