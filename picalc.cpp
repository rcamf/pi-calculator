#include <iostream>
#include <iomanip>
#include <cstdio>
#include <fstream>
#include <string>
#include <sstream>
#include <cmath>
#include <thread>
#include <future>
#include <random>
#include <limits>
#include <chrono>
#include <ctime>
#include <vector>
using namespace std;

vector<unsigned long long> pointsPerThread;	

void generatePoints(int id, long long iterations) {
	cout << "Thread-ID: " << id << "; Status: Started calculating" << endl;
	mt19937_64 rng;
	uint64_t timeSeed = chrono::high_resolution_clock::now().time_since_epoch().count();
	seed_seq ss{uint32_t(timeSeed & 0xffffffff), uint32_t(timeSeed>>32)};
	rng.seed(ss);
	uniform_real_distribution<double> unif(0.0, nextafter(1.0, numeric_limits<double>::max()));
	unsigned long long points = 0;
	for (long long i = 0; i < iterations; ++i) {
		double x = unif(rng);
		double y = unif(rng);
		double distance = sqrt(pow(x - 0.5, 2) + pow(y - 0.5, 2));
		if (distance <= 0.5) 
			++points;
		if (i % (iterations / 10) == 0) {
			ofstream threadFile("thread-" + to_string(id));
			time_t currentTime = chrono::system_clock::to_time_t(chrono::system_clock::now());
			tm *tmLocal = localtime(&currentTime);
			threadFile << put_time(tmLocal, "%c") << ": " << i + 1 << "/" << iterations << ": " << points << endl;
			threadFile.close();
		}
	}
	cout << "Thread-ID: " << id	<< "; Status: Found " << points << " points in " << iterations << " iterations" << endl;
	pointsPerThread.push_back(points);
}

int main () {
	int noOfThreads;
	long long iterations;
	cout << "Enter number of threads: ";
	cin >> noOfThreads;
	cout << "Enter number of iterations per thread: ";
	cin >> iterations;

	thread threads[noOfThreads];
	for (int i = 0; i < noOfThreads; ++i) {
		threads[i] = thread(generatePoints, i, iterations);
		this_thread::sleep_for(chrono::milliseconds(250));
	}

	for (int i = 0; i < noOfThreads; ++i)
		threads[i].join();

	unsigned long long sumOfPoints = 0;
	for (auto i = pointsPerThread.begin(); i != pointsPerThread.end(); ++i)
		sumOfPoints += *i;
	long double approx_pi = 4.0 * sumOfPoints / (iterations * noOfThreads);
	
	ofstream results("results-" + to_string(chrono::high_resolution_clock::now().time_since_epoch().count()) + ".txt");
	cout << "Iterations: " << iterations * noOfThreads << endl;
	results <<  "Iterations: " << iterations * noOfThreads << endl;
	cout << "Sum of Points: " << sumOfPoints << endl;
	results << "Sum of Points: " << sumOfPoints << endl;
	cout << setprecision(16) << "Approximated Pi: " << approx_pi << endl;
	results << setprecision(16) << "Approximated Pi: " << approx_pi << endl;
	results.close();
		
	for (int i = 0; i < noOfThreads; ++i) 
		remove(("thread-" + to_string(i)).c_str());
}
