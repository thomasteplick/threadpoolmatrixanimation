//============================================================================
// Name        : ThreadPoolAnimation.cpp
// Author      : Tom Teplick
// Version     :
// Copyright   : Your copyright notice
// Description : Animation using threadpools.
//============================================================================

#include <iostream>
#include <fstream>
#include <thread>
#include <chrono>
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

const double pi = 3.14159265358979323846264;
// one degree step size
const double delAng = pi/180.0;
// static constant members
const std::string Matrix::namedensities = "nametjt.txt"; // matrix density, 50x50
const std::string Matrix::dataDir = "..\\data\\";  // directory for geometric objects



// generate solid multi-colored squares of varying sizes
void Matrix::genGeometricFigs2()
{
	constexpr int nfigs = 15;
	constexpr int maxrad = 10;
	// don't want white = 9, want 0-8
	constexpr int maxdensity = 9;
	// number of increment steps to draw the figure
	constexpr int nsteps = 25;
	// rad = length/2 of square
	int rad;
	double x, y;
	int midx, midy;
	double incr;
	// loop over the number of figures
	for (int fig = 0; fig < nfigs; fig++) {
		// generate random radius in [0,10), (x,y) coordinate in (0,50]
		// loop until square is inside (0,50] matrix dimensions
		do {
			rad = std::rand()%maxrad + 3;
			midx = std::rand()%dim;
			midy = std::rand()%dim;
		} while ((midx-rad)<=0 || (dim-midx-rad)<=0 ||
				(midy-rad)<=0 || (dim-midy-rad)<=0);

		// create the density in (1, 10], skip yellow=0
		int density = std::rand() % maxdensity + 1;
		// square:  x = +/- rad @ y1,y2  y = +- rad @ x1,x2
		incr = double(2*rad)/nsteps;
		x = double(midx-rad);
		y = double(midy-rad);
		for (int i = 0; i <= nsteps; i++) {
			x = double(midx-rad);
			for (int j = 0; j <= nsteps; j++) {
				mat[int(y)][int(x)] = density;
				x=double(x+incr);
			}
			y=double(y+incr);
		}
	}
}

// generate random geometric 2-D figures such as circles and squares of
// varying sizes and colors.
void Matrix::genGeometricFigs()
{
	constexpr int nfigs = 15;
	constexpr int maxrad = 10;
	// don't want white = 9, want 0-8
	constexpr int maxdensity = 9;
	// number of increment steps to draw the figure
	constexpr int nsteps = 25;
	// radius of circle, length/2 of square
	int rad;
	double x, y;
	int midx, midy;
	double incr;
	// loop over the number of figures
	for (int fig = 0; fig < nfigs; fig++) {
		// generate random radius in [0,10), (x,y) coordinate in (0,50]
		// loop until circle/square is inside (0,50] matrix dimensions
		do {
			rad = std::rand()%maxrad + 3;
			midx = std::rand()%dim;
			midy = std::rand()%dim;
		} while ((midx-rad)<=0 || (dim-midx-rad)<=0 ||
				(midy-rad)<=0 || (dim-midy-rad)<=0);

		// create the density in (1, 10], skip yellow=0
		int density = std::rand() % maxdensity + 1;
		// create circle, triangle, or square - 3 figures
		switch (std::rand() % 3) {
		case 0:
			// square:  x +/- rad @ y1,y2  y +- rad @ x1,x2
			incr = double(2*rad)/nsteps;
			x = double(midx-rad);
			y = double(midy-rad);
			for (int i = 0; i <= nsteps; i++) {
				mat[midy+rad][int(x)] = density;
				mat[midy-rad][int(x)] = density;
				mat[int(y)][midx+rad] = density;
				mat[int(y)][midx-rad] = density;
				x=double(x+incr);
				y=double(y+incr);
			}
			mat[midy+rad][midx+rad] = density;
			break;
		case 1:
			// triangle, choose a point p1 on radius, generate two other points
			// +/-120 degrees from p1.  use point-slope formula to generate the
			// lines connecting the vertices of the triangle. y2 = y1+m*(x2-x1)
			// m=(y2-y1)/(x2-x1), delta x = (x2-x1)/npoints, use floating point

			// generate first point on radius, other ones +/-120 deg from first
		{
			double phi1 = pi*double(std::rand())/double(RAND_MAX);
			double phi2 = phi1+2.0*pi/3.0;
			double phi3 = phi1-2.0*pi/3.0;

			double x1 = rad*std::cos(phi1);
			double y1 = rad*std::sin(phi1);

			double x2= rad*std::cos(phi2);
			double y2 = rad*std::sin(phi2);

			double x3 = rad*std::cos(phi3);
			double y3 = rad*std::sin(phi3);

			// draw line from x1 to x2
			// slope m, xdelta, ydelta
			//double m = (y2-y1)/(x2-x1);
			double delx = (x2-x1)/nsteps;
			double dely = (y2-y1)/nsteps;
			double x = midx+x1;
			double y = midy+y1;
			for (int i = 0; i < nsteps; i++) {
				mat[int(y)][int(x)] = density;
				x+=delx;
				y+=dely;
			}

			// draw line from x1 to x3
			// slope m, deltax, deltay
			//m = (y3-y1)/(x3-x1);
			delx = (x3-x1)/nsteps;
			dely = (y3-y1)/nsteps;
		    x = midx+x1;
			y = midy+y1;
			for (int i = 0; i < nsteps; i++) {
				mat[int(y)][int(x)] = density;
				x+=delx;
				y+=dely;
			}

			// draw line from x2 to x3
			// slope m, deltax, deltay
			//m = (y3-y2)/(x3-x2);
			delx = (x3-x2)/nsteps;
			dely = (y3-y2)/nsteps;
		    x = midx+x2;
			y = midy+y2;
			for (int i = 0; i < nsteps; i++) {
				mat[int(y)][int(x)] = density;
				x+=delx;
				y+=dely;
			}
			break;
		}
		case 2:
			// circle:  x +/- rad*cos(ang), y +/- rad*sin(ang), ang = (0, 90)
			incr = pi/(2.0*nsteps);
			double ang = 0.0;
			// use symmetry of trig functions to only compute quadrant (0,pi/4)
			for (int i = 0; i < nsteps; i++) {
				x = rad*std::cos(ang);
				y = rad*std::sin(ang);
				mat[int(midy+y)][int(midx+x)] = density;
				mat[int(midy-y)][int(midx+x)] = density;
				mat[int(midy+y)][int(midx-x)] = density;
				mat[int(midy-y)][int(midx-x)] = density;
				ang+=incr;
			}
			mat[midy][midx+rad] = 0;
			break;
		}
	}
}

