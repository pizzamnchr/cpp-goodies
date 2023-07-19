#include <iostream>
#include <sstream>
#include <string>
#include <iomanip>
#include <unordered_map>
#include <map>
#include <vector>

using namespace std;

class AdjacencyList 
{
private:

	// Using an ordered map to store the fromWebpage as well as a pair containing it's associated rank and websites that it's pointing to inside a vector
	map<string, pair<double, vector<string>>> outgoingWebpages;

	// Using an ordered map to store the toWebpage as well as websites that it was pointed from inside a vector
	map<string, vector<string>> incomingWebpages;

public:

	// Constructor for the adjacency list
	AdjacencyList() 
	{

	}

	void insertWebpage(string fromWebpage, string toWebpage)
	{
		// Pushing the toWebpage into the fromWebpage's vector of web pages that it's pointing to
		outgoingWebpages[fromWebpage].second.push_back(toWebpage);

		if (outgoingWebpages.find(toWebpage) == outgoingWebpages.end())
		{
			outgoingWebpages[toWebpage] = {};
		}

		// Pushing the fromWebpage into the toWebpage's vector of web pages that it was pointed from
		incomingWebpages[toWebpage].push_back(fromWebpage);

		if (incomingWebpages.find(fromWebpage) == incomingWebpages.end())
		{
			incomingWebpages[fromWebpage] = {};
		}
	}

	void printWebpageRanks()
	{
		// Using an iterator to go through the outgoingWebpages map and print all the fromWebpages with their associated ranks
		for (auto mapIterator = outgoingWebpages.begin(); mapIterator != outgoingWebpages.end(); mapIterator++) 
		{
			// Using setprecision so all the fromWebpage ranks are rounded off to only two decimal places
			cout << setprecision(2) << fixed;

			// Printing the fromWebpage and it's associated rank
			cout << mapIterator->first << " " << mapIterator->second.first << endl;
		}
	}

	void powerIterate(int numberOfPowerIterations)
	{
		// Using an iterator to go through the outgoingWebpages map and give all of the web pages an initial ranking
		for (auto mapIterator = outgoingWebpages.begin(); mapIterator != outgoingWebpages.end(); mapIterator++) 
		{
			mapIterator->second.first = 1.00 / outgoingWebpages.size();
		}

		// Power iterating till the user's entered number is reached
		for (int i = 1; i < numberOfPowerIterations; i++) 
		{
			// Using an unordered map to store the fromWebpages and their new ranks
			unordered_map<string, double> fromWebpagesNewRanks;

			for (auto mapIterator = outgoingWebpages.begin(); mapIterator != outgoingWebpages.end(); mapIterator++) 
			{
				// The rank is equal to the sum of incoming webpage ranks
				double sumOfIncomingWebpageRanks = 0;

				string currentFromWebpage = mapIterator->first;

				auto incomingWebpage = incomingWebpages[currentFromWebpage];

				// Calculating the sum of the fromWebpage's incoming webpage ranks
				for (unsigned int j = 0; j < incomingWebpage.size(); j++) 
				{
					sumOfIncomingWebpageRanks += outgoingWebpages[incomingWebpage[j]].first * (1.00 / outgoingWebpages[incomingWebpage[j]].second.size());
				}

				// Setting the sum to be the rank of the currentFromWebpage
				fromWebpagesNewRanks[currentFromWebpage] = sumOfIncomingWebpageRanks;
			}

			// Transferring the fromWebpages and their newly calculated ranks into the outgoingWebpages map
			for (auto mapIterator = fromWebpagesNewRanks.begin(); mapIterator != fromWebpagesNewRanks.end(); mapIterator++) 
			{
				outgoingWebpages[mapIterator->first].first = mapIterator->second;
			}

			// Clearing the unordered map after everything's done
			fromWebpagesNewRanks.clear();
		}
	}
};

int main() 
{
	// Creating an adjacency list
	AdjacencyList webpages;

	string fromWebpage;

	string toWebpage;

	int numberOfLines;

	int numberOfPowerIterations;

	// Taking in the number of lines that the user wants to enter
	cin >> numberOfLines;

	// Taking in the number of power iterations that the user wants
	cin >> numberOfPowerIterations;

	// Inserting all of the user's web pages
	for (int i = 0; i < numberOfLines; i++) 
	{
		cin >> fromWebpage;

		cin >> toWebpage;

		webpages.insertWebpage(fromWebpage, toWebpage);
	}

	webpages.powerIterate(numberOfPowerIterations);
	
	webpages.printWebpageRanks();

	return 0;
}