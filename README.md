# Iterative Closest Point (ICP)

The main concept of 2D ICP is to find the closest point for each source point. In addition, the key is that after determining the closest points' coorespondences, how do we calculate the rotation. Here is the big picture and peusdo code.

We have two points set: source points set and closest points set.

After centering at each mean, every point lives in its own coordinate.

Suppose the rotation we want to compute is r.

Let r = r_closest - r_src.

r_closest is the shifted closest points' coordinate relative to the global coordinate.

r_src is the shifted source points' coordinate relative to the global coordinate.

The formula below is an approximation to r.

r = atan2(y'/x') - atan2(y/x), (x, y) is the source coordinate and (x', y') is the closest points' coordinate.

app(r) = (atan2(y'/x') - atan2(y/x))/(1+yy'/xx')

app(r) is proportional to r.

## Algorithm

[Approximate Nearest Neighbors](https://www.cs.umd.edu/~mount/ANN/)

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

2. Is there another way to solve local minima problem?

## Reference

[ICP（Iterative Closest Point）演算法實作 (C 語言)](http://ivory-cavern.blogspot.com/2009/11/icp-iterative-closest-point-c.html)
