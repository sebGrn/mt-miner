// MT-Miner.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include <fstream>
#include <map>
#include <vector>
#include <list>
#include <string>
#include <set>
#include <cassert>
#include <sstream>
#include <algorithm>
#include <iterator>

// --------------------------------------------------------------------------------------------------------------------- //

#define MAX_READ_BUFFER 500
#define MAX_ITEM 10000

// --------------------------------------------------------------------------------------------------------------------- //
// globals

//int numOfFItem;
int minSupport;

struct Item
{
	int index;
	int num;
};

Item* item;
int itemCount = 0;
int objectCount = 0;

// binary representation map of boolean list
// very usefull for disjonctif support computation
std::map<int, bool*> binaryRep;

const std::vector<std::string> originTraverse{ "1", "2", "3", "4", "5", "6", "7", "8" };

// --------------------------------------------------------------------------------------------------------------------- //


/// <summary>
///	load a file containing representation of hypergraph from formal context
///	build a binary representation made of a list of boolean into a map
/// </summary>
/// <param name="filename"></param>
/// <param name="support"></param>
/// <returns></returns>
bool buildBinaryRepresentationFromFile(const char* filename, double support)
{
	FILE* in = NULL;
	errno_t err = fopen_s(&in, filename, "r");
	if(err != 0)
	{
		std::cout << "read wrong!" << std::endl;
		fclose(in);
		return false;
	}

	char str[MAX_READ_BUFFER];
	//char strpre[MAX_READ_BUFFER];
	Item** tempItem = new Item * [10];
	tempItem[0] = new Item[10000];
	
	// initialisation
	for (int i = 0; i < 10000; i++)
	{
		tempItem[0][i].index = i;
		tempItem[0][i].num = 0;
	}

	int size = 1;
	int num = 0;
	int numpre = 0;
	int col = 0;
	int k = 1;
	// 1st pass 
	// - compute number of items
	// - read file with a buffer of MAX_READ_BUFFER characters
	// - read MAX_ITEM items into temporary list, this list will be added into main item list later
	while (fgets(str, MAX_READ_BUFFER, in))
	{
		if (feof(in))
			break;

		objectCount++;
		//cout<<num_obj<<endl;
		num = 0;
		k = 1;
		for (int i = 0; i < MAX_READ_BUFFER && str[i] != '\0'; i++)
		{
			if (str[i] != ' ') 
			{
				num = num * 10 + str[i] - '0';
				//	cout<<"num="<<num<<endl; 
			}
			else
			{
				col = num / MAX_ITEM;
				//cout<<"col="<<col<<endl;
				//cout<<"size="<<size<<endl;
				//si la valeur de l'attribut est un multiple de 100,1000,...
				if (col >= size)
				{
					for (int j = size; j <= col; j++)
					{
						tempItem[j] = new Item[MAX_ITEM];
						for (int p = 0; p < MAX_ITEM; p++)
						{
							tempItem[j][p].index = j * 10000 + p;
							tempItem[j][p].num = 0;
						}
					}
					size = col + 1;
				}
				if (0 == tempItem[col][num % MAX_ITEM].num++)
					itemCount++;
				//cout<<"itemCount="<<numOfItem<<endl;
				num = 0;
			}
		}
	}

	//cout<<"#numOfItem "<<numOfItem<<endl;
	//cout<<"#size "<<size<<endl;

	// add temporary item list into result list
	minSupport = int(support * objectCount);
	item = new Item[itemCount];
	for (int i = 0, p = 0; i < size; i++)
	{
		for (int j = 0; j < MAX_ITEM; j++)
		{
			if (tempItem[i][j].num != 0)
			{
				item[p++] = tempItem[i][j];
			}
		}
	}

	// free memory
	for (int i = 0; i < size; i++)
		delete[] tempItem[i];
	delete[] tempItem;

	//qsort(item, itemCount, sizeof(Item), comp);
	//for(numOfFItem = 0; numOfFItem < itemCount; numOfFItem++)
	//{
	//	cout<<"item[numOfFItem].index="<<item[numOfFItem].index<<endl;
	//	cout<<"item[numOfFItem].num="<<item[numOfFItem].num<<endl;
	//	cout<<"***"<<endl;
	//	if(item[numOfFItem].num < Min_Support)
	//	break;
	//}

	// allocate binary representation 
	std::map<int, bool*>::iterator iter;
	for (int i = 0; i < itemCount; i++)
	{
		binaryRep[item[i].index] = new bool[objectCount];
		// cout<<"***"<<item[i].index<<"***"<<endl;
	}

	// init binary representation boolean lists to false
	for (iter = binaryRep.begin(); iter != binaryRep.end(); ++iter)
	{
		//iter->second;
		// cout<<(*iter).first<<endl; 
		for (int i = 0; i < objectCount; i++)
		{
			iter->second[i] = false;
			//cout<<iter->second[i]<<" ";
		}
		//cout<<endl;
	}

	// 2nd pass
	// - read file to update binary representation boolean value to true if a value exists
	err = fopen_s(&in, filename, "r");
	int tr = -1;
	while (fgets(str, MAX_READ_BUFFER, in))
	{
		if (feof(in)) 
			break;
		num = 0;
		tr++;
		for (int i = 0; i < MAX_READ_BUFFER && str[i] != '\0'; i++)
		{
			if (str[i] != ' ')
			{
				num = num * 10 + str[i] - '0';
				continue;
			}
			else
			{
				iter = binaryRep.find(num);
				if (iter != binaryRep.end())
				{
					iter->second[tr] = true;
				}
				num = 0;
			}
		}
	}

	//cout<<(*iter).first<<endl;
	//ietr
	//iter->second[tr]

	//-----Affichage du binary rep--------//
/*	for(iter = binaryRep.begin(); iter != binaryRep.end(); ++iter)
	{
		cout << "Key: " << (*iter).first <<endl;
		for(int i=0; i<num_obj;i++)
		{
			//iter->second[i]=0;
			cout<<iter->second[i]<<" ";
		}
		cout<<endl;
		cout<<"$$$$$$"<<endl;
	}
*/

	fclose(in);
	return true;
}

