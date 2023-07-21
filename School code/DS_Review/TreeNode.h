#include <vector>
#include <queue>

using std::vector;
using std::queue;

namespace ufl_cap4053
{
	namespace fundamentals
	{
		template <class T>
		class TreeNode
		{
		private:

			// Initializing the data that'll be stored in each node
			T data;

			// Initializing a vector that'll store the children of the node
			vector<TreeNode*> nodesChildren;

			// Helper function for the DF preorder traversal that'll use the given node + dataFunction
			void preOrderHelper(const TreeNode* givenNode, void (*dataFunction)(const T)) const
			{
				// Getting the data of the given node
				dataFunction(givenNode->getData());

				// Iterating through the children of the given node
				for (const TreeNode* givenNodesChild : givenNode->nodesChildren)
				{
					// Recursively calling this function on each child of the given node
					preOrderHelper(givenNodesChild, dataFunction);
				}
			}

			// Helper function for the DF postorder traversal that'll use the given node + dataFunction
			void postOrderHelper(const TreeNode* givenNode, void (*dataFunction)(const T)) const
			{
				// Iterating through the children of the given node
				for (const TreeNode* givenNodesChild : givenNode->nodesChildren)
				{
					// Recursively calling this function on each child of the given node
					postOrderHelper(givenNodesChild, dataFunction);
				}

				// Lastly, getting the data of the given node
				dataFunction(givenNode->getData());
			}

		public:

			// TreeNode's constructor, initializes the data value + starts w/ no children
			TreeNode<T>()
			{
				// Initializing the data value to 0
				this->data = 0;
			}

			// TreeNode's 2nd constructor, stores the given element as the data value + starts w/ no children
			TreeNode<T>(T element)
			{
				this->data = element;
			}

			// Getter for the data that is stored inside the node
			const T& getData() const
			{
				return this->data;
			}

			// Getter for the # of children that this node has
			size_t getChildCount() const
			{
				// Initializing the number of children to 0
				size_t numberOfChildren = 0;

				// Iterating through all of the nodes' children
				for (TreeNode* myNode : this->nodesChildren)
				{
					numberOfChildren++;
				}

				return numberOfChildren;
			}

			// Getter for the child node that uses the given index
			TreeNode<T>* getChild(size_t index)
			{
				// If the child's index isn't null
				if (nodesChildren[index] != nullptr)
				{
					// Just return the child at this index
					return this->nodesChildren[index];
				}
				// Else, we can just return null
				else
				{
					return nullptr;
				}
			}

			// Getter for the child node that uses the given index (const version)
			TreeNode<T>* getChild(size_t index) const
			{
				// If the child's index isn't null
				if (nodesChildren[index] != nullptr)
				{
					// Just return the child at this index
					return this->nodesChildren[index];
				}
				// Else, we can just return null
				else
				{
					return nullptr;
				}
			}

			// Adds the given child to this node's children
			void addChild(TreeNode<T>* child)
			{
				// Adding the given child to the node's vector of children
				this->nodesChildren.push_back(child);
			}

			// Removes the child node at the given index
			void removeChild(size_t index)
			{
				// Initializing the child node that we'll be removing using the given index
				TreeNode* childToBeRemoved = this->nodesChildren[index];

				// Getting the # of children that this child node has
				size_t numberOfChildren = childToBeRemoved->getChildCount();

				// If the child node has children
				if (numberOfChildren != 0)
				{
					// Iterate through all of the child node's children
					for (TreeNode* childOfTheChild : childToBeRemoved->nodesChildren)
					{
						// Deleting the child node's children
						delete childOfTheChild;
					}
				}

				// After the child's children are deleted, we'll finally delete the child node itself
				delete childToBeRemoved;
			}

			// BFS starting at this node, calls dataFunction on the element to process it
			void breadthFirstTraverse(void (*dataFunction)(const T)) const
			{
				// Initializing a queue that'll store the nodes
				queue<const TreeNode*> nodesQueue;

				// Pushing the root node into the queue of nodes
				nodesQueue.push(this);

				// While the queue of nodes isn't empty
				while (!nodesQueue.empty())
				{
					// Initializing the current node to be the node at the front of the queue
					const TreeNode* currentNode = nodesQueue.front();

					// Popping the current node from the nodes queue
					nodesQueue.pop();

					// Getting the data of the current node
					dataFunction(currentNode->getData());

					// Iterating through the current node's children
					for (const TreeNode* currentNodeChild : currentNode->nodesChildren)
					{
						// Pushing the current node's children into the nodes queue
						nodesQueue.push(currentNodeChild);
					}
				}
			}

			// These two functions below call private kickoff functions to perform the traversals with 
			// the recursion suggested in the manual provided to us.

			// DF Preorder traversal starting at this node, calls dataFunction on the element to process it
			void preOrderTraverse(void (*dataFunction)(const T)) const
			{
				// Using the preorder helper function to do the actual traversal
				preOrderHelper(this, dataFunction);
			}

			// DF Postorder traversal starting at this node, calls dataFunction on the element to process it
			void postOrderTraverse(void (*dataFunction)(const T)) const
			{
				// Using the postorder helper function to do the actual traversal
				postOrderHelper(this, dataFunction);
			}
		};
	}
}