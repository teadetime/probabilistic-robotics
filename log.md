# Log of work done
#### 2/1/22

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
