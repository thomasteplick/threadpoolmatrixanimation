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

    // Queue of tasks
    std::queue<int> tasks;

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
    void rotateMatrixSpiralCW();
    void rotateMatrixSpiralCCW();
    void rotateMatrixSerpentineCW();
    void rotateMatrixSerpentineCCW();
    void rotateMatrixRowDown();
    void rotateMatrixColumnDown();
    void rotateMatrixRowUp();
    void rotateMatrixColumnUp();
public:
    Matrix(int numTasks = std::thread::hardware_concurrency());
    ~Matrix();

    // Enqueue task for execution by the thread pool
    void enqueue(int subMat);

    // iterate over queueing tasks to color the matrix
    void handleColorMatrix(int niters);

    // iterate over queueing tasks to rotate the matrix colors
    void handleRotateMatrix();

    // no copying or moving or assigning
    Matrix(const Matrix &mat) = delete;
    Matrix& operator=(const Matrix& mat) = delete;
    Matrix(Matrix &&mat) = delete;
    Matrix& operator=(Matrix &&mat) = delete;
};



#endif /* ANIMATEMATRIX_H_ */
