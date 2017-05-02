//COSC3360 Assignment 3
//Xin Zhou 1367925
#include<iostream>
#include <stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<fstream>
#include<string>
#include<pthread.h>
#include<time.h>
using namespace std;

static pthread_mutex_t mutex;
static int bridgeMaxLoad = 10;
static int bridgeCurrentLoad = 0;
static int cTime = 0;
static int vehicleWaitingList = 0;

struct struct_vehicle {
	string plateNumber;
	int weight;
	int arrival_delay;
	int processTime;
	int ticketNumber;
};

void *vehicle(void *arg) {
	struct_vehicle *vehicle = (struct_vehicle*) arg;

	pthread_mutex_t mutex;
	pthread_mutex_init(&mutex, NULL);
	static pthread_cond_t ok = PTHREAD_COND_INITIALIZER;

	cout << "Vehicle " << vehicle->plateNumber << " arrives at bridge. " << endl;
	cout << "The current bridge load is " << bridgeCurrentLoad << endl;
	if( vehicle->weight > bridgeMaxLoad ){
		cout << "Vehicle " << vehicle->plateNumber << " exceeds maximum bridge load." << endl;
		pthread_exit(0);
	}
	//cout << "--Vehicle ticket number: " <<vehicle->ticketNumber << endl;
	if((vehicle->weight > (bridgeMaxLoad - bridgeCurrentLoad))) {
		pthread_mutex_lock(&mutex);
		pthread_cond_wait(&ok, &mutex);
		while (! ((vehicle->weight <= (bridgeMaxLoad - bridgeCurrentLoad)) && (vehicleWaitingList == vehicle->ticketNumber - 1)) ) {
			pthread_cond_wait(&ok, &mutex);
		}
		bridgeCurrentLoad += vehicle->weight;
		cout << "Vehicle " << vehicle->plateNumber << " goes on the bridge." << endl;
		cout << "The current bridge load is " << bridgeCurrentLoad << endl;
		vehicleWaitingList = vehicle->ticketNumber;
		pthread_cond_signal(&ok);
		sleep(vehicle->processTime); // processing
		bridgeCurrentLoad -= vehicle->weight;
		cout << "Vehicle " << vehicle->plateNumber << " leaves the bridge." << endl;
		cout << "The current bridge load is " << bridgeCurrentLoad << endl;
		pthread_cond_signal(&ok);
		pthread_mutex_unlock(&mutex);
	}
	else {
		//pthread_cond_signal(&ok);
		pthread_mutex_lock(&mutex);
		bridgeCurrentLoad += vehicle->weight;
		cout << "Vehicle " << vehicle->plateNumber << " goes on the bridge." << endl;
		cout << "The current bridge load is " << bridgeCurrentLoad << endl;
		vehicleWaitingList = vehicle->ticketNumber;
		sleep(vehicle->processTime); // processing
		bridgeCurrentLoad -= vehicle->weight;
		cout << "Vehicle " << vehicle->plateNumber << " leaves the bridge." << endl;
		cout << "The current bridge load is " << bridgeCurrentLoad << endl;
		pthread_cond_signal(&ok);
		pthread_mutex_unlock(&mutex);
	}

	pthread_exit(0);
}

int main(int argc, char *argv[]) {
	struct_vehicle vehicleList[100];
	pthread_t tid[100];	// ID
	int count = 0;
	bridgeMaxLoad = atoi(argv[1]) ;
	cout << "Maximum bridge load is " << bridgeMaxLoad << " tons." << endl;
	string plate;
	int weight, arrival_delay, processTime;

	while (!cin.eof()) {
		cin >> plate;
		cin >> arrival_delay;
		cin >> weight;
		cin >> processTime;
		vehicleList[count].plateNumber = plate;
		vehicleList[count].arrival_delay = arrival_delay;
		vehicleList[count].weight = weight;
		vehicleList[count].processTime = processTime;
		vehicleList[count].ticketNumber = count + 1;
		count++;
	}
	//for (int i = 0; i < count; i++) {
	//	cout << vehicleList[i].ticketNumber << " ";
	//	cout << vehicleList[i].plateNumber << " ";
	//	cout << vehicleList[i].arrival_delay << " ";
	//	cout << vehicleList[i].weight << " ";
	//	cout << vehicleList[i].processTime << endl;
	//}
	for (int i = 0; i < count; i++)
	{
		cTime += vehicleList[i].arrival_delay;
		sleep(vehicleList[i].arrival_delay);
		pthread_create(&tid[i], NULL, vehicle, (void *) &vehicleList[i]);
	}
	for (int i = 0; i < count; i++)
	{
		pthread_join(tid[i], NULL);
	}
	cout << "Total number of vehicles: " << count << endl;
	pthread_exit(0);
	return 0;
}
