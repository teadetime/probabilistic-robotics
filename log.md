# Log of work done
#### 2/9/22
Didn't spend enough time in the last week working on this, will be adding another block to my calendar!
Also need to sync with Paul but should have more to show!

Note: Should look for video about complex conditional probability, conditional independence vs absolute independence. Order of operation for probabilities

**State**
- Dynamic or static, changes with time or no
- Inlcudes pose, state of surroundings and robot, as comple as you want to make
- ALso need landmarks! somethign that doesn't change

**Env Interaction**
- Measurement, note these are delayed!
- Control, actuation of some sort
- Robot is assumed to do both of these at any point
- Collectively called data and the robot could look at both of these for decisions

- Measurement Data is laser, camera scan ie denoted by Z_t generally igonre timing delays
- Control Data, Ie the velocity of the robot. Measured via time steps ie we told the robot to move 10cm/s or also the use of odometry

If our state is complete then interestingly knowing our control data allows us to perfectly predict the next state -- not true in practice

**State transition Probability**

p(x_t | x_t−1 , u_t )

**Measurement Probaility**
p(z t | x t )

Measurements are noisy projections of state

Together these describe the dynamic stochastic system of the robot and its environment.

**Belief Distributions**

Belief != state

bel(x_t ) = p(x_t | z_1:t , u_1:t )

!bel(x t )  = p(x t | z 1:t−1 , u 1:t ) 

Calculate before incorporating Z_t

This is often called the prediction, this is the prediction of X_t based on the previous state posterior before current measurements. calculating bel from !bel is called **correction update**

**Bayes Filters**
- used for calculating beliefs, very general
- Damn this is confusing -- Why do we integrate? Maybe I can find a video
- The integral and multiplications require finite state spaces or closed  form solutions


#### 2/1/22 3 hrs

- Read Chapter 1 in probabilistic robotics. Plan of action is to understand math in chapters 2-4 unless that is painfully boring.
- They recommend math and then a particle filter. I've already played with a particel filter so maybe it will be good to look at all the math.


Questions:
- Discrete Bayes Filter -- What's https://www.youtube.com/watch?v=k6Dw0on6NtM


Goals:
- Work through Chapter 2 by next week: Recursive State estimation


RECURSIVE STATE ESTIMATION

- Probabilities of a random variable that are discrete sum to 1! adn are always positive
- - p(X = x)
- SIGMA_x p(X = x) = 1

Continuous Probability Density Functions(PDFS)
- ie 1-d normal distribution with mean and variance
- This PDF is p(x) = (2pistd^2)^-.5 exp(-1/2 ((x-mu)^2)/(std^2))
- Case can be made vectorized which involves the determinate and a covariance matrix

Joint distribution (probability that both happen)
- If p(x) is independent from p(y) then p(x,y) = p(x)\*p(y) 
- If Dependent than p(x|y) probability x given y

Conditional Probability (if y is known)
- p(x|y) = p(x,y) / p(y)
