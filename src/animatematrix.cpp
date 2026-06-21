//============================================================================
// Name        : ThreadPoolAnimation.cpp
// Author      : Tom Teplick
// Version     :
// Copyright   : Your copyright notice
// Description : Animation using threadpools.
//============================================================================

#include <iostream>
#include <thread>
#include <chrono>
#include <iostream>
#include <Windows.h>
#include <wincon.h>
#include <cstdlib>
#include <string>
#include "animatematrix.h"

// set the submatrix with random densities
void Matrix::colorMatrix(int submatrix)
{
	constexpr int maxdensity = 10;
	int nrows = dim/numThreads;
	int startRow = nrows*submatrix;
	int endRow = startRow + nrows;
	// set the matrix with random densities in (0-10]
	for (int row = startRow; row < endRow; row++) {
		for (int col = 0; col < dim; col++) {
			mat[row][col] = std::rand() % maxdensity;
		}
	}
}

// Perform interations of coloring the matrix and queueing tasks
void Matrix::handleColorMatrix(int niters) {

	//COLORS LIST foreground 1-15
	//0: Black
	//1: Blue
	//2: Green
	//3: Cyan
	//4: Red
	//5: Purple
	//6: Brown
	//7: Default white
	//8: Gray/Grey
	//9: Bright blue
	//10: Bright green
	//11: Bright cyan
	//12: Bright red
	//13: Pink/Magenta
	//14: Yellow
	//15: Bright white
	//Numbers after 15 include background colors
	//16: background blue  0x10
	//32: background green 0x20
	//48: background cyan  0x30
	//64: background red   0x40
	//80: background purple 0x50
	//96: background brown 0x60
	//108: background white 0x70
	//124: background gray 0x80

	// map matrix density to windows color attribute

	// colors not in wincon.h
	enum Color : int {
		FOREGROUND_BLACK=0,
		FOREGROUND_CYAN=3,
		FOREGROUND_PURPLE=5,
		FOREGROUND_BROWN=6,
		FOREGROUND_WHITE=7,
		FOREGROUND_GRAY=8,
		FOREGROUND_ORANGE=12,
		FOREGROUND_YELLOW=14,
	};

	std::vector<int> density2FGcolor = {
		FOREGROUND_YELLOW,
		FOREGROUND_GRAY,
		FOREGROUND_ORANGE,
		FOREGROUND_CYAN,
		FOREGROUND_GREEN,
		FOREGROUND_BLUE,
		FOREGROUND_RED,
		FOREGROUND_PURPLE,
		FOREGROUND_BROWN,
		FOREGROUND_WHITE,
	};

	std::vector<int> density2BGcolor = {
		FOREGROUND_YELLOW << 4,
		FOREGROUND_GRAY << 4,
		FOREGROUND_ORANGE << 4,
		FOREGROUND_CYAN << 4,
		FOREGROUND_GREEN << 4,
		FOREGROUND_BLUE << 4,
		FOREGROUND_RED << 4,
		FOREGROUND_PURPLE << 4,
		FOREGROUND_BROWN  << 4,
		FOREGROUND_WHITE << 4,
	};

	HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
	CONSOLE_SCREEN_BUFFER_INFO csbiInfo;
	// Save the current text colors.
	GetConsoleScreenBufferInfo(hConsole, &csbiInfo);

	// Create tasks for the worker threads for the desired
	// number of iterations.  Divide the matrix up among the threads.
	// Each thread task colors a portion of the matrix.
	const int sleepms = 50;
	for (int iter = 0; iter < niters; ++iter) {
		for (int i = 0; i < numThreads; i++) {
			enqueue(i);
		}
		// how fast to change the matrix colors
		std::this_thread::sleep_for(std::chrono::milliseconds(sleepms));
		// read the matrix containing the densities (0-9) to be converted to colors
		for (const auto &vec : mat) {
			for (const auto &val: vec) {
				SetConsoleTextAttribute(hConsole, density2FGcolor[val] | density2BGcolor[val]);
				std::cout << "  ";
			}
			// Restore default foreground and background
			SetConsoleTextAttribute(hConsole, FOREGROUND_BLACK);
			std::cout << std::endl;
		}
	}
}

// iterate over queueing tasks to rotate the matrix colors
void Matrix::handleRotateMatrix() {
	// create pool with 5 threads

}

// Constructor to creates a thread pool with given number of threads
Matrix::Matrix(int nthreads)
{
	numThreads = nthreads;
	// Create worker threads
	for (int i = 0; i < numThreads; ++i) {
		threads.emplace_back((std::thread(&Matrix::runWorkerTask, this)));
	}
}

// Destructor to stop the thread pool
Matrix::~Matrix()
{
	{
		// Lock the queue to update the stop flag safely
		std::unique_lock<std::mutex> lock(queue_mutex);
		stop = true;
	}

	// Notify all threads
	cv.notify_all();

	// Joining all worker threads to ensure they have
	// completed their tasks
	for (auto& thread : threads) {
		thread.join();
	}
}

