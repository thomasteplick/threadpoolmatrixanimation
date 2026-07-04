/*
 * animatematrix.h
 *
 *  Created on: Jun 19, 2026
 *      Author: Thomas Teplick
 */

#ifndef ANIMATEMATRIX_H_
#define ANIMATEMATRIX_H_

#include <vector>
#include <thread>
#include <queue>
#include <utility>
#include <atomic>
#include <condition_variable>

using mat90deg = std::vector<std::vector<int>>;

// matrix class to process
class Matrix {

	// matrix dimension dim x dim, rows x columns
	static const int dim = 50;
	static const std::string namedensities; // matrix density, 50x50
	static const std::string dataDir;  // directory for matrix densities

	int numThreads;

	// the matrix that will be animated
    int mat[dim][dim];
    int dblBuf[2][dim][dim];

    // Last row element in mat from previous iteration
    int prevRow[dim];

    // Vector to store worker threads
    std::vector<std::thread> threads;

    // Queue of tasks, first=member function, second=submatrix
    std::queue<std::pair<int,int>> tasks;

    // Mutex to synchronize access to shared data
    std::mutex queue_mutex;

    // Condition variable to signal changes in the state of
    // the tasks queue
    std::condition_variable cv;

    // number of tasks done, protected by cv_taskdone
    std::atomic<short> tasksdone;

    // matrix buffer to use
    std::atomic<short> mat_buffer;

    // Flag to indicate whether the thread pool should stop
    // or not
    bool stop = false;

    // wait for tasks until told to terminate
    void runWorkerTask();

    // mutex to synchronize access to matrix
    std::mutex mat_mutex;

    // animate functions for the matrix
    void colorMatrix(int submatrix);
    void rotateMatrixSpiralCW(int submatrix);
    void rotateMatrixSpiralCCW(int submatrix);
    void rotateMatrixSerpentineCW(int submatrix);
    void rotateMatrixSerpentineCCW(int submatrix);
    void rotateMatrixRowDown(int submatrix);
    void rotateMatrixRowUp(int submatrix);
    void rotateMatrixColumnLeft(int submatrix);
    void rotateMatrixColumnRight(int submatrix);
    void rotateMatrix90CW(int submatrix);
    void rotateMatrix90CCW(int submatrix);

    // read the matrix density file
    void loadMatrixDensity();

    // generate 2-D geometric figures
    void genGeometricFigs();

	// alias for a member void function with int argument
    using matrixMbr = void(Matrix::*)(int);

    // an array of the member functions in the thread pool
    matrixMbr matfcn[];

public:

    Matrix(int numTasks = std::thread::hardware_concurrency());
    ~Matrix();

    // Enqueue task for execution by the thread pool
    void enqueue(std::pair<int,int> task);

    // iterate over queueing tasks to color the matrix
    void handleColorMatrix(int niters);

    // rotate the matrix CW in a spiral motion
    void handleRotateMatrixSpiralCW(int niters);

    // rotate the matrix CCW in a spiral motion
    void handleRotateMatrixSpiralCCW(int niters);

    // rotate the matrix CW 90 degrees
    void handleRotateMatrix90CW(int niters);

    // rotate the matrix CCW 90 degrees
    void handleRotateMatrix90CCW(int niters);

    // rotate the matrix CW in a serpentine manner
    void handleRotateMatrixSerpentineCW(int niters);

    // rotate the matrix CCW in a serpentine manner
    void handleRotateMatrixSerpentineCCW(int niters);

    // rotate the matrix rows up
    void handleRotateMatrixRowUp(int niters);

    // rotate the matrix rows down
    void handleRotateMatrixRowDown(int niters);

    // rotate the matrix columns left
    void handleRotateMatrixColumnLeft(int niters);

    // rotate the matrix columns right
    void handleRotateMatrixColumnRight(int niters);

    // no copying or moving or assigning
    Matrix(const Matrix &mat) = delete;
    Matrix& operator=(const Matrix& mat) = delete;
    Matrix(Matrix &&mat) = delete;
    Matrix& operator=(Matrix &&mat) = delete;
};



#endif /* ANIMATEMATRIX_H_ */
