#include "Wad.h"

// Defining the constructor for the node
Wad::Node::Node(string _nodeName, int _nodeOffset, int _nodeLength, Node* _nodeParent)
{
	// Setting the node's offset
	this->nodeOffset = _nodeOffset;

	// Setting the node's length
	this->nodeLength = _nodeLength;

	// Setting the node's name
	this->nodeName = _nodeName;

	// Setting the node's parent
	this->nodeParent = _nodeParent;

	// Setting the node's content
	nodeContent = new char[_nodeLength];
}

// Defining a function for getting the parent of the node
Wad::Node* Wad::Node::getParent()
{
	// Just returning the parent of the node
	return nodeParent;
}

// Defining a function for getting the children of the node
Wad::Node* Wad::Node::getChildren(string _nodeName)
{
	// Iterating through the children of the node
	for (int i = 0; i < nodeChildren.size(); i += 1)
	{
		// If their names are correct
		if (nodeChildren.at(i)->nodeName == _nodeName)
		{
			// Returning the children of the node
			return nodeChildren.at(i);
		}
	}

	return nullptr;
}

// Defining the constructor for the Wad class
Wad::Wad()
{
	// Setting the WAD data's root node
	rootNode = new Node("/", 0, 0, nullptr);
}

// Defining a function for creating a Wad obj + loading the WAD file data from the given path into memory
Wad* Wad::loadWad(const string &path)
{
	// Creating the wad system
	Wad* wadSys = new Wad(); 

	// Declaring the magic (file type) of the file
	string fileMagic;

	// Declaring the number of descriptors in the file
	int fileDescripterAmount;

	// Declaring the descriptor offset of the file
	int fileDescripterOffset;

	// Setting the root node
	Node* myRoot = wadSys->getRootNode();
	
	// Setting the root directory
	Wad::Node* myDir = myRoot;

	// Reading the input file
	ifstream myInputFile(path, ios::binary);

	// If we can open the file
	if (myInputFile.is_open())
	{
		// Reading the file's magic
		for (int i = 0; i < 4; i += 1)
		{
			// Creating a temporary character
			char myTemp;

			// Using the temporary character to read the file's header
			myInputFile.read((char*)&myTemp, sizeof(myTemp));

			// Retrieving the file's magic
			fileMagic.push_back(myTemp);
		}

		// Reading the file's descripter amount
		myInputFile.read((char*)&fileDescripterAmount, sizeof(fileDescripterAmount));

		// Reading the file's descripter offset
		myInputFile.read((char*)&fileDescripterOffset, sizeof(fileDescripterOffset));

		// Storing the file's magic + name
		wadSys->store(path, fileMagic);

		// Moving to the file's offset
		myInputFile.seekg(fileDescripterOffset);

		// Reading the file
		for (int i = 0; i < fileDescripterAmount; i += 1)
		{
			// Declaring the offset
			int myOffset;

			// Declaring the length
			int myLength;

			// Declaring the name
			string myName;

			// Reading the offset
			myInputFile.read((char*)&myOffset, sizeof(myOffset));

			// Reading the length
			myInputFile.read((char*)&myLength, sizeof(myLength));

			for (int j = 0; j < 8; j += 1)
			{
				// Creating a temporary character
				char myTemp;

				// Using the temporary character to read the file
				myInputFile.read((char*)&myTemp, sizeof(myTemp));

				// If it's valid
				if (isalpha(myTemp) || isdigit(myTemp) || ispunct(myTemp))
				{
					// Retrieving the file's name
					myName.push_back(myTemp);
				}
			}
			
			// If this is the start of a directory
			if (wadSys->thisIsTheStartOfADirectory(myName))
			{
				// Setting the working directory's name
				string myDirName = wadSys->getNameOfDirectory(myName);

				// Adding the directory
				wadSys->addToDirectory(myOffset, myLength, myDirName, myDir);

				// Setting the children to be the working directory
				myDir = myDir->getChildren(myDirName);
			}
			// Else if this is the end of a directory
			else if (wadSys->thisIsTheEndOfADirectory(myName))
			{
				// Setting the parent to be the working directory
				myDir = myDir->getParent();
			}
			// Else if this is a map marker
			else if (wadSys->thisIsAMapMarker(myName))
			{
				// Declaring a limit for the following ten map element descriptors
				int mapElemDescriptors = i + 10;

				// Adding the map marker to the working directory
				wadSys->addToDirectory(myOffset, myLength, myName, myDir);

				// Setting the map marker to be the working directory
				myDir = myDir->getChildren(myName);

				// Iterating through each map element descriptor
				for (i; i < mapElemDescriptors; i += 1)
				{
					// Declaring the map element descriptor's offset
					int mapElemOffset;

					// Declaring the map element descriptor's length
					int mapElemLength;

					// Declaring the map element descriptor's name
					string mapElemName;

					// Using the map element descriptor's offset to read the file
					myInputFile.read((char*)&mapElemOffset, sizeof(mapElemOffset));

					// Using the map element descriptor's length to read the file
					myInputFile.read((char*)&mapElemLength, sizeof(mapElemLength));

					for (int j = 0; j < 8; j += 1)
					{
						// Creating a temporary character
						char myTempTwo;

						// Using the temporary character to read the file
						myInputFile.read((char*)&myTempTwo, sizeof(myTempTwo));

						if (isalpha(myTempTwo) || isdigit(myTempTwo) || ispunct(myTempTwo))
						{
							// Retrieving the map element descriptor's name
							mapElemName.push_back(myTempTwo);
						}
					}

					// Adding the map element descriptors to the working directory
					wadSys->addToDirectory(mapElemOffset, mapElemLength, mapElemName, myDir);
				}

				// Setting the parent to be the working directory
				myDir = myDir->getParent();
			}
			// Else just add the file to the working directory
			else
			{
				wadSys->addToDirectory(myOffset, myLength, myName, myDir);
			}
			
		}
			// Closing the input file
			myInputFile.close();
		}

		// Loading the root node's files
		wadSys->loadFiles(wadSys->rootNode);

		// Returning the wad system
		return wadSys;
}