void Matrix::loadMatrixDensity()
{
	// Open the matrix file containing the densities
	std::fstream fmatrix;
	fmatrix.open((dataDir+namedensities).c_str(), std::fstream::in);
	if (!fmatrix.is_open()) {
		std::cout << "cannot open file " + namedensities << std::endl;
		throw std::runtime_error("cannot open file " + namedensities);
	}

	// Read the matrix density file
	for (int i = 0; i < dim; i++) {
		for (int j = 0; j < dim; j++) {
			fmatrix >> mat[i][j];
		}
	}
	fmatrix.close();
}

// Translate each column left, wrap the first column to the last
void Matrix::rotateMatrixColumnLeft(int submatrix)
{
	/*
	 * Translate each column left. The first column becomes the last column.
	 */

	/*
	 * Divide the matrix into numThreads sections.
	 * Each section consists of all the columns and
	 * dim/numThreads rows.  Each section-thread
	 * moves the columns left.  The first column wraps
	 * around to the last column.
	 *
	 * | <- <- <-|
	 * | <- <- <-|
	 * | <- <- <-|
	 * | <- <- <-|
	 * | <- <- <-|
	 */
	int prev;
	int next;
	int nrows = dim/numThreads;
	int startRow = nrows*submatrix;
	int endRow = startRow + nrows;

	for (int row = startRow; row < endRow; row++) {
		prev = mat[row][dim-1];
		mat[row][dim-1] = prevRow[row];;
		for (int col = dim-1; col > 0; col--) {
			next = mat[row][col-1];
			mat[row][col-1] = prev;
			prev = next;
		}
	}
}

void Matrix::rotateMatrixColumnRight(int submatrix)
{
	/*
	 * Translate each column right. The last column becomes the first column.
	 */

	/*
	 * Divide the matrix into numThreads sections.
	 * Each section consists of all the columns and
	 * dim/numThreads rows.  Each section-thread
	 * moves the columns right.  The last column wraps
	 * around to the first column.
	 *
	 * |-> -> ->|
	 * |-> -> ->|
	 * |-> -> ->|
	 * |-> -> ->|
	 * |-> -> ->|
	 */
	int prev;
	int next;
	int nrows = dim/numThreads;
	int startRow = nrows*submatrix;
	int endRow = startRow + nrows;

	for (int row = startRow; row < endRow; row++) {
		prev = mat[row][0];
		mat[row][0] = prevRow[row];;
		for (int col = 0; col < dim-1; col++) {
			next = mat[row][col+1];
			mat[row][col+1] = prev;
			prev = next;
		}
	}
}

