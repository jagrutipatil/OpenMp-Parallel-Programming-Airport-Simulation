#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <iostream>
#include <fstream>
#include <sstream>
#include "omp.h"
#include <ctime>
#include <chrono>
#include <thread>
#include <cmath>
#include <unistd.h>

#include <boost/random/mersenne_twister.hpp>
#include <boost/random/uniform_int.hpp>
#include <boost/random/variate_generator.hpp>


#include "../queue/StampingService.h"
string filePath = "";
string outFilePath = "./output.txt";

int count = 10000;
int *arr = new int[count];

Traveler *travelers = new Traveler[count];
string q1ans[]  = {"India", "USA", "China", "Bangladesh"};
string felony[]  = {"yes", "no", "minor", "major"};
const int length = 1000;
char stamp[length];

float *A = new float[36000];
float *B = new float[36000];
float *C = new float[36000];


using namespace stampingService;



extern "C" void stampCalc(const float *A, const float *B, float *C, int numElements)
{
	int i = 0;
	for(i=0;i<numElements;i++)
	{
			C[i]  = A[i] * B[i] * pow(A[i]/20,B[i]/20);
	}
}

namespace dataGenerator {

	string visa[] = { "F1", "B1", "H1B", "F2", "F3", "H2", "L1", "L2" };

	char prefix[][5] = { "ab", "din", "jon", "hpn", "lig", "vpn", "tim" };

	char suffix[][5] = { "bd", "doc", "lab", "tom", "har", "gen", "linr", "vnn",
			"tmq", "hbm", "lin", "hel", "man", "abc", "xyz", "pqr", "mnp", "tdf",
			"hbp" };

	char stem[][10] = { "jagr", "patil", "awai", "preet", "vija", "hell", "tomr",
			"sdf", "ert", "vpnf", "hlmn", "timer", "henr", "zxc", "xyz", "bnm",
			"hjk", "jkl", "dfg" };

	void generateName(char* name) {
		name[0] = 0;
		strcat(name, prefix[(rand() % 7)]);
		strcat(name, stem[(rand() % 20)]);
		strcat(name, suffix[(rand() % 16)]);
		name[0] = toupper(name[0]);
		return;
	}

	void writeUpateToFile(Traveler *ltravelers) {
		ofstream myfile;
		char name[22];
		myfile.open(outFilePath.c_str(), ios::app | ios::out | ios::in);
		myfile << ltravelers->toString();
		myfile.close();
	}

	void writeToFile() {
		Traveler *ltravelers = new Traveler[count];
		ofstream myfile;
		char name[22];
		myfile.open(filePath.c_str(), ios::app | ios::out | ios::in);

		for (int i = 0; i < count; i++) {
			generateName(name);
			ltravelers[i].setFirstName(name);
			generateName(name);
			ltravelers[i].setLastName(name);
			ltravelers[i].setStampingStatus(false);
			ltravelers[i].setVisaType(visa[i % 8]);
			string length = ltravelers[i].toString().length() + "";
			int totalLength = length.length() + 3;
			myfile << totalLength << " | " << ltravelers[i].toString();
		}

		ltravelers = NULL;
		delete ltravelers;
		myfile.close();
	}

	void readFromFile() {
		Traveler *travelers = new Traveler[count];
		int byte;
		string firstname, lastname, visaType, isValidVisa, isStampingDone;
		std::ifstream infile(filePath.c_str());
		std::string line;
		int i = 0;
		while (std::getline(infile, line)) {
			std::istringstream iss(line);

			char * dup = strdup(line.c_str());
			char * token = strtok(dup, " | ");
			byte = atoi(token);
			firstname = strtok(NULL, " | ");
			lastname = strtok(NULL, " | ");
			visaType = strtok(NULL, " | ");
			isValidVisa = strtok(NULL, " | ");
			isStampingDone = strtok(NULL, " | ");
			travelers[i].setFirstName(firstname);
			travelers[i].setLastName(lastname);
			travelers[i].setVisaType(visaType);
			if (isValidVisa.compare("true") == 0) {
				travelers[i].setVisaStatus(true);
			} else {
				travelers[i].setVisaStatus(false);
			}

			if (isStampingDone.compare("true") == 0) {
				travelers[i].setStampingStatus(true);
			} else {
				travelers[i].setStampingStatus(false);
			}

			//MAKE USE of BYTE or CREATE EXTRA FIELD IN TRAVELER CLASS
			cout << travelers[i].toString();
			i++;
		}

		//travelers array will have all data
		infile.close();
	   }
}


namespace Main {
	Traveler *travelers = new Traveler[count];
	TravelQueue queue;
	StampingService stmps;

