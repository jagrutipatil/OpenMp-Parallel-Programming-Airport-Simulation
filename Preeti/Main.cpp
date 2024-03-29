#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <boost/random/mersenne_twister.hpp>
#include <boost/random/uniform_int.hpp>
#include <boost/random/variate_generator.hpp>
#include <string.h>
#include <iostream>
#include <fstream>
#include <sstream>
#include "omp.h"
#include <ctime>
#include <chrono>
#include <thread>
#include <unistd.h>




using namespace std;
boost::mt19937 gen;

#define NUM_THREADS 8
#include "../queue/StampingService.h"
string filePath = "C://Users//SCS_USER//Desktop//SecondandThirdWeek//SecondWeek//275//copy//Code//data.txt";
int count2 = 60000;
int *arr = new int[count2];

Traveler *travelers = new Traveler[count2];
string q1ans[]  = {"India", "USA", "China", "Bangladesh"};
string felony[]  = {"yes", "no", "minor", "major"};


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
		Traveler *ltravelers = new Traveler[count2];
		ofstream myfile;
		char name[22];
		myfile.open(filePath.c_str(), ios::app | ios::out | ios::in);

		for (int i = 0; i < count2; i++) {
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
		Traveler *travelers = new Traveler[count2];
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
	Traveler *travelers = new Traveler[count2];
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

			if (i < count2) {

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
		for (int i = 0; i < count2; i++) {
			Officer* offc = new Officer;

			dataGenerator::generateName(name);
			offc->setFirstName(name);
			dataGenerator::generateName(name);
			offc->setLastName(name);
			offc->setProcessingTime(1.0);
			stmps.addOfficer(offc);
		}
	}

	bool allQuestionsAnswered(Traveler travelers) {
		usleep(500);
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

			for(i=omp_get_thread_num(); i < count2; i=i + omp_get_num_threads()) {
				infile.clear();
				if(i < count2) {
					infile.seekg(arr[i]);
				}
				getline(infile, line);
				std::istringstream iss(line);

				char * dup = strdup(line.c_str());
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

				if (i < count2) {
					travelers[i].setFirstName(firstname);
				}

				if (i < count2) {
					travelers[i].setLastName(lastname);
				}

				if (i < count2) {
					travelers[i].setVisaType(visaType);
				}

				if(i < count2 && travelers[i].ifValidVisa()) {
					if (allQuestionsAnswered(travelers[i])) {
						travelers[i].setStampingStatus(true);
					}
				} else if( i < count2) {
					travelers[i].setStampingStatus(false);
				}
			}
		}
	}


	void doSections() {
		cout << "\nStarted Processing Work";
		int i = 0;
		omp_set_num_threads(NUM_THREADS);
		#pragma omp parallel sections
		  {
			if (i < count2) {
				int id = omp_get_thread_num();
				i++;
				cout << endl << id;
			}

		#pragma omp section
		{
			#pragma omp parallel
				{
					while (i < count2) {
						int id = omp_get_thread_num();
						cout << endl << "\n Thread:" << id;
						if (i < count2 && travelers[i].ifValidVisa()) {
							travelers[i].setStampingStatus(true);
						} else {
							travelers[i].setStampingStatus(false);
						}

						#pragma omp atomic
							i++;
					}
				}
		}

		#pragma omp section
			{
				#pragma omp parallel
					{
						while (i < count2) {
							int id = omp_get_thread_num();
							cout << endl << "\n Thread in here:" << id;
							if (i < count2 && travelers[i].ifValidVisa()) {
									travelers[i].setStampingStatus(true);
							} else {
								travelers[i].setStampingStatus(false);
							}

						#pragma omp atomic
							i++;
				}
			}
		}
	}
  }
	void progressWork() {
		cout << "\nStarted Sequential Processing Work"<<endl;
			int i = 0;
			std::ifstream infile(filePath.c_str());
			string line;
			int byte;
			string firstname, lastname, visaType, isValidVisa, isStampingDone;

			for(i = 0; i < count2; i = i + 1) {
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
					if (allQuestionsAnswered(travelers[i])) {
						travelers[i].setStampingStatus(true);
		   		    }
				} else if( i < count2) {
					travelers[i].setStampingStatus(false);
				}
			}
		}



	void printTravelers() {
		for (int i = 0; i < count2; i++) {
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
			if (i<count2) {
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

int randomize() {
    boost::uniform_int<> dist(1, 5);
    boost::variate_generator<boost::mt19937&, boost::uniform_int<> > die(gen, dist);
    return die();
}
int main(int argc, char* argv[]) {

//	dataGenerator::writeToFile();
    //Introducing TIME DELAY
    std::chrono::time_point<std::chrono::system_clock> currTime, endCurrTime;
    currTime = std::chrono::system_clock::now();
    cout<<"Start Delay !!";
    
    
    endCurrTime = std::chrono::system_clock::now();
    std::chrono::duration<double> diff = endCurrTime-currTime;
    //std::cout << "finished computation at " << endl
      //        << "elapsed time: " << diff.count() << "s\n";
    
    int section = randomize();
    currTime = std::chrono::system_clock::now();
    endCurrTime = std::chrono::system_clock::now();
    diff = endCurrTime-currTime;
    
   //cout<< section - int(diff.count());
    
    while(double(section/100) - double(diff.count()) >= 0){
        //std::string str="We think in generalities, but we live in details.";
        //std::string str2 = str.substr (3,5);     
        //std::size_t pos = str.find("live");      
        //std::string str3 = str.substr (pos);    
        //cout<<str2;
        endCurrTime = std::chrono::system_clock::now();
        diff = endCurrTime-currTime;
        //std::cout << "finished computation at " << endl
          //    << "elapsed time: " << diff.count() << "s\n";
    }
   
	cout << "************************Parallel Work Statistics******************************";
    std::chrono::time_point<std::chrono::system_clock> start, end;
    readFirstCharacters();

	
	cout << endl << "Progressing work"<<endl;

	start = std::chrono::system_clock::now();
	Main:: progressWorkParallel();
	end = std::chrono::system_clock::now();

	std::chrono::duration<double> elapsed_seconds = end-start;

	std::cout << "finished computation at " << endl
              << "elapsed time: " << elapsed_seconds.count() << "s\n";
	cout << "************************Sequential Work Statistics******************************";

	cout << endl << "Progressing work"<<endl;
	start = std::chrono::system_clock::now();
	Main:: progressWork();
	end = std::chrono::system_clock::now();

	elapsed_seconds = end-start;

	std::cout << "finished computation at " << endl
              << "elapsed time: " << elapsed_seconds.count() << "s\n";

	cout << "******************************************************"<< endl;

//	Main::printTravelers();
	cleanup();
}
