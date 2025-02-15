// Main.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"


using namespace std;

//definition 
template<typename T>
class KMEANS
{
private:
	vector< vector<T> > dataSet;//the data set  
	vector< T > mmin, mmax;
	int colLen, rowLen;//colLen:the dimension of vector;rowLen:the number of vectors  
	int k;	//the clusters' number
	vector< vector<T> > centroids; //the central point of K clusters(same dimension with samples）
	vector< vector< vector<T>>> totalCentroids;//Record the center of all iteration clusters
	typedef struct MinMax
	{
		T Min;
		T Max;
		MinMax(T min, T max) :Min(min), Max(max) {}
	}tMinMax;	 // The cluster should be in [min, max]
	typedef struct Node
	{
		int minIndex;  //index value of the cluster
		double minDist;//Euclidean distance from the center of a cluster
		Node(int idx, double dist) :minIndex(idx), minDist(dist) {}
	}tNode; //the cluster category and distance value belonging to each points
	vector<tNode>  clusterAssment;  //the category of clusters that each vectors belongs to

	/* Split line into numbers
	 * divide a line into several values and put it into a container
	*/
	void split(char *buffer, vector<T> &vec);	

	tMinMax getMinMax(int idx);
	void setCentroids(tMinMax &tminmax, int idx);
	void initClusterAssment();
	double distEclud(vector<T> &v1, vector<T> &v2);  //calculate Euclidean distance

public:
	KMEANS(int k);
	void loadDataSet(char *filename);	//Read the file loading data and write it into dataSet, 
										//initialize the number of samples (rowLen) and sample dimension (colLen)

	void randCent();//Randomly generating mean vector
	void print();	//Write the result into the res.txt file,each line is: the sample belongs to the category
	void printCentroids();
	void kmeans();	//K means clustering
};

//realization
template<typename T>
void KMEANS<T>::initClusterAssment()
{
	tNode node(-1, -1);	//Represent index values and storage errors (SSE) of the clusters respectively
	for (int i = 0; i<rowLen; i++)
	{
		clusterAssment.push_back(node);
	}
}

template<typename T>
void KMEANS<T>::kmeans()
{
	int count = 0;
	initClusterAssment();  //initialize ClusterAssement，each element is a structure，
	bool clusterChanged = true;
	//the termination condition can also be the loops less than some number such as 1000  
	while (clusterChanged)
	{
		count++;
		clusterChanged = false;
		//step one : find the nearest centroid of each point  
		cout << "\nthe " << count << " iterator:\n" << "\tfind the nearest centroid of each point :" << endl;
		for (int i = 0; i < rowLen; i++)
		{
			int minIndex = -1;			// Initial
			double minDist = INT_MAX;	// Initial
			for (int j = 0; j < k; j++)
			{
				double distJI = distEclud(centroids[j], dataSet[i]);	// Calculate Euler's distance
				if (distJI < minDist)
				{	// Updating cluster's index (j) which is min distance to element (i)
					minDist = distJI;	// And also the min distance updated
					minIndex = j;	//clusters' class identifier
				}
			}
			if (clusterAssment[i].minIndex != minIndex)
			{	// When the cluster's centers had been changed, Updatding it
				clusterChanged = true;
				clusterAssment[i].minIndex = minIndex;
				clusterAssment[i].minDist = minDist;
			}
		}

		//step two : update the centroids  
		cout << "\tupdate the centroids:" << endl;
		for (int cent = 0; cent < k; cent++)
		{
			vector<T> vec(colLen, 0);
			int cnt = 0;
			for (int i = 0; i < rowLen; i++)
			{
				if (clusterAssment[i].minIndex == cent)
				{
					++cnt;
					//sum of two vectors  
					for (int j = 0; j < colLen; j++)
					{
						vec[j] += dataSet[i].at(j);
					}
				}
			}
			cout << "(";
			//mean of the vector and update the centroids[cent] ,simultaneously ouput
			for (int i = 0; i < colLen; i++)
			{
				if (cnt != 0)    vec[i] /= cnt;
				centroids[cent].at(i) = vec[i];
				//
				cout << vec[i];
				if (i != colLen - 1)
					cout << ",";
			}
			cout << ")\t";
		}//for  
		cout << endl;

		totalCentroids.push_back(centroids);//accumulated centroids

		print();
		printCentroids();

	}//while  

#if 0  
	typename vector<tNode> ::iterator it = clusterAssment.begin();
	while (it != clusterAssment.end())
	{
		cout << (*it).minIndex << "\t" << (*it).minDist << endl;
		it++;
	}
#endif  
}

template<typename T>
KMEANS<T>::KMEANS(int k)
{
	this->k = k;
}

template<typename T>
void KMEANS<T>::setCentroids(tMinMax &tminmax, int idx)
{
	T rangeIdx = tminmax.Max - tminmax.Min;
	for (int i = 0; i < k; i++)
	{
		/* generate float data between 0 and 1 */
		centroids[i].at(idx) = tminmax.Min + rangeIdx *  (rand() / (double)RAND_MAX);
	}
}

//get the min and max value of the idx column  
template<typename T>
typename KMEANS<T>::tMinMax KMEANS<T>::getMinMax(int idx)
{
	T min, max;
	// To get the max and min values
	dataSet[0].at(idx) > dataSet[1].at(idx) ? (max = dataSet[0].at(idx), min = dataSet[1].at(idx)) : (max = dataSet[1].at(idx), min = dataSet[0].at(idx));
	for (int i = 2; i<rowLen; i++)
	{
		if (dataSet[i].at(idx) < min)   min = dataSet[i].at(idx);
		else if (dataSet[i].at(idx) > max) max = dataSet[i].at(idx);
		else continue;
	}

	tMinMax tminmax(min, max);
	return tminmax;
}