void Matrix::rotateMatrixRowDown(int submatrix)
{
	/*
	 * Translate each row down. The last row becomes the first row.
	 */

	/*
	 * Divide the matrix into numThreads sections.
	 * Each section consists of all the rows and
	 * dim/numThreads columns.  Each section-thread
	 * moves the rows down.  The bottom row wraps
	 * around to the first row.
	 *
	 *  | | | | ...
	 *  v v v v
	 *  | | | |
	 *  v v v v
	 *  | | | |
	 *  v v v v
	 *  | | | | ...
	 *  v v v v
	 */
	int prev;
	int next;
	int ncols = dim/numThreads;
	int startCol = ncols*submatrix;
	int endCol = startCol + ncols;

	for (int col = startCol; col < endCol; col++) {
		prev = mat[0][col];
		mat[0][col] = prevRow[col];;
		for (int row = 0; row < dim-1; row++) {
			next = mat[row+1][col];
			mat[row+1][col] = prev;
			prev = next;
		}
	}
}

void Matrix::rotateMatrixRowUp(int submatrix)
{
	/*
	 * Translate each row up. The first row becomes the last row.
	 */

	/*
	 * Divide the matrix into numThreads sections.
	 * Each section consists of all the rows and
	 * dim/numThreads columns.  Each section-thread
	 * moves the rows up.  The top row wraps
	 * around to the last row.
	 *
	 *  ^ ^ ^ ^ ...
	 *  | | | |
	 *  ^ ^ ^ ^
	 *  | | | |
	 *  ^ ^ ^ ^
	 *  | | | |
	 *  ^ ^ ^ ^
	 *  | | | | ...
	 */
	int prev;
	int next;
	int ncols = dim/numThreads;
	int startCol = ncols*submatrix;
	int endCol = startCol + ncols;

	for (int col = startCol; col < endCol; col++) {
		prev = mat[dim-1][col];
		mat[dim-1][col] = prevRow[col];;
		for (int row = dim-1; row > 0; row--) {
			next = mat[row-1][col];
			mat[row-1][col] = prev;
			prev = next;
		}
	}
}

void Matrix::rotateMatrixSerpentineCCW(int submatrix)
{
	/*
	 * Translate each row to the left and feed the previous row
	 * output to the next row input.  The last row output feeds
	 * the first row input.
	 */

	/*
	 * Divide the algorithm into four sections,
	 * traverse the rows of the matrix
	 * and move the elements in a CCW direction.
	 * Go to the next row and traverse it
	 * in a CWW direction the same as before.
	 * The movement goes back and forth across the columns
	 * as you descend the rows.
	 * | <- <- <-|
	 * | <- <- <-|
	 * | <- <- <-|
	 * | <- <- <-|
	 * | <- <- <-|
	 */

	int prev;
	int next;
	int nrows = dim/numThreads;
	int startRow = dim - nrows*submatrix -1;
	int endRow = startRow - nrows;

	for (int row = startRow; row > endRow; row--) {
		prev = mat[row][dim-1];
		mat[row][dim-1] = prevRow[row];;
		for (int col = dim-1; col > 0; col--) {
			next = mat[row][col-1];
			mat[row][col-1] = prev;
			prev = next;
		}
	}
}


// Rotate the matrix in CW serpentine manner
void Matrix::rotateMatrixSerpentineCW(int submatrix)
{
	/*
	 * Translate each row to the right and feed the previous row
	 * output to the next row input.  The last row output feeds
	 * the first row input.
	 */

	/*
	 * Divide the algorithm into four sections,
	 * traverse the rows of the matrix
	 * and move the elements in a CW direction.
	 * Go to the next row and traverse it
	 * in a CW direction the same as before.
	 * The movement goes back and forth across the columns
	 * as you descend the rows.
	 *  |-> -> -> |
	 *  |-> -> -> |
	 *  |-> -> -> |
	 *  |-> -> -> |
	 *  |-> -> -> |
	 */

	int prev;
	int next;
	int nrows = dim/numThreads;
	int startRow = nrows*submatrix;
	int endRow = startRow + nrows;

	for (int row = startRow; row < endRow; row++) {
		prev = mat[row][0];
		mat[row][0] = prevRow[row];;
		for (int col = 0; col < dim-1; col++) {
			next = mat[row][col+1];
			mat[row][col+1] = prev;
			prev = next;
		}
	}
}

// Rotate the matrix 90 degrees counter-clockwise
void Matrix::rotateMatrix90CCW(int submatrix)
{
	/*
	 * Rotate the matrix 90 degrees CCW.  Swap rows and columns using
	 * local buffer matrix.
	 */

	// gain exclusive access to mat
	//std::unique_lock<std::mutex> lock(mat_mutex);

	// Place the columns of input in the rows of output in reverse order.
	int ncols = dim/numThreads;
	int startCol = ncols*submatrix;
	int endCol = startCol + ncols;

	for (int col = startCol; col < endCol; col++) {
		for (int row = 0; row < dim; row++) {
			matbuf[dim-col-1][row] = mat[row][col];
		}
	}
}