// Defining a function for returning the magic, aka file type, of the WAD data
string Wad::getMagic()
{
	return magic;
}

// Defining a function for checking if a path represents content (data), returning true if yes, and false otherwise
bool Wad::isContent(const string &path)
{
	// Declaring a vector for a path
	vector<string> myPath;

	// Getting rid of the delimiters in the WAD data
	deleteWadDataDelimiters(path, myPath);

	// Setting the name
	string myName = myPath.at(myPath.size() - 1);

	// Looking inside the directory + setting the file
	Node* myFile = lookInsideDirectory(myName, rootNode);

	// If the file isn't null
	if (myFile != nullptr)
	{
		// If the file's length isn't invalid
		if (myFile->nodeLength != 0)
		{
			// Returning true
			return true;
		}
		else
		{
			// Returning false
			return false;
		}
	}
	// Else if it's null
	else
	{
		// Returning false
		return false;
	}
}

// Defining a function for checking if a path represents a directory, returning true if yes, and false otherwise
bool Wad::isDirectory(const string &path)
{
	// Declaring a vector for a path
	vector<string> myPath;

	// Getting rid of the delimiters in the WAD data
	deleteWadDataDelimiters(path, myPath);

	// Setting the name
	string myName = myPath.at(myPath.size() - 1);

	// Looking inside the directory + setting the file
	Node* myFile = lookInsideDirectory(myName, rootNode);

	// If the file isn't null
	if (myFile != nullptr)
	{
		// If the file's length is 0
		if (myFile->nodeLength == 0)
		{
			// Returning true
			return true;
		}
		else
		{
			// Returning false
			return false;
		}
	}
	// Else if it's null
	else
	{
		// Returning false
		return false;
	}
}

// Defining a function for returning the number of bytes in the data, and -1 otherwise
int Wad::getSize(const string &path)
{
	// If the given path is content
	if (isContent(path))
	{
		// Declaring a vector for a path
		vector<string> myPath;

		// Getting rid of the delimiters in the WAD data
		deleteWadDataDelimiters(path, myPath);

		// Setting the name
		string myName = myPath.at(myPath.size() - 1);

		// Looking inside the directory + setting the file
		Node* myFile = lookInsideDirectory(myName, rootNode);

		return myFile->nodeLength;
	}
	// Else if it's not content
	else
	{
		// Returning -1
		return -1;
	}
}

