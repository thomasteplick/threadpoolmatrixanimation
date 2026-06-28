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
#include <cmath>
#include "animatematrix.h"

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

static std::vector<int> density2FGcolor = {
	FOREGROUND_YELLOW,
	FOREGROUND_GRAY,
	FOREGROUND_ORANGE,
	FOREGROUND_CYAN,
	FOREGROUND_GREEN,
	FOREGROUND_BLUE,
	FOREGROUND_RED,
	FOREGROUND_PURPLE,
	FOREGROUND_BROWN,
	FOREGROUND_WHITE
};

static std::vector<int> density2BGcolor = {
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

// one degree step size
const double delAng = 3.14159265358979323846264/180.0;

void Matrix::rotateMatrixSpiralCCW(int submatrix) {
	/*
	 * Divide the algorithm into four sections,
	 * traverse the outer perimeter of the matrix
	 * and move the elements in a CCW direction.
	 * Go to the next inner matrix and traverse it
	 * in a CCW direction the same as before.  Continue
	 * moving inward until the perimeter is one element.
	 * The movement resembles an inward spiral.
	 * | <- <- <- ^
	 * v          |
	 * |          ^
	 * v -> -> -> |
	 */

	int prev = mat[submatrix][dim-submatrix-1];
	int next;
	int rowStart = submatrix;         // index of matrix
	int rowEnd = dim - submatrix - 1; // valid index, not one past
	int colStart = dim - submatrix -1;
	int colEnd = submatrix;           // valid index, not one past
	int dimStep = dim/numThreads;
	int startStep = dimStep/2;
	int endStep = startStep;

	// loop over dimension, decreasing by ten in each iteration, starts and stops
	// decrease by five, to distribute work among threads
	for (int d = submatrix; d < dim; d+=dimStep) {
		prev = mat[d][dim - d -1];
		// traverse top of matrix going from right to left
		for (int col = colStart; col > colEnd; col--) {
			next = mat[rowStart][col-1];
			mat[rowStart][col-1] = prev;
			prev = next;
		}
		// traverse left side of matrix going top to bottom
		for (int row = rowStart; row < rowEnd; row++) {
			next = mat[row+1][colStart];
			mat[row+1][colStart] = prev;
			prev = next;
		}
		// traverse bottom of matrix going from left to right
		for (int col = colStart; col < colEnd; col++) {
			next = mat[rowEnd][col+1];
			mat[rowEnd][col+1] = prev;
			prev = next;
		}

		// traverse right side of matrix going from bottom to top
		for (int row = rowEnd; row > 0; row--) {
			next = mat[row-1][colEnd];
			mat[row-1][colEnd] = prev;
			prev = next;
		}

		// traverse inner matrix by making dimensions smaller
		rowStart += startStep;
		rowEnd -= endStep;
		colStart -= startStep;
		colEnd += endStep;
	}
}

void Matrix::rotateMatrixSpiralCW(int submatrix) {
	/*
	 * Divide the algorithm into four sections,
	 * traverse the outer perimeter of the matrix
	 * and move the elements in a CW direction.
	 * Go to the next inner matrix and traverse it
	 * in a CW direction the same as before.  Continue
	 * moving inward until the perimeter is one element.
	 * The movement resembles an inward spiral.
	 *  -> -> -> |
	 * ^         v
	 * |         |
	 * ^         v
	 * | <- <-  <-
	 *
	 */

	int prev = mat[submatrix][submatrix];
	int rowStart = submatrix;         // index of matrix
	int rowEnd = dim - submatrix - 1; // valid index, not one past
	int colStart = submatrix;
	int colEnd = dim - submatrix - 1; // valid index, not one past
	int dimStep = dim/numThreads;
	int startStep = dimStep/2;
	int endStep = startStep;

	// gain exclusive access to mat
	std::unique_lock<std::mutex> lock(mat_mutex);

	// loop over dimension, decreasing by ten in each iteration, starts and stops
	// decrease by five, to distribute work among threads
	for (int d = submatrix; d < dim; d+=dimStep) {
		prev = mat[rowStart][colStart];
		// traverse top of matrix going from left to right
		for (int col = colStart; col < colEnd; col++) {
			int next = mat[rowStart][col+1];
			mat[rowStart][col+1] = prev;
			prev = next;
		}

		// traverse right side of matrix going top to bottom
		for (int row = rowStart; row < rowEnd; row++) {
			int next = mat[row+1][colEnd];
			mat[row+1][colEnd] = prev;
			prev = next;
		}

		// traverse bottom of matrix going from right to left
		for (int col = colEnd; col > colStart; col--) {
			int next = mat[rowEnd][col-1];
			mat[rowEnd][col-1] = prev;
			prev = next;
		}

		// traverse left side of matrix going from bottom to top
		for (int row = rowEnd; row > rowStart; row--) {
			int next = mat[row-1][colStart];
			mat[row-1][colStart] = prev;
			prev = next;
		}

		// traverse inner matrix by making dimensions smaller
		rowStart += startStep;
		rowEnd -= endStep;
		colStart += startStep;
		colEnd -= endStep;
	}

}

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

	// map matrix density to windows color attribute

	HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
	CONSOLE_SCREEN_BUFFER_INFO csbiInfo;
	// Save the current text colors.
	GetConsoleScreenBufferInfo(hConsole, &csbiInfo);
	const int colorMatrix = 0;

	// Create tasks for the worker threads for the desired
	// number of iterations.  Divide the matrix up among the threads.
	// Each thread task colors a portion of the matrix.
	for (int iter = 0; iter < niters; ++iter) {
		tasksdone = 0;
		for (int i = 0; i < numThreads; i++) {
			enqueue(std::make_pair(colorMatrix, i));
		}
		const int sleepms = 25;

		while (tasksdone != numThreads) {
			std::this_thread::sleep_for(std::chrono::milliseconds(sleepms));
		}
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

// rotate the matrix CW in a spiral motion
void Matrix::handleRotateMatrixSpiralCW(int niters) {
	// map matrix density to windows color attribute

	HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
	CONSOLE_SCREEN_BUFFER_INFO csbiInfo;
	// Save the current text colors.
	GetConsoleScreenBufferInfo(hConsole, &csbiInfo);

	// Create tasks for the worker threads for the desired
	// number of iterations.  Divide the matrix up among the threads.
	// Each thread task colors a portion of the matrix.

	// create the densities and show their colors
	//const int sleepms = 25;
	constexpr int maxdensity = 10;

	int rowStart = 0;     // index of matrix
	int rowEnd = dim - 1; // valid index, not one past
	int colStart = 0;
	int colEnd = dim - 1; // valid index, not one past
	int dimStep = 2;  // matrix dimension is reduced by 2 each iteration
	int startStep = dimStep/2;
	int endStep = startStep;

	// Set color according to matrix size and the side of the matrix
	// Spiral inward along the outer perimeter of the matrix.
	for (int d = 0; d < dim; d+=dimStep) {
		// traverse top of matrix going from left to right
		// traverse bottom of matrix going from right to left
		int density1 = std::rand()%maxdensity;
		int density2 = std::rand()%maxdensity;
		for (int col = colStart; col < colEnd; col++) {
			mat[rowStart][col] = density1;
			mat[rowEnd][col+1] = density2;
		}

		// traverse right side of matrix going top to bottom
		// traverse left side of matrix going from bottom to top
		density1 = std::rand()%maxdensity;
		density2 = std::rand()%maxdensity;
		for (int row = rowStart; row < rowEnd; row++) {
			mat[row+1][colStart] = density1;
			mat[row][colEnd] = density2;
		}

		// traverse inner matrix by making dimensions smaller
		rowStart += startStep;
		rowEnd -= endStep;
		colStart += startStep;
		colEnd -= endStep;
	}

	// read the matrix values containing the densities (0-9) and convert to FG and BG colors
	for (const auto &vec : mat) {
		for (const auto &val: vec) {
			SetConsoleTextAttribute(hConsole, density2FGcolor[val] | density2BGcolor[val]);
			std::cout << "  ";
		}
		// Restore default foreground and background
		SetConsoleTextAttribute(hConsole, FOREGROUND_BLACK);
		std::cout << std::endl;
	}

	// rotate the densities created above and show their colors
	const int rotateMatrixSpiralCW = 1;
	for (int iter = 0; iter < niters; ++iter) {

		// Reset to zero each iteration
		tasksdone = 0;

		// queue the tasks for the worker threads
		for (int i = 0; i < numThreads; i++) {
			enqueue(std::make_pair(rotateMatrixSpiralCW, i));
		}

		const int sleepms = 25;
		// Wait for the worker threads to finish their tasks
		while (tasksdone != numThreads) {
			std::this_thread::sleep_for(std::chrono::milliseconds(sleepms));
		}

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

// rotate the matrix CCW in a spiral motion
void Matrix::handleRotateMatrixSpiralCCW(int niters) {
	// map matrix density to windows color attribute

	HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
	CONSOLE_SCREEN_BUFFER_INFO csbiInfo;
	// Save the current text colors.
	GetConsoleScreenBufferInfo(hConsole, &csbiInfo);

	// Create tasks for the worker threads for the desired
	// number of iterations.  Divide the matrix up among the threads.
	// Each thread task colors a portion of the matrix.

	// create the densities and show their colors
	constexpr int maxdensity = 10;

	int rowStart = 0;     // index of matrix
	int rowEnd = dim - 1; // valid index, not one past
	int colStart = 0;
	int colEnd = dim - 1; // valid index, not one past
	int dimStep = 2;  // matrix dimension is reduced by 2 each iteration
	int startStep = dimStep/2;
	int endStep = startStep;

	// Set color according to matrix size and the side of the matrix
	// Spiral inward along the outer perimeter of the matrix.
	for (int d = 0; d < dim; d+=dimStep) {
		// traverse top of matrix going from left to right
		// traverse bottom of matrix going from right to left
		int density1 = std::rand()%maxdensity;
		int density2 = std::rand()%maxdensity;
		for (int col = colStart; col < colEnd; col++) {
			mat[rowStart][col] = density1;
			mat[rowEnd][col+1] = density2;
		}

		// traverse right side of matrix going top to bottom
		// traverse left side of matrix going from bottom to top
		density1 = std::rand()%maxdensity;
		density2 = std::rand()%maxdensity;
		for (int row = rowStart; row < rowEnd; row++) {
			mat[row+1][colStart] = density1;
			mat[row][colEnd] = density2;
		}

		// traverse inner matrix by making dimensions smaller
		rowStart += startStep;
		rowEnd -= endStep;
		colStart += startStep;
		colEnd -= endStep;
	}

	// read the matrix values containing the densities (0-9) and convert to FG and BG colors
	for (const auto &vec : mat) {
		for (const auto &val: vec) {
			SetConsoleTextAttribute(hConsole, density2FGcolor[val] | density2BGcolor[val]);
			std::cout << "  ";
		}
		// Restore default foreground and background
		SetConsoleTextAttribute(hConsole, FOREGROUND_BLACK);
		std::cout << std::endl;
	}

	// rotate the densities created above and show their colors
	const int rotateMatrixSpiralCCW = 2;
	for (int iter = 0; iter < niters; ++iter) {
		tasksdone= 0;
		for (int i = 0; i < numThreads; i++) {
			enqueue(std::make_pair(rotateMatrixSpiralCCW, i));
		}
		const int sleepms = 25;

		while (tasksdone != numThreads) {
			std::this_thread::sleep_for(std::chrono::milliseconds(sleepms));
		}
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

// Constructor to create a thread pool with given number of threads
Matrix::Matrix(int nthreads)
{

	numThreads = nthreads;
	tasksdone = 0;
	stop = false;
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
void Matrix::enqueue(std::pair<int,int> task)
{
	{
		// access the shared data safely - critical section
		std::unique_lock<std::mutex> lock(queue_mutex);
		tasks.push(task);
	}
	cv.notify_one();
}

// run the tasks until told to stop
void Matrix::runWorkerTask()
{
	// assign the member functions address to the array of member function pointers
	// Do not change the order of the functions since the handle functions rely on
	// this order.
	Matrix::matrixMbr matfcn[] = {
	    &Matrix::colorMatrix,
		&Matrix::rotateMatrixSpiralCW,
		&Matrix::rotateMatrixSpiralCCW,
	};

	while (true) {
		// Unlock the queue before executing the task so that other
		// threads can perform enqueue tasks
		std::pair<int,int> task;
		{
			// Locking the queue so that data
			// can be shared safely - critical section
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
			task = tasks.front();
			tasks.pop();
		}

		// run the task using first=member matrix function, second=submatrix
		(this->*matfcn[task.first])(task.second);

		// Atomically increment this thread finished its task
		tasksdone++;

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
			matrx.handleRotateMatrixSpiralCW(niters);
			break;
		case 3:
			matrx.handleRotateMatrixSpiralCCW(niters);
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