// Rotate the matrix 90 degrees clockwise
void Matrix::rotateMatrix90CW(int submatrix)
{
	/*
	 * Rotate the matrix 90 degrees CW.  Swap rows and columns using
	 * local buffer matrix .
	 */

	// gain exclusive access to mat
	//std::unique_lock<std::mutex> lock(mat_mutex);

	// Place the columns of input in the rows of output in reverse order.
	int ncols = dim/numThreads;
	int startCol = ncols*submatrix;
	int endCol = startCol + ncols;
	for (int col = startCol; col < endCol; col++) {
		for (int row = 0; row < dim; row++) {
			matbuf[col][dim-row-1] = mat[row][col];
		}
	}
}

void Matrix::rotateMatrixSpiralCCW(int submatrix)
{
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
	int colStart = dim - submatrix -1; // valid index, not one past
	int colEnd = submatrix;
	int dimStep = dim/numThreads;
	int startStep = dimStep/2;
	int endStep = startStep;

	// gain exclusive access to mat
	std::unique_lock<std::mutex> lock(mat_mutex);

	// loop over dimension, decreasing by ten in each iteration, starts and stops
	// decrease by five, to distribute work among threads
	for (int d = submatrix; d < dim; d+=dimStep) {
		prev = mat[rowStart][colStart];
		// traverse top of matrix going from right to left
		for (int col = colStart; col > colEnd; col--) {
			next = mat[rowStart][col-1];
			mat[rowStart][col-1] = prev;
			prev = next;
		}
		// traverse left side of matrix going top to bottom
		for (int row = rowStart; row < rowEnd; row++) {
			next = mat[row+1][colEnd];
			mat[row+1][colEnd] = prev;
			prev = next;
		}
		// traverse bottom of matrix going from left to right
		for (int col = colEnd; col < colStart; col++) {
			next = mat[rowEnd][col+1];
			mat[rowEnd][col+1] = prev;
			prev = next;
		}

		// traverse right side of matrix going from bottom to top
		for (int row = rowEnd; row > rowStart; row--) {
			next = mat[row-1][colStart];
			mat[row-1][colStart] = prev;
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
	int next;
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
			next = mat[rowStart][col+1];
			mat[rowStart][col+1] = prev;
			prev = next;
		}

		// traverse right side of matrix going top to bottom
		for (int row = rowStart; row < rowEnd; row++) {
			next = mat[row+1][colEnd];
			mat[row+1][colEnd] = prev;
			prev = next;
		}

		// traverse bottom of matrix going from right to left
		for (int col = colEnd; col > colStart; col--) {
			next = mat[rowEnd][col-1];
			mat[rowEnd][col-1] = prev;
			prev = next;
		}

		// traverse left side of matrix going from bottom to top
		for (int row = rowEnd; row > rowStart; row--) {
			next = mat[row-1][colStart];
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
		// Reset tasksdone each iteration
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

// rotate the matrix 90 degrees CCW niters times
void Matrix::handleRotateMatrix90CCW(int niters) {
	/*
	 * Rotate the matrix 90 degrees CCW.  Swap rows and columns using
	 * two matrices and swapping them back and forth.  Cannot be done
	 * in place.
	 */
	// map matrix density to windows color attribute

	HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
	CONSOLE_SCREEN_BUFFER_INFO csbiInfo;
	// Save the current text colors.
	GetConsoleScreenBufferInfo(hConsole, &csbiInfo);

	// create the densities and show their colors

	// Create tasks for the worker threads for one iteration.
	// Divide the matrix up among the threads.
	// Each thread task colors a portion of the matrix.

	// load the matrix
	loadMatrixDensity();

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

	// rotate the densities created above and show their colors
	const int rotateMatrix90CCW = 4;
	for (int iter = 0; iter < niters; ++iter) {

		// Reset to zero each iteration
		tasksdone = 0;

		// queue the tasks for the worker threads
		for (int i = 0; i < numThreads; i++) {
			enqueue(std::make_pair(rotateMatrix90CCW, i));
		}

		const int sleepms = 25;
		// Wait for the worker threads to finish their tasks
		while (tasksdone != numThreads) {
			std::this_thread::sleep_for(std::chrono::milliseconds(sleepms));
		}

		// copy the matrix densities from the buffer
		for (int row = 0; row < dim; row++) {
			for (int col = 0; col < dim; col++) {
				mat[row][col] = matbuf[row][col];
			}
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

// rotate the matrix 90 degrees CW niters times
void Matrix::handleRotateMatrix90CW(int niters) {
	/*
	 * Rotate the matrix 90 degrees CW.  Swap rows and columns using
	 * local buffer matrix.  Cannot be done in place with one matrix.
	 */
	// map matrix density to windows color attribute

	HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
	CONSOLE_SCREEN_BUFFER_INFO csbiInfo;
	// Save the current text colors.
	GetConsoleScreenBufferInfo(hConsole, &csbiInfo);

	// Create tasks for the worker threads for the desired
	// number of iterations.  Divide the matrix up among the threads.
	// Each thread task colors a portion of the matrix.

	// create the densities and show their colors

	// Create tasks for the worker threads for one iteration.
	// Divide the matrix up among the threads.
	// Each thread task colors a portion of the matrix.

	// load the matrix
	loadMatrixDensity();

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

	// rotate the densities created above and show their colors
	const int rotateMatrix90CW = 3;
	for (int iter = 0; iter < niters; ++iter) {

		// Reset to zero each iteration
		tasksdone = 0;

		// queue the tasks for the worker threads
		for (int i = 0; i < numThreads; i++) {
			enqueue(std::make_pair(rotateMatrix90CW, i));
		}

		const int sleepms = 25;
		// Wait for the worker threads to finish their tasks
		while (tasksdone != numThreads) {
			std::this_thread::sleep_for(std::chrono::milliseconds(sleepms));
		}

		// copy the matrix densities from the buffer
		for (int row = 0; row < dim; row++) {
			for (int col = 0; col < dim; col++) {
				mat[row][col] = matbuf[row][col];
			}
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

// rotate the matrix in a CW serpentine path
void Matrix::handleRotateMatrixSerpentineCW(int niters)
{
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

	// Set each row a different density
	for (int row = 0; row < dim; row++) {
		int density = std::rand()%maxdensity;
		for (int col = 0; col < dim; col++) {
			mat[row][col] = density;
		}
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
	const int rotateMatrixSerpentineCW = 5;
	for (int iter = 0; iter < niters; ++iter) {

		// Reset to zero each iteration
		tasksdone = 0;

		// Save the previous iteration's row ends
		prevRow[0] = mat[dim-1][dim-1];
		for (int row = 1; row < dim; row++) {
			prevRow[row] = mat[row-1][dim-1];
		}

		// queue the tasks for the worker threads
		for (int i = 0; i < numThreads; i++) {
			enqueue(std::make_pair(rotateMatrixSerpentineCW, i));
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

void Matrix::handleRotateMatrixSerpentineCCW(int niters)
{
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

	// Set each row a different density
	for (int row = 0; row < dim; row++) {
		int density = std::rand()%maxdensity;
		for (int col = 0; col < dim; col++) {
			mat[row][col] = density;
		}
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
	const int rotateMatrixSerpentineCCW = 6;
	for (int iter = 0; iter < niters; ++iter) {

		// Reset to zero each iteration
		tasksdone = 0;

		// Save the previous iteration's row starts
		prevRow[dim-1] = mat[0][0];
		for (int row = dim-1; row > 0; row--) {
			prevRow[row-1] = mat[row][0];
		}

		// queue the tasks for the worker threads
		for (int i = 0; i < numThreads; i++) {
			enqueue(std::make_pair(rotateMatrixSerpentineCCW, i));
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


// rotate the matrix rows down
void Matrix::handleRotateMatrixRowDown(int niters)
{
	// map matrix density to windows color attribute

	HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
	CONSOLE_SCREEN_BUFFER_INFO csbiInfo;
	// Save the current text colors.
	GetConsoleScreenBufferInfo(hConsole, &csbiInfo);

	// Create tasks for the worker threads for the desired
	// number of iterations.  Divide the matrix up among the threads.
	// Each thread task colors a portion of the matrix.

	// clear the matrix
	for (auto &vec : mat) {
		for (auto &dens : vec) {
			dens = 0;
		}
	}

	// Create 2-D geometric figures and insert into the matrix
	genGeometricFigs();

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
	std::cout << "----------------------------------------------------------------------------------------------------\n";

	// rotate the densities created above and show their colors
	const int rotateMatrixRowDown = 7;
	for (int iter = 0; iter < niters; ++iter) {

		// Reset to zero each iteration
		tasksdone = 0;

		// Save the previous iteration's column ends (last row)
		for (int col = 0; col < dim; col++) {
			prevRow[col] = mat[dim-1][col];
		}

		// queue the tasks for the worker threads
		for (int i = 0; i < numThreads; i++) {
			enqueue(std::make_pair(rotateMatrixRowDown, i));
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
		std::cout << "----------------------------------------------------------------------------------------------------\n";
	}

	// clear the matrix
	for (auto &vec : mat) {
		for (auto &dens : vec) {
			dens = 0;
		}
	}
}

// rotate the matrix rows up
void Matrix::handleRotateMatrixRowUp(int niters)
{
	// map matrix density to windows color attribute

	HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
	CONSOLE_SCREEN_BUFFER_INFO csbiInfo;
	// Save the current text colors.
	GetConsoleScreenBufferInfo(hConsole, &csbiInfo);

	// Create tasks for the worker threads for the desired
	// number of iterations.  Divide the matrix up among the threads.
	// Each thread task colors a portion of the matrix.

	// clear the matrix
	for (auto &vec : mat) {
		for (auto &dens : vec) {
			dens = 0;
		}
	}

	// Create 2-D geometric figures and insert into the matrix
	genGeometricFigs();

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
	std::cout << "----------------------------------------------------------------------------------------------------\n";


	// rotate the densities created above and show their colors
	const int rotateMatrixRowUp = 8;
	for (int iter = 0; iter < niters; ++iter) {

		// Reset to zero each iteration
		tasksdone = 0;

		// Save the previous iteration's column starts (first row)
		for (int col = 0; col < dim; col++) {
			prevRow[col] = mat[0][col];
		}

		// queue the tasks for the worker threads
		for (int i = 0; i < numThreads; i++) {
			enqueue(std::make_pair(rotateMatrixRowUp, i));
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
		std::cout << "----------------------------------------------------------------------------------------------------\n";
	}

	// clear the matrix
	for (auto &vec : mat) {
		for (auto &dens : vec) {
			dens = 0;
		}
	}
}

// rotate the matrix columns left
void Matrix::handleRotateMatrixColumnLeft(int niters)
{
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

	// Set each col a different density
	for (int col = 0; col < dim; col++) {
		int density = std::rand()%maxdensity;
		for (int row = 0; row < dim; row++) {
			mat[row][col] = density;
		}
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
	const int rotateMatrixColLeft = 9;
	for (int iter = 0; iter < niters; ++iter) {

		// Reset to zero each iteration
		tasksdone = 0;

		// Save the previous iteration's row starts (first column)
		for (int row = 0; row < dim; row++) {
			prevRow[row] = mat[row][0];
		}

		// queue the tasks for the worker threads
		for (int i = 0; i < numThreads; i++) {
			enqueue(std::make_pair(rotateMatrixColLeft, i));
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

// rotate the matrix columns right
void Matrix::handleRotateMatrixColumnRight(int niters)
{
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

	// Set each col a different density
	for (int col = 0; col < dim; col++) {
		int density = std::rand()%maxdensity;
		for (int row = 0; row < dim; row++) {
			mat[row][col] = density;
		}
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
	const int rotateMatrixColRight = 10;
	for (int iter = 0; iter < niters; ++iter) {

		// Reset to zero each iteration
		tasksdone = 0;

		// Save the previous iteration's row ends (last column)
		for (int row = 0; row < dim; row++) {
			prevRow[row] = mat[row][dim-1];
		}

		// queue the tasks for the worker threads
		for (int i = 0; i < numThreads; i++) {
			enqueue(std::make_pair(rotateMatrixColRight, i));
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

// Composite matrix rotation consisting of 1-3 rotation types with separate iteration counts
void Matrix::handleRotateMatrixComposite(const std::vector<int>& nrots, const std::vector<int>& rottype)
{
	// Note that the main menu has a 0 entry not covered here which causes the offset
/*
 	    &Matrix::colorMatrix,
		&Matrix::rotateMatrixSpiralCW,
		&Matrix::rotateMatrixSpiralCCW,
		&Matrix::rotateMatrix90CW,
		&Matrix::rotateMatrix90CCW,
		&Matrix::rotateMatrixSerpentineCW,
		&Matrix::rotateMatrixSerpentineCCW,
		&Matrix::rotateMatrixRowDown,
		&Matrix::rotateMatrixRowUp,
		&Matrix::rotateMatrixColumnLeft,
		&Matrix::rotateMatrixColumnRight,
 */

	HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
	CONSOLE_SCREEN_BUFFER_INFO csbiInfo;
	// Save the current text colors.
	GetConsoleScreenBufferInfo(hConsole, &csbiInfo);

	// clear the matrix
	for (auto &vec : mat) {
		for (auto &dens : vec) {
			dens = 0;
		}
	}

	// Create 2-D solid multi-colored square figures and insert into the matrix
	genGeometricFigs2();

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
	std::cout << "----------------------------------------------------------------------------------------------------\n";

	for (std::vector<int>::size_type i = 0; i < rottype.size(); i++) {

		for (int j = 0; j < nrots[i]; j++) {
			// Reset to zero each iteration
			tasksdone = 0;

			// Determine what to save in the prevRow based on rotation type
			switch (rottype[i]) {
			// rotateMatrixSerpentineCW
			case 6:
				// Save the previous iteration's row ends
				prevRow[0] = mat[dim-1][dim-1];
				for (int row = 1; row < dim; row++) {
					prevRow[row] = mat[row-1][dim-1];
				}
				break;
			// rotateMatrixSerpentineCCW
			case 7:
				// Save the previous iteration's row starts
				prevRow[dim-1] = mat[0][0];
				for (int row = dim-1; row > 0; row--) {
					prevRow[row-1] = mat[row][0];
				}

				break;
			// rotateMatrixRowDown
			case 8:
				// Save the previous iteration's column ends (last row)
				for (int col = 0; col < dim; col++) {
					prevRow[col] = mat[dim-1][col];
				}
				break;
			// rotateMatrixRowUp
			case 9:
				// Save the previous iteration's column starts (first row)
				for (int col = 0; col < dim; col++) {
					prevRow[col] = mat[0][col];
				}
				break;
			// rotateMatrixColumnLeft
			case 10:
				// Save the previous iteration's row starts (first column)
				for (int row = 0; row < dim; row++) {
					prevRow[row] = mat[row][0];
				}
				break;
			// rotateMatrixColumnRight
			case 11:
				// Save the previous iteration's row ends (last column)
				for (int row = 0; row < dim; row++) {
					prevRow[row] = mat[row][dim-1];
				}
				break;
			default:
				if (rottype[i] < 2 || rottype[i] > 11) {
					std::cout << "invalid rotation type: " << rottype[i] << std::endl;
				}
			}

			// queue the tasks for the worker threads
			for (int k = 0; k < numThreads; k++) {
				enqueue(std::make_pair(rottype[i]-1, k));
			}

			const int sleepms = 25;
			// Wait for the worker threads to finish their tasks
			while (tasksdone != numThreads) {
				std::this_thread::sleep_for(std::chrono::milliseconds(sleepms));
			}

			// rotateMatrix90CW, rotateMatrix90CCW
			if (rottype[i] == 4 || rottype[i] == 5) {
				// copy the matrix densities from the buffer
				for (int row = 0; row < dim; row++) {
					for (int col = 0; col < dim; col++) {
						mat[row][col] = matbuf[row][col];
					}
				}
			}

		}

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
	std::cout << "----------------------------------------------------------------------------------------------------\n";

	// clear the matrix
	for (auto &vec : mat) {
		for (auto &dens : vec) {
			dens = 0;
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

// default constructor
Matrix::Matrix()
{
	numThreads = 5;
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
		&Matrix::rotateMatrix90CW,
		&Matrix::rotateMatrix90CCW,
		&Matrix::rotateMatrixSerpentineCW,
		&Matrix::rotateMatrixSerpentineCCW,
		&Matrix::rotateMatrixRowDown,
		&Matrix::rotateMatrixRowUp,
		&Matrix::rotateMatrixColumnLeft,
		&Matrix::rotateMatrixColumnRight,
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

int main() {

	// 1. choose operation to perform on the matrix: rotation types, random colors
	// 2. choose number of iterations of the matrix operation
	// 3. if matrix is 50x50 and #workers = 5, then each worker thread processes 250 densities/colors in the matrix
	// 4. the worker threads need access to the matrix because they will be writing to it
	// 5. the task queue contains the section of the matrix to set the density/color

	const int minIters = 0;
	const int maxIters = 200;
	int matrixOp;
	int niters = 0;
	int nrottypes;
	std::vector<int> rottype;
	std::vector<int> nrots;
	std::srand (time(NULL));
	std::string result = "";
	std::cout << "Choose operation to perform on the matrix and the number of iterations\n";
	std::cout << "0:quit\n1:matrix color\n2:matrix rotate spiral CW\n3:matrix rotate spiral CCW\n";
	std::cout << "4:matrix rotate 90 degrees CW\n5:matrix rotate 90 degrees CCW\n";
	std::cout << "6:matrix rotate serpentine CW\n7:matrix rotate serpentine CCW\n8:matrix rotate row down\n";
	std::cout << "9:matrix rotate row up\n10:matrix rotate column left\n11:matrix rotate column right\n";
	std::cout << "12:matrix rotate composite --> ";
	std::cin >> matrixOp;
	if (matrixOp != 12) {
		std::cout << "\nEnter number of iterations (0-200) --> ";
		std::cin >> niters;
	} else {
		std::cout << "Enter number of rotation types -> ";
		int tp;
		int rts;
		do {
			std::cin >> nrottypes;
		} while (nrottypes < 1);
		for (int ntyp = 0; ntyp < nrottypes; ntyp++) {
			std::cout << "Enter one rotation type (not 1:matrix color) -> ";
			std::cin >> tp;
			if (tp != 1) {
				rottype.push_back(tp);
				std::cout << "Enter number of rotations for this type (1-200) -> ";
				std::cin >> rts;
				nrots.push_back(rts);
				niters+=rts;
			}
		}
	}

	std::cout << "You entered " << matrixOp << " " << niters << std::endl;

	if ((matrixOp < 0) || (matrixOp > 12)) {
		result += "matrix operation not in range 0-12 ";
	}
	if ((niters < minIters) || (niters > maxIters)) {
		result += "iterations not in range 0-200\n";
	}
	while (result.size() != 0) {
		std::cout << "Problems: " << result << std::endl;

		std::cout << "Enter matrix operation --> ";
		std::cin >> matrixOp;

		if (matrixOp != 12) {
			std::cout << "\nEnter number of iterations (0-200) --> ";
			std::cin >> niters;
		} else {
			niters = 0;
			rottype.clear();
			nrots.clear();
			std::cout << "Enter number of rotation types -> ";
			int tp;
			int rts;
			do {
				std::cin >> nrottypes;
			} while (nrottypes < 1);
			for (int ntyp = 0; ntyp < nrottypes; ntyp++) {
				std::cout << "Enter one rotation type (not 1:matrix color) -> ";
				std::cin >> tp;
				if (tp != 1) {
					rottype.push_back(tp);
					std::cout << "Enter number of rotations for this type (1-200) -> ";
					std::cin >> rts;
					nrots.push_back(rts);
					niters+=rts;
				}
			}
		}

		std::cout << "You entered " << matrixOp << " " << niters << std::endl;

		result.clear();
		if ((matrixOp < 0) || (matrixOp > 12)) {
			result += "matrix operation not in range 0-12 ";
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
	Matrix matrx(numthreads); // @suppress("Ambiguous problem")

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
			matrx.handleRotateMatrix90CW(niters);
			break;
		case 5:
			matrx.handleRotateMatrix90CCW(niters);
			break;
		case 6:
			matrx.handleRotateMatrixSerpentineCW(niters);
			break;
		case 7:
			matrx.handleRotateMatrixSerpentineCCW(niters);
			break;
		case 8:
			matrx.handleRotateMatrixRowDown(niters);
			break;
		case 9:
			matrx.handleRotateMatrixRowUp(niters);
			break;
		case 10:
			matrx.handleRotateMatrixColumnLeft(niters);
			break;
		case 11:
			matrx.handleRotateMatrixColumnRight(niters);
			break;
		case 12:
			matrx.handleRotateMatrixComposite(nrots, rottype);
			break;
		default:
			std::cout << "matrix operation " << matrixOp << " not valid\n";
		}

		std::cout << "Choose operation to perform on the matrix (0-12) --> ";
		std::cin >> matrixOp;
		if (matrixOp != 12) {
			std::cout << "\nEnter number of iterations (0-200) --> ";
			std::cin >> niters;
		} else {
			niters = 0;
			rottype.clear();
			nrots.clear();
			std::cout << "Enter number of rotation types -> ";
			int tp;
			int rts;
			do {
				std::cin >> nrottypes;
			} while (nrottypes < 1);
			for (int ntyp = 0; ntyp < nrottypes; ntyp++) {
				std::cout << "Enter one rotation type (not 1:matrix color) -> ";
				std::cin >> tp;
				if (tp != 1) {
					rottype.push_back(tp);
					std::cout << "Enter number of rotations for this type (1-200) -> ";
					std::cin >> rts;
					nrots.push_back(rts);
					niters+=rts;
				}
			}
		}

		std::cout << "You entered " << matrixOp << " " << niters << std::endl;

		result.clear();
		if ((matrixOp < 0) || (matrixOp >12)) {
			result += "matrix operation not in range 0-12 ";
		}
		if ((niters < minIters) || (niters > maxIters)) {
			result += "iterations not in range 0-200\n";
		}
		while (result.size() != 0) {
			std::cout << "Problems: " << result << std::endl;

			std::cout << "Enter matrix operation --> ";
			std::cin >> matrixOp;

			if (matrixOp != 12) {
				std::cout << "\nEnter number of iterations (0-200) --> ";
				std::cin >> niters;
			} else {
				niters = 0;
				rottype.clear();
				nrots.clear();
				std::cout << "Enter number of rotation types -> ";
				int tp;
				int rts;
				do {
					std::cin >> nrottypes;
				} while (nrottypes < 1);
				for (int ntyp = 0; ntyp < nrottypes; ntyp++) {
					std::cout << "Enter one rotation type (not 1:matrix color) -> ";
					std::cin >> tp;
					if (tp != 1) {
						rottype.push_back(tp);
						std::cout << "Enter number of rotations for this type (1-200) -> ";
						std::cin >> rts;
						nrots.push_back(rts);
						niters+=rts;
					}
				}
			}

			std::cout << "You entered " << matrixOp << " " << niters << std::endl;

			result.clear();
			if ((matrixOp < 0) || (matrixOp > 12)) {
				result += "matrix operation not in range 0-12 ";
			}
			if ((niters < minIters) || (niters > maxIters)) {
				result += "iterations not in range 0-200\n";
			}
		}
	}

    return 0;
}