// Defining a function for copying as many bytes as possible, up to the length, of the content's data into the pre-existing buffer + returning the # of bytes copied into buffer, or -1 if the path does not represent content (e.g., if it represents a directory)
int Wad::getContents(const string &path, char *buffer, int length, int offset)
{
	// If the path is content
	if (isContent(path))
	{
		// Declaring a vector for a path
		vector<string> myPath;

		// Getting rid of the delimiters in the WAD data
		deleteWadDataDelimiters(path, myPath);

		// Setting the name
		string myName = myPath.at(myPath.size() - 1);

		// Looking inside the directory + setting the file
		Node* myFile = lookInsideDirectory(myName, rootNode);

		// If the offset is greater than / equal to the file's length
		if (offset >= myFile->nodeLength)
		{
			// Returning -1
			return -1;
		}

		// Copying the file's contents into the buffer
		memcpy(buffer, myFile->nodeContent + offset, length);

		// Returning the bytes copied
		return length;
	}
	// Else if it's not content
	else
	{
		// Returning -1
		return -1;
	}
}

// Defining a function for getting a directory, returning the # of elements in the directory, or -1 if the path does not represent a directory (e.g., if it represents content)
int Wad::getDirectory(const string &path, vector<string> *directory)
{
	// If the path is a directory
	if (isDirectory(path))
	{
		// Declaring a vector for a path
		vector<string> myPath;

		// Getting rid of the delimiters in the WAD data
		deleteWadDataDelimiters(path, myPath);

		// Setting the name
		string myName = myPath.at(myPath.size() - 1);

		// Looking inside the directory + setting the file
		Node* myFile = lookInsideDirectory(myName, rootNode);

		// Iterating through the file's children
		for (int i = 0; i < myFile->nodeChildren.size(); i += 1)
		{
			// Storing the childrens' names
			directory->push_back(myFile->nodeChildren.at(i)->nodeName);
		}

		// Returning the # of elements in the directory
		return myFile->nodeChildren.size();
	}
	// Else if it's not a directory
	else
	{
		// Returning -1
		return -1;
	}
}


// Helper functions that the main library functions will use

// Defining a function for getting the name of a directory in a WAD file
string Wad::getNameOfDirectory(string &_wadFile)
{
	// Declaring the name of the directory
	string myName;

	// Initializing an index for reading the name
	int i = 0;

	// While we're at the WAD file directory's name
	while (_wadFile.at(i) != '_')
	{
		// Storing the name
		myName.push_back(_wadFile.at(i));

		i += 1;
	}
	
	// Returning the directory's name
	return myName;
}

// Defining a function to be used for checking if something is the start of a directory in a WAD file
bool Wad::thisIsTheStartOfADirectory(string &_wadFile)
{
	// Declaring the indicator for the directory's start
	string myIndicator = "_START";

	// Initializing the result we find
	size_t findResult = _wadFile.find(myIndicator);

	// If the result we find is valid
	if (findResult != string::npos && findResult == _wadFile.size() - 6)
	{
		// Returning true
		return true;
	}
	// Else it's not valid
	else
	{
		// Returning false
		return false;
	}
}

// Defining a function to be used for checking if something is the end of a directory in a WAD file
bool Wad::thisIsTheEndOfADirectory(string &_wadFile)
{
	// Declaring the indicator for the directory's end
	string myIndicator = "_END";

	// Initializing the result we find
	size_t findResult = _wadFile.find(myIndicator);

	// If the result we find is valid
	if (findResult != string::npos && findResult == _wadFile.size() - 4)
	{
		// Returning true
		return true;
	}
	// Else it's not valid
	else
	{
		// Returning false
		return false;
	}
}

// Defining a function to be used for checking if something is a map marker in a WAD file
bool Wad::thisIsAMapMarker(string &_wadFile)
{
	// If it's format is valid
	if (_wadFile.at(0) == 'E' && isdigit(_wadFile.at(1)) && _wadFile.at(2) == 'M' && isdigit(_wadFile.at(3)))
	{
		// Returning true
		return true;
	}
	// Else it's not valid
	else
	{
		// Returning false
		return false;
	}
}

// Defining a function for getting the root node
Wad::Node* Wad::getRootNode()
{
	// Returning the root node
	return rootNode;
}