	void loadTravelers() {
		int byte;
		string firstname, lastname, visaType, isValidVisa, isStampingDone;
		std::ifstream infile(filePath.c_str());
		std::string line;
		int i = 0;

		while (std::getline(infile, line)) {
			std::istringstream iss(line);

			if (i < count) {
				char * dup = strdup(line.c_str());
				char * token = strtok(dup, " | ");
				byte = atoi(token);
				firstname = strtok(NULL, " | ");
				lastname = strtok(NULL, " | ");
				visaType = strtok(NULL, " | ");
				isValidVisa = strtok(NULL, " | ");
				isStampingDone = strtok(NULL, " | ");

				travelers[i].setFirstName(firstname);
				travelers[i].setLastName(lastname);
				travelers[i].setVisaType(visaType);
				if (isValidVisa.compare("true") == 0) {
					travelers[i].setVisaStatus(true);
				} else {
					travelers[i].setVisaStatus(false);
				}

				if (isStampingDone.compare("true") == 0) {
					travelers[i].setStampingStatus(true);
				} else {
					travelers[i].setStampingStatus(false);
				}
				i++;
			}
		}

		//travelers array will have all data
		infile.close();
	}

	void addOfficers() {
		char name[22];
		for (int i = 0; i < count; i++) {
			Officer* offc = new Officer;

			dataGenerator::generateName(name);
			offc->setFirstName(name);
			dataGenerator::generateName(name);
			offc->setLastName(name);
			offc->setProcessingTime(1.0);
			stmps.addOfficer(offc);
		}
	}

	int randomize() {
		boost::mt19937 gen;
	    boost::uniform_int<> dist(1, 5);
	    boost::variate_generator<boost::mt19937&, boost::uniform_int<> > die(gen, dist);
	    return die();
	}

	void genVISAStamp(char *s, const int len) {
	    static const char alphanum[] =
	        "0123456789"
	        "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
	        "abcdefghijklmnopqrstuvwxyz";

	    for (int i = 0; i < len; ++i) {
	        s[i] = alphanum[rand() % (sizeof(alphanum) - 1)];
	    }
	    s[len] = 0;
	}


	bool doWork() {
		//Calculating Stamp number for the traveler
		stampCalc(A,B,C,36000/4);
		return true;
	}

	void progressWorkParallel() {
		cout << "\nStarted Parallel Processing Work"<<endl;

		#pragma omp parallel
		{
			int i = 0;
			std::ifstream infile(filePath.c_str());
			string line;
			int byte;
			string firstname, lastname, visaType, isValidVisa, isStampingDone;

			for(i = omp_get_thread_num(); i < count; i = i + omp_get_num_threads()) {
				infile.clear();
				if(i < count) {
					infile.seekg(arr[i]);
				}
				getline(infile, line);
				std::istringstream iss(line);

				char * dup = strdup(line.c_str());
				char * token = strtok(dup, " | ");
				byte = atoi(token);
				char delimiter[] = " | ";

				firstname = line.substr(0, line.find(delimiter));
				line = line.substr(line.find(delimiter)+1, line.length());
				lastname = line.substr(0, line.find(delimiter));
				line = line.substr(line.find(delimiter)+1, line.length());
				visaType = line.substr(0, line.find(delimiter));
				line = line.substr(line.find(delimiter)+1, line.length());
				isValidVisa = line.substr(0, line.find(delimiter));
				line = line.substr(line.find(delimiter)+1, line.length());
				isStampingDone = line.substr(0, line.find(delimiter));

				travelers[i].setFirstName(firstname);
				travelers[i].setLastName(lastname);
				travelers[i].setVisaType(visaType);

				if (i < count && travelers[i].ifValidVisa()) {
					doWork();
					travelers[i].setStampingStatus(true);
				} else if( i < count) {
					travelers[i].setStampingStatus(false);
				}
			}
		}
	}


	void onBoardFlightA(Traveler t) {
		if (t.ifStampingDone()) {
			doWork();
		}
	}

	void onBoardFlightB(Traveler t) {
		if (t.ifStampingDone()) {
			doWork();
		}
	}

	void onBoardingParallel() {
		cout << "\nStarted Traveler onBoarding in Two Sections";
		int i = 0;
		int totalOnBoardedPeople = 0;
		int groupA = 0, groupB = 0;

		#pragma omp parallel sections
		  {
				#pragma omp section
					{
						for (int i = 0; i < count; i = i + 2) {
							onBoardFlightA(travelers[i]);
							#pragma omp atomic
								groupA++;
						}
					}

				#pragma omp section
					{
						for (int i = 1; i < count; i = i + 2) {
							onBoardFlightB(travelers[i]);
							#pragma omp atomic
								groupB++;
						}
					}
		  }

		  totalOnBoardedPeople = groupA + groupB;
		  cout << "\nTotal Travelers: "<< count << endl << "Total onBoarded Travelers:" << totalOnBoardedPeople;
		  cout << "\n Total onBoarded GroupA Travelers :"<< groupA;
		  cout << "\n Total Travelers went through final security check:"<< groupB << endl;
  }

