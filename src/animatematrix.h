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
#include <condition_variable>

// matrix class to process
class Matrix {
	// matrix dimension dim x dim, rows x columns
	static const int dim = 50;

	int numThreads;

	// the matrix that will be animated
    int mat[dim][dim];

    // Vector to store worker threads
    std::vector<std::thread> threads;

    // Queue of tasks, first=member function, second=submatrix
    std::queue<std::pair<int,int>> tasks;

    // Mutex to synchronize access to shared data
    std::mutex queue_mutex;

    // Condition variable to signal changes in the state of
    // the tasks queue
    std::condition_variable cv;

    // Flag to indicate whether the thread pool should stop
    // or not
    bool stop = false;

    // wait for tasks until told to terminate
    void runWorkerTask();

    // animate functions for the matrix
    void colorMatrix(int submatrix);
    void rotateMatrixSpiralCW(int submatrix);
    void rotateMatrixSpiralCCW(int submatrix);
    void rotateMatrixSerpentineCW(int submatrix);
    void rotateMatrixSerpentineCCW(int submatrix);
    void rotateMatrixRowDown(int submatrix);
    void rotateMatrixRowUp(int submatrix);
    void rotateMatrixColumnDown(int submatrix);
    void rotateMatrixColumnUp(int submatrix);
    void rotateMatrix90CW(int submatrix);
    void rotateMatrix90CCW(int submatrix);

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

    // no copying or moving or assigning
    Matrix(const Matrix &mat) = delete;
    Matrix& operator=(const Matrix& mat) = delete;
    Matrix(Matrix &&mat) = delete;
    Matrix& operator=(Matrix &&mat) = delete;
};



#endif /* ANIMATEMATRIX_H_ */