// Enqueue task for execution by the thread pool
void Matrix::enqueue(int subMat)
{
	{
		std::unique_lock<std::mutex> lock(queue_mutex);
		tasks.push(subMat);
	}
	cv.notify_one();
}

// run the tasks until told to stop
void Matrix::runWorkerTask()
{
	while (true) {
		// Unlock the queue before executing the task so that other
		// threads can perform enqueue tasks
		int submatrix;
		{
			// Locking the queue so that data
			// can be shared safely
			std::unique_lock<std::mutex> lock(queue_mutex);

			// Waiting until there is a task to
			// execute or the pool is stopped
			cv.wait(lock, [this] {
				return !tasks.empty() || stop;
			});

			// exit the thread in case the pool
			// is stopped and there are no tasks
			if (stop && tasks.empty()) {
				return;
			}

			// Get the next task from the queue
			submatrix = tasks.front();
			tasks.pop();
		}

		// set the matrix for this submatrix with random densities
		colorMatrix(submatrix);
	}
}

/*
void colorMatrix() {}
void rotateMatrixSpiralCW() {}
void rotateMatrixSpiralCCW() {]
void rotateMatrixSerpentineCW() {}
void rotateMatrixSerpentineCCW() {}
void rotateMatrixRowDown() {}
void rotateMatrixColumnDown() {}
void rotateMatrixRowUp() {}
void rotateMatrixColumnUp() {}

*/

int main() {

	// 1. choose operation to perform on the matrix: rotation types, random colors
	// 2. choose number of iterations of the matrix operation
	// 3. if matrix is 50x50 and #workers = 5, then each worker thread processes 250 densities/colors in the matrix
	// 4. the worker threads need access to the matrix because they will be writing to it
	// 5. the task queue contains the section of the matrix to set the density/color

	const int minIters = 0;
	const int maxIters = 200;
	int matrixOp;
	int niters;
	std::srand (time(NULL));
	std::string result = "";
	std::cout << "Choose operation to perform on the matrix and the number of iterations\n";
	std::cout << "0:quit\n1:matrix color\n2:matrix rotate spiral CW\n3:matrix rotate spiral CCW\n";
	std::cout << "4:matrix rotate serpentine CW\n5:matrix rotate serpentine CCW\n6:matrix rotate row down\n";
	std::cout << "7:matrix rotate row up\n8:matrix rotate column down\n9:matrix rotate column up --> ";
	std::cin >> matrixOp;
	std::cout << "\nEnter number of iterations (0-200)--> ";
	std::cin >> niters;

	std::cout << "You entered " << matrixOp << " " << niters << std::endl;

	if ((matrixOp < 0) || (matrixOp > 9)) {
		result += "matrix operation not in range 0-9 ";
	}
	if ((niters < minIters) || (niters > maxIters)) {
		result += "iterations not in range 0-200\n";
	}
	while (result.size() != 0) {
		std::cout << "Problems: " << result << std::endl;
		std::cout << "Enter matrix operation and number of iterations --> ";
		std::cin >> matrixOp >> niters;

		std::cout << "You entered " << matrixOp << " " << niters << std::endl;

		result.clear();
		if ((matrixOp < 0) || (matrixOp > 9)) {
			result += "matrix operation not in range 0-9 ";
		}
		if ((niters < minIters) || (niters > maxIters)) {
			result += "iterations not in range 0-200\n";
		}
	}

	// create pool with 5 threads at least
	int hwconcur = std::thread::hardware_concurrency();
	std::cout << "hardware concurrency = " << hwconcur << std::endl;
	const int numthreads = std::min(hwconcur, 5);

	// Create a Matrix instance
	Matrix matrx(numthreads);

	for (;;) {
		switch (matrixOp) {
		case 0:
			std::cout << "Good Bye\n";
			return 0;
		case 1:
			matrx.handleColorMatrix(niters);
			break;
		case 2:
			break;
		case 3:
			break;
		case 4:
			break;
		case 5:
			break;
		case 6:
			break;
		case 7:
			break;
		case 8:
			break;
		case 9:
			break;
		default:
			std::cout << "matrix operation " << matrixOp << " not valid\n";
		}

		std::cout << "Choose operation to perform on the matrix (0-9) and the number of iterations (0-200) --> ";
		std::cin >> matrixOp >> niters;

		std::cout << "You entered " << matrixOp << " " << niters << std::endl;

		result.clear();
		if ((matrixOp < 0) || (matrixOp > 9)) {
			result += "matrix operation not in range 0-9 ";
		}
		if ((niters < minIters) || (niters > maxIters)) {
			result += "iterations not in range 0-200\n";
		}
		while (result.size() != 0) {
			std::cout << "Problems: " << result << std::endl;
			std::cout << "Enter matrix operation and number of iterations --> ";
			std::cin >> matrixOp >> niters;

			std::cout << "You entered " << matrixOp << " " << niters << std::endl;

			result.clear();
			if ((matrixOp < 0) || (matrixOp > 9)) {
				result += "matrix operation not in range 0-9 ";
			}
			if ((niters < minIters) || (niters > maxIters)) {
				result += "iterations not in range 0-200\n";
			}
		}
	}

    return 0;
}