// --------------------------------------------------------------------------------------------------------------------- //

/// <summary>
/// extract a list of int from a string
/// </summary>
/// <param name="s"></param>
/// <param name="delimiter"></param>
/// <returns></returns>
template <typename Out>
void split(const std::string& s, char delim, Out result)
{
	std::istringstream iss(s);
	std::string item;
	while (std::getline(iss, item, delim)) {
		*result++ = std::stoi(item);
	}
}

/// <summary>
/// extract a list of int from a string
/// </summary>
/// <param name="s"></param>
/// <param name="delimiter"></param>
/// <returns></returns>
std::vector<unsigned int> splitToVectorOfInt(const std::string& s, char delim)
{
	std::vector<unsigned int> elems;
	split(s, delim, std::back_inserter(elems));
	return elems;
}

/// <summary>
/// extract a list of int from a string
/// </summary>
/// <param name="s"></param>
/// <param name="delimiter"></param>
/// <returns></returns>
std::vector<int> splitPattern(const std::string& s, const std::string& delimiter)
{
	std::string tmpstr = s;
	size_t pos = 0;
	std::string token;
	std::vector<int> v;
	while ((pos = tmpstr.find(delimiter)) != std::string::npos)
	{
		token = tmpstr.substr(0, pos);
		v.push_back(atol(token.c_str()));
		tmpstr.erase(0, pos + delimiter.length());
	}
	v.push_back(atol(tmpstr.c_str()));
	return v;
}

/// <summary>
/// compute disjonctif support from binary representation 
/// use a OR operator for each element of the items
/// </summary>
/// <param name="pattern"></param>
/// <returns></returns>
unsigned int computeDisjonctifSupport(const std::string& pattern)
{
	//N_nodes++;
	//std::vector<int> itemsOfpattern = splitPattern(pattern, " ");
	std::vector<unsigned int> itemsOfpattern = splitToVectorOfInt(pattern, ' ');
	std::vector<unsigned int> SumOfN_1Items;
	for (int i = 0; i < objectCount; i++)
		SumOfN_1Items.push_back(0);

	for (int i = 0; i < itemsOfpattern.size(); i++)
	{
		for (int j = 0; j < objectCount; j++)
		{
			//SumOfN_1Items[j] = SumOfN_1Items[j] | binaryRep[itemsOfpattern[i]][j];
			SumOfN_1Items[j] = (SumOfN_1Items[j] != 0) | binaryRep[itemsOfpattern[i]][j];
			//std::cout << SumOfN_1Items[j] << " ";
		}
		//std::cout << std::endl;
	}
	unsigned int disSupp = 0;
	for (int i = 0; i < objectCount; i++)
	{
		if (SumOfN_1Items[i] == 1)
			disSupp++;
	}
	return disSupp;
}

