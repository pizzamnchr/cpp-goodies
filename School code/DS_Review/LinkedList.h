// LinkedList class should go in the "ufl_cap4053::fundamentals" namespace!
namespace ufl_cap4053
{
	namespace fundamentals
	{
		template <class T>
		class LinkedList
		{
			class Node
			{
			public:
				// The pointer for the previous node
				Node* previousNode;

				// The pointer for the next node
				Node* nextNode;

				// The goodies in each node (in this case, coordinates for a location)
				T goodies;
			};

			class Iterator
			{
			private:
				// The current node that the iterator is on
				Node* currentNode;

			public:
				// Constructing the iterator
				Iterator(Node* beginning)
				{
					// Initializing the beginning of the LL
					currentNode = beginning;
				}

				// Retrieving the iterator's current node
				Node* getCurrentNode()
				{
					return currentNode;
				}

				// Returning the element at the iterator's current position in the queue
				T operator*() const
				{
					return currentNode->goodies;
				};

				// Pre-increment overload; Advancing the operator to the next position in the LL + returns this iterator
				Iterator& operator++()
				{
					// If the node after the current node isn't null, move the current node up to its position
					if (currentNode->nextNode != nullptr)
					{
						currentNode = currentNode->nextNode;
					}
					// Else, set the current node to null
					else
						currentNode = nullptr;

					// Returning this iterator
					return *this;
				};

				// Returning true if both iterators point to the same node in the list + false otherwise
				bool operator==(Iterator const& rhs)
				{
					return (currentNode == rhs.currentNode);
				};

				// Returning false if both iterators point to the same node in the list + true otherwise
				bool operator!=(Iterator const& rhs)
				{
					return (currentNode != rhs.currentNode);
				};
			};

		private:
			// A sentinel front and back node, as well as count of elements in the list
			int count = 0;

			// The sentinel node (doesn't hold/refer to any data) at the front of the list
			Node* front;

			// The sentinel node at the back of the list
			Node* back;

		public:
			// Constructing the LL
			LinkedList<T>()
			{
				// Intitializing the front + back sentinel nodes to null
				front = nullptr;

				back = nullptr;
			};

			// Getter for the iterator right before the front of the LL
			Iterator begin() const
			{
				// Returning the iterator
				return Iterator(front);
			};

			// Getter for the iterator right after the back of the LL
			Iterator end() const
			{
				// If we're already after the back of the LL (which we want)
				if (back == nullptr)
				{
					// Returning the iterator
					return Iterator(back);
				}

				// Else, we're at the last node of the LL
				// Returning the iterator
				return Iterator(back->nextNode);
			};

			// Returning true if the LL is empty + false otherwise
			bool isEmpty() const
			{
				// True if count is 0 + false otherwise
				return (count == 0);
			};

			// Returning the node at the front of the LL
			T getFront() const
			{
				// Returning its contents
				return front->goodies;
			};

			// Returning the node at the back of the LL
			T getBack() const
			{
				// Returning its contents
				return back->goodies;
			};

			// Inserting the given element into the LL
			void enqueue(T element)
			{
				// Creating a placeholder node
				Node* placeholderNode = new Node;

				// Storing the given element in the placeholder node
				placeholderNode->goodies = element;

				// Setting the node after the placeholder node to null
				placeholderNode->nextNode = nullptr;

				// Incrementing the LL's count
				count++;

				// If the back of the LL is null
				if (back == nullptr)
				{
					// Setting the front + back nodes to the placeholder node
					front = back = placeholderNode;

					// Setting the node before the front node to null
					front->previousNode = nullptr;

					return;
				}

				// Setting the node before the placeholder node to be the back node
				placeholderNode->previousNode = back;

				// Setting the node after the back node to be the placeholder node
				back->nextNode = placeholderNode;

				// Setting the back node to be the placeholder node
				// The given element is now added to the LL
				back = placeholderNode;
			};

