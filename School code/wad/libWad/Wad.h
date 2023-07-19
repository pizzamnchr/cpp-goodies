#pragma once

#include <iostream>

#include <fstream>

#include <cstring>

#include <string>

#include <vector>

using namespace std;

// Wad class, used for representing WAD data
class Wad
{
public:
	// Defining a struct for representing nodes, aka files + directories, in the WAD filesystem
	struct Node
	{
		// Initializing the offset of the node
		int nodeOffset;

		// Initializing the length of the node
		int nodeLength;

		// Initializing the name of the node
		string nodeName;

		// Initializing the parent of the node
		Node* nodeParent;

		// Initializing the content of the node
		char* nodeContent;

		// Initializing the vector that will contain all the children of the node
		vector<Node*> nodeChildren;

		// Declaring the constructor for the node
		//Node(int _nodeOffset, int _nodeLength, string _nodeName, Node* _nodeParent);

		Node(string _nodeName, int _nodeOffset, int _nodeLength, Node* _nodeParent);
		
		// Declaring a function for getting the parent of the node
		Node* getParent();

		// Declaring a function for getting the children of the node
		Node* getChildren(string _nodeName);
	};

	// Constructor for the Wad class
	Wad();

	// Main library functions

	// Declaring a function for creating a Wad obj + loading the WAD file data from the given path into memory
	static Wad* loadWad(const string &path);

	// Declaring a function for returning the magic, aka file type, of the WAD data
	string getMagic();

	// Declaring a function for checking if a path represents content (data), returning true if yes, and false otherwise
	bool isContent(const string &path);

	// Declaring a function for checking if a path represents a directory, returning true if yes, and false otherwise
	bool isDirectory(const string &path);

	// Declaring a function for returning the number of bytes in the data, and -1 otherwise
	int getSize(const string &path);

	// Declaring a function for copying as many bytes as possible, up to the length, of the content's data into the pre-existing buffer + returning the # of bytes copied into buffer, or -1 if the path does not represent content (e.g., if it represents a directory)
	int getContents(const string &path, char *buffer, int length, int offset = 0);

	// Declaring a function for getting a directory, returning the # of elements in the directory, or -1 if the path does not represent a directory (e.g., if it represents content)
	int getDirectory(const string &path, vector<string> *directory);


	// Helper functions that the main library functions will use

	// Declaring a function for getting the name of a directory in a WAD file
	string getNameOfDirectory(string &_wadFile);

	// Declaring a function to be used for checking if something is the start of a directory in a WAD file
	bool thisIsTheStartOfADirectory(string &_wadFile);
	
	// Declaring a function to be used for checking if something is the end of a directory in a WAD file
	bool thisIsTheEndOfADirectory(string &_wadFile);

	// Declaring a function to be used for checking if something is a map marker in a WAD file
	bool thisIsAMapMarker(string &_wadFile);

	// Declaring a function for getting the root node
	Node* getRootNode();

	// Declaring a function for looking inside a directory
	Node* lookInsideDirectory(string _fileName, Node* _node);

	// Declaring a function for loading files + their content
	void loadFiles(Node* _node);

	// Declaring a function for removing the delimiters from the WAD data
	void deleteWadDataDelimiters(const string &_path, vector<string> &_wadPath);

	// Declaring a function for adding a file to the given directory
	void addToDirectory(int _nodeOffset, int _nodeLength, string _nodeName, Node* _directory);

	// Declaring a function for displaying a node (file) + its children
	void display(Node* _node);

	// Declaring a function for storing the WAD data's magic + WAD file
	void store(const string &_path, string _magic);

	// Declaring a function for deleting a node (file) + its children
	void clear(Node* _node);

private:
	// Declaring the root node
	Node* rootNode;

	// Declaring a string for the magic, aka file type, in a WAD file
	string magic;

	// Declaring a string for the WAD file
	string wadFile;
};