// --------------------------------------------------------------------------------------------------------------------- //
// --------------------------------------------------------------------------------------------------------------------- //
// --------------------------------------------------------------------------------------------------------------------- //

struct compare
{
	int key;
	compare(int const& i) : key(i) { }

	bool operator()(int const& i)
	{
		return (i == key);
	}
};


std::vector<std::string> combineInVector(const std::string& eltToCombine, const std::vector<std::string>& v)
{
	std::vector<std::string> combinedListElt;
	for_each(v.begin(), v.end(), [&](const std::string& s) {
		if (eltToCombine != s)
		{
			std::string combinedElt = eltToCombine + ' ' + s;
			combinedListElt.push_back(combinedElt);			
		}
	});

			
			/*
	std::vector<unsigned int> intList1 = splitToVectorOfInt(str1, ' ');
	std::vector<unsigned int> intList2 = splitToVectorOfInt(str2, ' ');
	// "1" + "2" => "12"
	// "71" + "72" => "712"
	std::vector<std::string> combinedListElt;
	for_each(intList1.begin(), intList1.end(), [&](unsigned int i) {
		for_each(intList2.begin(), intList2.end(), [&](unsigned int j) {
			if (i != j)
			{
				std::string combinedElt = std::to_string(i) + ' ' + std::to_string(j);
				combinedListElt.push_back(combinedElt);
			}

		//auto it = std::find_if(intList2.begin(), intList2.end(), compare(i));
		//if (it != intList2.end())
		//{
		//	// remove elt
		//	intList2.erase(it);
		//}
		});
	});
	*/
	/*
	// merge 2 lists into intList1
	intList1.insert(intList1.end(), intList2.begin(), intList2.end());
	// transform int list into string list seperated by ' '
	std::string combinaisonElt;
	for_each(intList1.begin(), intList1.end(), [&](unsigned int i) {
		combinaisonElt += std::to_string(i) + ' ';
	});
	// remove last character
	combinaisonElt.pop_back();
	//std::string combinaisonElt = str1 + " " + str2;
	*/
	return combinedListElt;
}

std::string combineIntoString(const std::string& str1, const std::string& str2)
{
	std::vector<unsigned int> intList1 = splitToVectorOfInt(str1, ' ');
	std::vector<unsigned int> intList2 = splitToVectorOfInt(str2, ' ');
	// "1" + "2" => "12"
	// "71" + "72" => "712"
	std::vector<std::string> combinedListElt;
	for_each(intList1.begin(), intList1.end(), [&](unsigned int i) {
		auto it = std::find_if(intList2.begin(), intList2.end(), compare(i));
		if (it != intList2.end())
		{
			// remove elt
			intList2.erase(it);
		}
	});
	// merge 2 lists into intList1
	intList1.insert(intList1.end(), intList2.begin(), intList2.end());
	// transform int list into string list seperated by ' '
	std::string combinedElt;
	for_each(intList1.begin(), intList1.end(), [&](unsigned int i) {
		combinedElt += std::to_string(i) + ' ';
	});
	// remove last character
	combinedElt.pop_back();
	//std::string combinedElt = str1 + " " + str2;
	return combinedElt;
}

std::string getStringFromStringVector(const std::vector<std::string> v, char delim)
{
	std::string res;
	for_each(v.begin(), v.end(), [&](const std::string& str) {
		res += str + delim;
	});
	// remove last character (delimiter)
	res.pop_back();
	return res;
}

std::vector<std::string> getStringVectorFromString(const std::string& s)
{
	std::stringstream ss(s);
	std::istream_iterator<std::string> begin(ss);
	std::istream_iterator<std::string> end;
	std::vector<std::string> vstrings(begin, end);
	std::copy(vstrings.begin(), vstrings.end(), std::ostream_iterator<std::string>(std::cout, "\n"));
	return vstrings;
}

