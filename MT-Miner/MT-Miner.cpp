// MT-Miner.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include <fstream>
#include <map>
#include <vector>
#include <list>
#include <set>

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
std::vector<int> splitPattern(const std::string& s, const std::string& delimiter)
{
	size_t pos = 0;
	std::string token;
	vector<int> v;
	while ((pos = s.find(delimiter)) != std::string::npos) 
	{
		token = s.substr(0, pos);
		v.push_back(atol(token.c_str()));
		s.erase(0, pos + delimiter.length());
	}
	v.push_back(atol(s.c_str()));

	return v;
}

/// <summary>
/// compute disjonctif support from binary representation 
/// use a OR operator for each element of the items
/// </summary>
/// <param name="pattern"></param>
/// <returns></returns>
int computeDisjonctifSupport(const std::string& pattern)
{
	//N_nodes++;
	std::vector<int> itemsOfpattern = splitPattern(pattern, " ");
	std::vector<int> SumOfN_1Items;
	for (int i = 0; i < objectCount; i++)
		SumOfN_1Items.push_back(0);

	for (int i = 0; i < itemsOfpattern.size(); i++)
	{
		for (int j = 0; j < objectCount; j++)
		{
			SumOfN_1Items[j] = SumOfN_1Items[j] | binaryRep[itemsOfpattern[i]][j];
			//std::cout << SumOfN_1Items[j] << " ";
		}
		//std::cout << std::endl;
	}
	int disSupp = 0;
	for (int i = 0; i < num_obj; i++)
	{
		if (SumOfN_1Items[i] == 1)
			disSupp++;
	}
	return disSupp;
}

// --------------------------------------------------------------------------------------------------------------------- //

int main()
{
    std::cout << "Hello World!\n";

	buildBinaryRepresentationFromFile("test.txt", 0);
	std::cout << "itemCount " << itemCount << std::endl;
	std::cout << "objectCount " << objectCount << std::endl;

	getchar();
}

// Run program: Ctrl + F5 or Debug > Start Without Debugging menu
// Debug program: F5 or Debug > Start Debugging menu

// Tips for Getting Started: 
//   1. Use the Solution Explorer window to add/manage files
//   2. Use the Team Explorer window to connect to source control
//   3. Use the Output window to see build output and other messages
//   4. Use the Error List window to view errors
//   5. Go to Project > Add New Item to create new code files, or Project > Add Existing Item to add existing code files to the project
//   6. In the future, to open this project again, go to File > Open > Project and select the .sln file