			// Removing the first element of the LL
			void dequeue()
			{
				// If the front of the LL is already null, we're good
				if (front == nullptr)
				{
					return;
				}

				// Creating a placeholder node + pointing it at the front node
				Node* placeholderNode = front;

				// Setting the front node to the node after it
				front = front->nextNode;

				// If the front node is null
				if (front == nullptr)
				{
					// Setting the back node to null
					back = nullptr;
				}
				// Else, the front node isn't null
				else
				{
					// Setting the node before the front node to null
					front->previousNode = nullptr;
				}

				// Deleting the placeholder node as we're done with it
				delete placeholderNode;

				// Decrementing the LL's count
				count--;
			};

			// Removing the last element of the LL
			void pop()
			{
				// If the back of the LL is already null, we're good
				if (back == nullptr)
				{
					return;
				}

				// Creating a placeholder node + pointing it at the back node
				Node* placeholderNode = back;

				// Setting the back node to be the node before it
				back = back->previousNode;

				// Deleting the placeholder node
				delete placeholderNode;

				// Decrementing the LL's count
				count--;

				// If the back node is null
				if (back == nullptr)
				{
					// Setting the front node to null
					front = nullptr;

					return;
				}

				// Setting the node after the back node to null
				back->nextNode = nullptr;
			};

			// Getting rid of all the LL's elements
			void clear()
			{
				// Initializing the beginning iterator for the LL
				Iterator beginningIterator = begin();

				// Initializing the ending iterator for the LL
				Iterator endingIterator = end();

				// While the iterators don't meet each other
				while (beginningIterator != endingIterator)
				{
					// Bump up the beginning iterator
					++beginningIterator;

					// Removing the last element of the LL
					dequeue();
				}
			};

			// Returning true if you find a match + false otherwise
			bool contains(T element) const
			{
				// Initializing the beginning iterator for the LL
				Iterator beginningIterator = begin();

				// While the beginning iterator doesn't meet the end of the LL
				while (beginningIterator != end())
				{
					// If the beginning iterator's element matches the given element
					if (*beginningIterator == element)
					{
						// Returning true if we have a match
						return true;
					}

					// Else, we bump up the beginning iterator to the next element
					++beginningIterator;
				}

				// Returning false if we reach the end of the LL + don't find a match
				return false;
			};

			// Removes the first node you find whose data equals the specified element.
			void remove(T element)
			{
				// Initializing the beginning iterator for the LL
				Iterator beginningIterator = begin();

				// While the beginning iterator doesn't meet the end of the LL
				while (beginningIterator != end())
				{
					// If the beginning iterator's element matches the given element
					if (*beginningIterator == element)
					{
						// If the front node is null, we're done
						if (front == nullptr)
						{
							return;
						}

						// Initializing the placeholder node + pointing it at the beginning iterator's node
						Node* placeholderNode = beginningIterator.getCurrentNode();

						// Decrementing the LL's count
						count--;

						// If the front node is equal to the placeholder node
						if (front == placeholderNode)
						{
							// Setting the front node to be the node after the placeholder node
							front = placeholderNode->nextNode;
						}

						// If the back node is equal to the placeholder node
						if (back == placeholderNode)
						{
							// Setting the back node to be the node before the placeholder node
							back = placeholderNode->previousNode;
						}

						// If the node after the placeholder node isn't null
						if (placeholderNode->nextNode != nullptr)
						{
							// Setting the node before the node after the placeholder node to be the node before the placeholder node
							placeholderNode->nextNode->previousNode = placeholderNode->previousNode;
						}

						// If the node before the placeholder node isn't null
						if (placeholderNode->previousNode != nullptr)
						{
							// Setting the node after the node before the placeholder node to be the node after the placeholder node
							placeholderNode->previousNode->nextNode = placeholderNode->nextNode;
						}

						// Deleting the placeholder node as we're done with it
						delete placeholderNode;

						// We're done
						return;
					}

					// Bumping up the beginning iterator if we're still going
					++beginningIterator;
				}
			};

			// Destroying the LL
			~LinkedList<T>()
			{
				// Getting rid of all the LL's elements
				clear();
			};
		};
	}  // namespace ufl_cap4053::fundamentals
}