<h3> 
Matrix Animation using Threadpools in C++11
</h3>
<p>
This program is written in C++11 and uses Eclipse CDE 2025-06 IDE with GCC compiler and MINGW linker. 
The program takes advantage of the C++ Multithread concurrency library. It creates five worker threads
which run continuously for the life of the program.  Work tasks are pushed to a FIFO queue by the handler thread.
The tasks are matrix maniuplation methods which color or rotate the matrix contents in various ways.
The matrix is 50 x 50 of type int.
The worker threads calls front/pop the tasks off the queue and perform the processing of the matrix concurrently
with each other.  Each thread reads and writes to a different part of the matrix so there is no race
condition.  The key analysis is determining how to process the matrix so that each thread does not write to
a part of the matrix that another thread is reading or writing to.  An atomic variable <i>workdone</i> is 
used to signal the handler thread when all the worker threads have completed
their individual processing of the matrix.  The worker threads increment the <i>workdone</i> atomic variable counter.
Since the variable is an atomic_short, there is also no data race.  The worker threads access the task queue using a
unique_lock on a mutex and a condition variable to wait until the task queue is not empty.  When signaled by the 
handler thread with notify_one, they pop the first task in the queue, unlock the mutex, and execute the task with any supplied parameters.
They will stop running and exit if the stop flag is set by the handler. This will occur when the program terminates.  The program
terminates when the user enters 0 as described below.

</p>

<p>
<h4>Here is the list of coloring/rotation operations that can be performed on the matrix:</h4>
	<ol start="0">
		<li>quit</li>
		<li>matrix color</li>
		<li>matrix rotate spiral CW</li>
		<li>matrix rotate spiral CCW</li>
		<li>matrix rotate 90 degrees CW</li>
		<li>matrix rotate 90 degrees CCW</li>
		<li>matrix rotate serpentine CW</li>
		<li>matrix rotate serpentine CCW</li>
		<li>matrix rotate row down</li>
		<li>matrix rotate row up</li>
		<li>matrix rotate column left</li>
		<li>matrix rotate column right</li>
	</ol>
</p>

<p>
<h4>Here are brief descriptions of what each operation listed above does.</h4>
<br />
<b>Matrix Color</b> colors the matrix elements to a different color each iteration.
<br />
<b>Matrix Rotate Spiral CW/CCW</b> rotates the matrix such that each row and column of the 
 submatrices are rotated together in spiral fashion. Starting at the outermost matrix, each
 element is moved CW or CCW one position along the perimeter of the submatrix.  Then the next
 inner matrix has its elements along the perimeter of the submatrix moved one position.  
 This continues until a 2 x 2 matrix has its elements moved one position.  A 1x1 matrix of course
 has nothing to move.  You will rotate the matrix approximately 90/50 degrees each iteration.
 Iterating this task 50 times is equivalent to performing Matrix Rotate 90 degrees with one iteration.
 <br />
 <b>Matrix Rotate 90 Degrees CW/CCW</b> rotates the matrix 90 degrees each iteration.  This operation
 is in essence 50 iterations of the above operation since the matrix is 50x50.  The above operation moves
 each element one position.  You would need to do that 50 times in order to rotate the matrix 90 degrees.
 <br />
 <b>Matrix Rotate Serpentine CW/CCW</b> rotates each row and feeds the output to the next row.  Each element
 in a row is moved one position.  The last/first element in a row is the first/last element in the next row.
 The last/first element in the last row becomes the first/last element in the first row.
 <br />
 <b>Matrix Rotate Row Up/Down</b> rotates the matrix rows up or down.  The top or bottom row of the matrix
 is fed to the bottom or top row of the matrix.  The rows are wrapped around.
 <br />
 <b>Matrix Rotate Column Left/Right</b> rotates the columns of the matrix left or right.
 The columns are wrapped around.
</p>

<h4>Matrix color</h4>
C:\Users\Thoma\eclipse-workspace\ThreadPoolAnimation\Debug>ThreadPoolAnimation.exe
<pre>
Choose operation to perform on the matrix and the number of iterations
0:quit
1:matrix color
2:matrix rotate spiral CW
3:matrix rotate spiral CCW
4:matrix rotate 90 degrees CW
5:matrix rotate 90 degrees CCW
6:matrix rotate serpentine CW
7:matrix rotate serpentine CCW
8:matrix rotate row down
9:matrix rotate row up
10:matrix rotate column left
11:matrix rotate column right --> 1

Enter number of iterations (0-200) --> 1
You entered 1 1
hardware concurrency = 12
</pre>
<img width="907" height="920" alt="image" src="https://github.com/user-attachments/assets/59d8b7eb-1f38-4892-83bf-ff9e37efdcde" />

<h4>Matrix Rotate Spiral CW</h4>
<img width="935" height="959" alt="image" src="https://github.com/user-attachments/assets/a86e30db-6d5d-40bb-85de-47c1c924016b" />
<img width="997" height="955" alt="image" src="https://github.com/user-attachments/assets/066976fb-f3e8-45fd-86ce-46de685dfcec" />
<h4>Matrix Rotate Serpentine CCW</h4>
<img width="935" height="896" alt="image" src="https://github.com/user-attachments/assets/4990924d-d14d-4841-8be0-d5c59354b6be" />
<img width="927" height="954" alt="image" src="https://github.com/user-attachments/assets/bbba5bdf-936a-495e-be89-e5aeba6505f0" />
<h4>Matrix Rotate 90 degrees CW
<img width="970" height="992" alt="image" src="https://github.com/user-attachments/assets/d520e77b-8f29-4d09-9096-d734fd4e5e63" />
<img width="959" height="995" alt="image" src="https://github.com/user-attachments/assets/c498ef82-7af2-4449-ab4d-cb32a8831148" />
<h4>Matrix Rotate Row Down</h4>
<img width="941" height="997" alt="image" src="https://github.com/user-attachments/assets/b44b29c4-9954-479c-a9a3-10df13e683e5" />
<img width="937" height="963" alt="image" src="https://github.com/user-attachments/assets/7845cc87-6c22-4a61-b920-fe2ae8b7593b" />