// Defining a function for looking inside a directory
Wad::Node* Wad::lookInsideDirectory(string _fileName, Node* _node)
{
	// If the name matches
	if (_node->nodeName == _fileName)
	{
		// Returning it
		return _node;
	}
	// Else the name isn't the same
	else
	{
		// Declaring a temporary node
		Node* myTemp;

		// Initializing a vector to store the directory's children
		vector<Node*> myChildren = _node->nodeChildren;

		// Iterating through the children
		for (int i = 0; i < myChildren.size(); i += 1)
		{
			// Checking the child's name
			string myFile = myChildren.at(i)->nodeName;

			// If the name matches
			if (myFile == _fileName)
			{
				// Returning the child
				return myChildren.at(i);
			}
			// Else the name isn't a match
			else
			{
				// Looking inside the next directory
				myTemp = lookInsideDirectory(_fileName, myChildren.at(i));

				// If it isn't null
				if (myTemp != nullptr)
				{
					// Returning it
					return myTemp;
				}
			}
		}
	}

	return nullptr;
}

// Defining a function for loading files + their content
void Wad::loadFiles(Node* _node)
{
	// If the node's length is valid
	if (_node->nodeLength != 0)
	{
		// Reading the WAD file
		ifstream inputFile(wadFile, ios::binary);

		// If it's open
		if (inputFile.is_open())
		{
			// Moving to the offset
			inputFile.seekg(_node->nodeOffset);

			// Reading the node's content
			inputFile.read(_node->nodeContent, _node->nodeLength);
		}

		// Closing the file
		inputFile.close();
	}

	// Iterating through the node's children
	for (int i = 0; i < _node->nodeChildren.size(); i += 1)
	{
		// Loading the node's children
		loadFiles(_node->nodeChildren[i]);
	}
}

// Defining a function for removing the delimiters from the WAD data
void Wad::deleteWadDataDelimiters(const string &_path, vector<string> &_wadPath)
{
	// Storing the WAD file's root path
	_wadPath.push_back("/");

	// Initializing the index for iterating through the WAD file's path
	int i = 0;

	// Iterating through the path
	while (i < _path.size())
	{
		// Declaring the temporary string
		string myTemp;

		// While the path is still valid
		while (i < _path.size() && _path.at(i) != '/')
		{
			// Storing the path
			myTemp.push_back(_path.at(i));

			i += 1;
		}
		
		// If the path's size is greater than 0
		if (myTemp.size() > 0)
		{
			// Storing the path
			_wadPath.push_back(myTemp);
		}

		i += 1;
	}
}

// Defining a function for adding a file to the given directory
void Wad::addToDirectory(int _nodeOffset, int _nodeLength, string _nodeName, Node* _directory)
{
	// If the directory isn't null
	if (_directory != nullptr)
	{
		// Set the new file
		Node* myNewFile = new Node(_nodeName, _nodeOffset, _nodeLength, _directory);

		// Storing the new file in the directory
		_directory->nodeChildren.push_back(myNewFile);
	}
}

// Defining a function for displaying a node (file) + its children
void Wad::display(Node* _node)
{
	// If the file isn't null
	if (_node != nullptr)
	{
		// Initializing a vector for the node's children
		vector<Node*> myChildren = _node->nodeChildren;

		// Displaying the node's name
		cout << _node->nodeName << " ";

		// Displaying the node's offset
		cout << _node->nodeOffset << " ";

		// Displaying the node's length
		cout << _node->nodeLength << endl;

		// Iterating through the node's children
		for (int i = 0; i < myChildren.size(); i += 1)
		{
			// Displaying the child
			display(myChildren.at(i));
		}
	}
}

// Defining a function for storing the WAD data's magic + WAD file
void Wad::store(const string &_path, string _magic)
{
	this->wadFile = _path;

	this->magic = _magic;
}

// Defining a function for deleting a node (file) + its children
void Wad::clear(Node* _node)
{
	// If the file isn't null
	if (_node != nullptr)
	{
		// Initializing a vector for the node's children
		vector<Node*> myChildren = _node->nodeChildren;

		// Iterating through the node's children
		for (int i = 0; i < myChildren.size(); i += 1)
		{
			// Deleting the child
			clear(myChildren.at(i));
		}

		// Deleting the node's content
		delete[] _node->nodeContent;

		// Deleting the node itself
		delete _node;
	}
}
