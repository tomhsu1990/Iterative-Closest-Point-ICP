# Iterative Closest Point (ICP)

The main concept of 2D ICP is to find closest points. In addition, the key is after determining the closest points' coorespondence, how do we calculate the rotation.

http://ivory-cavern.blogspot.com/2009/11/icp-iterative-closest-point-c.html

## Algorithm

https://www.cs.umd.edu/~mount/ANN/

## Discussion

## How to run (MacOS)

mkdir build

cd build

cmake ..

make

1) ./ICP
(The default img is ../img/curve.png.)

2) ./ICP ../img/corner.png
(Change img.)

3) ./ICP ../img/corner.png 1000
(Uniformly add 1000 noise points to the target points set.)

4) ./ICP ../img/corner.png 10 2000
(Uniformly add 10 noise points and remove 2000 points from the target points set.)

## Question

1. How do we speed up the performance by doing down-sampling?

2. 