	void OnBoardingSequential() {
		cout << "\nStarted Traveler onBoarding sequentially";
		int i = 0;
		int totalOnBoardedPeople = 0;
		int groupA = 0, groupB = 0;

		for (int i = 0; i < count; i = i + 2) {
			onBoardFlightA(travelers[i]);
				groupA++;
		}

		for (int i = 1; i < count; i = i + 2) {
			onBoardFlightB(travelers[i]);
				groupB++;
		}

		totalOnBoardedPeople = groupA + groupB;
		cout << "\nTotal Travelers: "<< count << endl << "Total onBoarded Travelers:" << totalOnBoardedPeople;
		cout << "\n Total Travelers Submitted onBoarding pass:"<< groupA;
		cout << "\n Total Travelers went through final security check:"<< groupB << endl;
  }


	void progressWork() {
		cout << "\nStarted Sequential Processing Work"<<endl;
			int i = 0;
			std::ifstream infile(filePath.c_str());
			string line;
			int byte;
			string firstname, lastname, visaType, isValidVisa, isStampingDone;

			for(i = 0; i < count; i = i + 1) {
				infile.clear();
				infile.seekg(arr[i]);

				getline(infile, line);
				std::istringstream iss(line);

				char * dup = strdup(line.c_str());
				char * token = strtok(dup, " | ");
				byte = atoi(token);
				char delimiter[] = " | ";

				firstname = line.substr(0, line.find(delimiter));
				line = line.substr(line.find(delimiter)+1, line.length());
				lastname = line.substr(0, line.find(delimiter));
				line = line.substr(line.find(delimiter)+1, line.length());
				visaType = line.substr(0, line.find(delimiter));
				line = line.substr(line.find(delimiter)+1, line.length());
				isValidVisa = line.substr(0, line.find(delimiter));
				line = line.substr(line.find(delimiter)+1, line.length());
				isStampingDone = line.substr(0, line.find(delimiter));

				travelers[i].setFirstName(firstname);
				travelers[i].setLastName(lastname);
				travelers[i].setVisaType(visaType);

				if (travelers[i].ifValidVisa()) {
					doWork();
					travelers[i].setStampingStatus(true);
				} else if( i < count) {
					travelers[i].setStampingStatus(false);
				}
			}
		}


	void printTravelers() {
		for (int i = 0; i < count; i++) {
			cout << travelers[i].toString();
		}
	  }
}

void readFirstCharacters(){
		std::ifstream is;
		std::ifstream infile(filePath.c_str());
		std::string line;
		int i = 1;
		arr[0] = int(infile.tellg());
		while (std::getline(infile, line)) {
			if (i<count) {
				arr[i] = int(infile.tellg());
			}
			i++;
		}
}

void cleanup() {
  arr = NULL;
  delete arr;

  travelers = NULL;
  delete travelers;
}

void initArray() {
    int j=0;
    float i=0;
   	while (j<36000) {
    	  A[j] = i;
    	  B[j] = i;
    	  i++;
    	  j++;
    }
}


int main(int argc, char* argv[]) {

//	dataGenerator::writeToFile();
	
	if ( argc > 1) {
		filePath = argv[1];
		cout<< "\nReading Data From File: " << filePath;
	    std::chrono::time_point<std::chrono::system_clock> start, end;
	    readFirstCharacters();


		cout << "\n************************Parallel Work Statistics******************************";

		start = std::chrono::system_clock::now();

		cout << endl << "Started VISA Stamping for Travelers"<<endl;
		Main:: progressWorkParallel();
		Main:: onBoardingParallel();
		end = std::chrono::system_clock::now();

		std::chrono::duration<double> elapsed_seconds = end-start;

		std::cout << "Finished computation at " << endl
	              << "Elapsed time: " << elapsed_seconds.count() << "s\n";
		cout << "\n************************Sequential Work Statistics******************************";
		cout << endl << "Started VISA Stamping for Travelers"<<endl;

		start = std::chrono::system_clock::now();
		Main:: progressWork();
		Main:: OnBoardingSequential();
		end = std::chrono::system_clock::now();

		elapsed_seconds = end-start;

		std::cout << "Finished computation at " << endl
	              << "Elapsed time: " << elapsed_seconds.count() << "s\n";

		cout << "\n******************************************************"<< endl;
	} else {
		cout << "\nPlease give Data File path as input\n Usage: <program> <filepath>\n";
	}

//	Main::printTravelers();
	cleanup();
}
