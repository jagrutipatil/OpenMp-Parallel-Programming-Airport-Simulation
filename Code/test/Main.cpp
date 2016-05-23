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
#define NUM_THREADS 8
#include "../queue/StampingService.h"
string filePath = "/home/jagruti/workspace/CMPE-275-Project-2-Lightening-Talk/Code/data.txt";
int count = 200;
int *arr = new int[count];
Traveler *travelers = new Traveler[count];

using namespace stampingService;

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

	void batchTravelers() {
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
	
	void progressWork() {
		cout << "\nStarted Processing Work"<<endl;

		#pragma omp parallel
		{
			int i = 0;
			cout << endl << "Total Threads: "<< omp_get_num_threads();
			std::ifstream infile(filePath.c_str());
			string line;
			int byte;
			string firstname, lastname, visaType, isValidVisa, isStampingDone;

			for(i=omp_get_thread_num(); i < count; i=i + omp_get_num_threads()) {

				cout<< endl <<"Thread: " << omp_get_thread_num();
				infile.clear();
				if(i < count) {
					infile.seekg(arr[i]);
				}
				getline(infile, line);
				std::istringstream iss(line);

				char * dup = strdup(line.c_str());

				#pragma omp critical
				{
					char * token = strtok(dup, " | ");
					byte = atoi(token);
					firstname = strtok(NULL, " | ");
					lastname = strtok(NULL, " | ");
					visaType = strtok(NULL, " | ");
					isValidVisa = strtok(NULL, " | ");
					isStampingDone = strtok(NULL, " | ");
				}

				if (i < count) {
					travelers[i].setFirstName(firstname);
				}

				if (i < count) {
					travelers[i].setLastName(lastname);
				}

				if (i < count) {
					travelers[i].setVisaType(visaType);
				}


				if(i < count && travelers[i].ifValidVisa()) {
					if (allQuestionsAnswered(travelers[i])) {
						travelers[i].setStampingStatus(true);
					}


				} else if( i < count) {
					travelers[i].setStampingStatus(false);
				}
			}
		}
	}

	bool allQuestionsAnswered(Traveler travelers) {

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


int main(int argc, char* argv[]) {

//	dataGenerator::writeToFile();
	
    std::chrono::time_point<std::chrono::system_clock> start, end;
    readFirstCharacters();



	//cout << "Creating Batch Travelers"<<endl;
    //Main:: batchTravelers();

	//cout << "Adding Batch Officers";
	//Main:: addOfficers();
	
	cout << "Progressing work"<<endl;
	start = std::chrono::system_clock::now();
	Main:: progressWork();
	end = std::chrono::system_clock::now();
	

	std::chrono::duration<double> elapsed_seconds = end-start;
	

	std::cout << "finished computation at " << endl
              << "elapsed time: " << elapsed_seconds.count() << "s\n";

	cleanup();
}
