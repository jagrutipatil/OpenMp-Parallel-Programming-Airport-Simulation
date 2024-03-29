#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <iostream>
#include <fstream>
#include <sstream>
#include "omp.h"

#define NUM_THREADS 20
#include "../queue/StampingService.h"
string filePath = "C://Users//SCS_USER//Desktop//SecondandThirdWeek//SecondWeek//275//Code//data.txt";
int count = 20;
using namespace stampingService;


namespace dataGenerator {
Traveler *travelers = new Traveler[count];

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
	Traveler *travelers = new Traveler[count];
	ofstream myfile;
	char name[22];
	myfile.open(filePath.c_str(), ios::app | ios::out | ios::in);

	for (int i = 0; i < count; i++) {
		generateName(name);
		travelers[i].setFirstName(name);
		generateName(name);
		travelers[i].setLastName(name);
		travelers[i].setStampingStatus(false);
		travelers[i].setVisaType(visa[i % 8]);
		string length = travelers[i].toString().length() + "";
		int totalLength = length.length() + 3;
		myfile << totalLength << " | " << travelers[i].toString();
	}

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
		cout << line << endl;

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
	cout << "\nStarted Processing Work";
	int i = 0;
	omp_set_num_threads(NUM_THREADS);
    #pragma omp parallel sections 
    {
        if(i<count){
            int id = omp_get_thread_num();
            i++;
            cout<<endl<<id;
        }

	#pragma omp section
	{        
		#pragma omp parallel
        {
        	while (i < count) {
            int id = omp_get_thread_num();
			cout <<endl << "\n Thread:" << id;
//			Officer* offc = stmps.getAvailableOfficer();
			if(i < count && travelers[i].ifValidVisa()) {
					travelers[i].setStampingStatus(true);
			} else {
				travelers[i].setStampingStatus(false);
			}

			#pragma omp critical
				 i++;
		}
        }
	}  
        
    #pragma omp section
	{
        #pragma omp parallel
        {
		while (i < count) {
            int id = omp_get_thread_num();
			cout <<endl << "\n Thread in here:" << id;
//			Officer* offc = stmps.getAvailableOfficer();
			if(i < count && travelers[i].ifValidVisa()) {
					travelers[i].setStampingStatus(true);
			} else {
				travelers[i].setStampingStatus(false);
			}

			#pragma omp critical
				 i++;
		}
	}
    }
}

}
void printTravelers() {
	for (int i = 0; i < count; i++) {
		cout << travelers[i].toString();
	}
  }
}


int main(int argc, char* argv[]) {
	cout << "Creating Batch Travelers";
	Main:: batchTravelers();

	cout << "\nAdding Batch Officers";
	Main:: addOfficers();

	cout << "\nProgressing work";
	Main:: progressWork();

//	cout << "Printing Travelers";
//	Main:: printTravelers();
}