template<typename T>
void KMEANS<T>::randCent()
{
	//init centroids  
	vector<T> vec(colLen, 0);
	for (int i = 0; i<k; i++)
	{
		centroids.push_back(vec);
	}

	//set values by column  
	srand(time(NULL));
	for (int j = 0; j<colLen; j++)
	{
		tMinMax tminmax = getMinMax(j);	// The clusters should be in [min, max]
		setCentroids(tminmax, j);
	}
}

template<typename T>
double KMEANS<T>::distEclud(vector<T> &v1, vector<T> &v2)
{
	T sum = 0;
	int size = v1.size();
	for (int i = 0; i<size; i++)
	{
		sum += (v1[i] - v2[i])*(v1[i] - v2[i]);
	}
	return sum;
}

template<typename T>
void KMEANS<T>::split(char *buffer, vector<T> &vec)
{
	char *p = strtok(buffer, ",");   //,
	int c = 1;
	while (p != NULL)
	{	//only read the first two columns of data
		vec.push_back(atof(p));
		if (c == 2)
		{
			break;
		}
		p = strtok(NULL, " \n");
		c++;
	}
	//Data format processing details
	//vec.pop_back(); //with tag after data item
}

template<typename T>
void KMEANS<T>::print()
{
	ofstream fout;
	fout.open("res.txt");
	if (!fout)
	{
		cout << "file res.txt open failed" << endl;
		exit(0);
	}

#if 0  
	typename vector< vector<T> > ::iterator it = centroids.begin();
	while (it != centroids.end())
	{
		typename vector<T> ::iterator it2 = (*it).begin();
		while (it2 != (*it).end())
		{
			//fout<<*it2<<"\t";  
			cout << *it2 << "\t";
			it2++;
		}
		//fout<<endl;  
		cout << endl;
		it++;
	}
#endif  

	typename vector< vector<T> > ::iterator it = dataSet.begin();
	typename vector< tNode > ::iterator itt = clusterAssment.begin();
	for (int i = 0; i<rowLen; i++)
	{
		typename vector<T> ::iterator it2 = (*it).begin();
		while (it2 != (*it).end())
		{
			fout << *it2 << ",";
			it2++;
		}
		fout << (*itt).minIndex << endl;
		itt++;
		it++;
	}

}

template <typename T>
void KMEANS<T>::printCentroids()
{
	ofstream fout;
	fout.open("resCentroids.txt");
	if (!fout)
	{
		cout << "file resCentroids.txt open failed" << endl;
		exit(0);
	}
	typename vector< vector< vector<T> > > ::iterator it = totalCentroids.begin();
	while (it != totalCentroids.end())
	{
		typename vector< vector<T>> ::iterator itt = (*it).begin();
		while (itt != (*it).end())
		{
			typename vector<T> ::iterator ittt = (*itt).begin();
			while (ittt != (*itt).end())
			{
				fout << (*ittt) << ",";
				ittt++;
			}
			itt++;
		}
		it++;
		fout << "\n";
	}
}


template<typename T>
void KMEANS<T>::loadDataSet(char *filename)
{
	FILE *pFile;
	pFile = fopen(filename, "r");
	if (!pFile)	
	{	// When the *pFile point to null
		printf("open file %s failed...\n", filename);
		exit(0);
	}

	//init dataSet  
	char *buffer = new char[300];   //The heap space must be large enough to hold one line of content, 
									//and each row of difficult-doughnut.txt takes up about 300 characters

	vector<T> temp;
	while (fgets(buffer, 300, pFile))
	{	// Read a line of pFile in every loop
		temp.clear();				// Temp intialization
		split(buffer, temp);
		dataSet.push_back(temp);	// Element insert into dataSet
	}

	// Update colLen, rowLen   
	colLen = dataSet[0].size();
	rowLen = dataSet.size();
}



/*	Note:
 *	In my files structure, and in this director:
 *  D:\work\VSWorkspace\MachineLearning\MachineLearning
 *  you could run like this:
 *	..\Debug\Kmeans testSet.txt 4
*/
int main(int argc, char *argv[])
{
	/*
	Run the program with command line parameters:
	Project--Attribute--Configuration property--Debug--Command line parameter

	A total of three parameters, namely:
	Project name (default), Test data file name, K value

	Note: all files are placed in the default directory of the project. If you want to place it somewhere else,
	you need to specify the path of the file, otherwise there will be "unreadable memory" errors
	*/
	//FILE * p;
	//p = fopen("testSet.txt", "r");	// set the file on the correct location
	//if (argc != 3)	// Input error occur, exit
	//{
	//	cout << "Usage : ./a.out filename k" << endl;
	//	exit(0);
	//}

	//char *filename = argv[1];	// Test data file name
	//int k = atoi(argv[2]);		// The value of K

	/* More simple to debug:
	 * Note:
	 * When in this code condition, the testSet.txt, res.txt, resCentroid.txt,
	 * should be in the same directory with KMeans.exe 
	*/
	char *filename = "testSet.txt";	// Test data file name
	int k = 1;		// The value of K
	cout << "K-Means: " << endl;
	cout << "Enter k: ";
	cin >> k;

	KMEANS<double> kms(k);
	kms.loadDataSet(filename);
	cout << "the dataSet would be devided into " << k << " clusters";
	kms.randCent();	// Randomly select init .. vector
	kms.kmeans();	// K-Means
	system("pause");
	return 0;
}