void computeMinimalTransversals(unsigned int currentIndex, std::string& previousElt, std::vector<std::string>& toTraverse, std::vector<std::string>& maxClique, std::vector<std::string>& toExplore, std::vector<std::string>& mt)
{
	if (currentIndex >= toTraverse.size())
	{
		if (toExplore.empty())
			return;
		else
		{
			// combine toExplore results with toTraverse
			//std::string toTraverseStr = getStringFromStringVector(toTraverse, ' ');
			//std::string toExploreStr = getStringFromStringVector(toExplore, ' ');

			// recurse for each element of toExplore list
			for_each(toExplore.begin(), toExplore.end(), [&](const std::string& toExploreElt) {

				std::vector<std::string> combinedList = combineInVector(toExploreElt, originTraverse);
				toTraverse = combinedList;
			
				// clear lists and continue...
				toExplore.clear();
				maxClique.clear();
				previousElt = "";
				computeMinimalTransversals(0, previousElt, toTraverse, maxClique, toExplore, mt);
			});
		}
	}
	else if (currentIndex == 0 && toTraverse[currentIndex].size() == 1)
	{
		// add solo element
		std::string currentElt = toTraverse[currentIndex];
		unsigned int disjSup = computeDisjonctifSupport(currentElt);
		if (disjSup != objectCount)
		{
			previousElt = currentElt;
			maxClique.push_back(currentElt);
		}
		else
		{
			// here we add element into toExplore list or into minimumTrasversals list ?
			toExplore.push_back(currentElt);
		}

		currentIndex++;
		computeMinimalTransversals(currentIndex, previousElt, toTraverse, maxClique, toExplore, mt);
	}
	else
	{
		// add combinaison of previous + current
		std::string lastElt = previousElt;
		std::string currentElt = toTraverse[currentIndex];
		// make a union on 2 elements
		std::string combinaisonStr = combineIntoString(lastElt, currentElt);
		unsigned int disjSup = computeDisjonctifSupport(combinaisonStr);
		if (disjSup != objectCount)
		{
			previousElt = combinaisonStr;
			maxClique.push_back(currentElt);
		}
		else
		{
			// here we add element into toExplore list or into minimumTrasversals list ?
			// do not recurse if we add element into minimumTrasversals list !!!
			toExplore.push_back(currentElt);
		}
		currentIndex++;
		computeMinimalTransversals(currentIndex, previousElt, toTraverse, maxClique, toExplore, mt);
	}
}

// --------------------------------------------------------------------------------------------------------------------- //

int main()
{
	// performing some unitary tests
	std::cout << "performing tests..." << std::endl;

	buildBinaryRepresentationFromFile("test.txt", 0);
	std::cout << "itemCount " << itemCount << std::endl;
	std::cout << "objectCount " << objectCount << std::endl;
	assert(itemCount == 8);
	assert(objectCount == 6);

	unsigned int disjonctifSupport = computeDisjonctifSupport("1");
	std::cout << "disjonctifSupport(V1) " << disjonctifSupport << std::endl;
	assert(disjonctifSupport == 1);

	disjonctifSupport = computeDisjonctifSupport("1 2");
	std::cout << "disjonctifSupport(V12) " << disjonctifSupport << std::endl;
	assert(disjonctifSupport == 2);

	disjonctifSupport = computeDisjonctifSupport("1 2 3");
	std::cout << "disjonctifSupport(V123) " << disjonctifSupport << std::endl;
	assert(disjonctifSupport == 3);

	disjonctifSupport = computeDisjonctifSupport("1 2 3 4");
	std::cout << "disjonctifSupport(V1234) " << disjonctifSupport << std::endl;
	assert(disjonctifSupport == 4);

	std::cout << "unitary tests are OK!\n";

	std::vector<std::string> maxClique;
	std::vector<std::string> toExplore;
	std::vector<std::string> minimalTransversals;
	std::vector<std::string> toTraverse = originTraverse;
	std::string previousElt = "";	
	computeMinimalTransversals(0, previousElt, toTraverse, maxClique, toExplore, minimalTransversals);
}
