# doudizhuAI
 This is my participation in the computer game competition landlord ai project code
The code is mainly written in C++:

Summary:

1. In the process of the game, there is a situation of random cards. When there are more feasible solutions (such as getting the landlord and playing for the first time), when the Monte Carlo algorithm is executed, the generated node tree is too Deep, leading to a poor carding plan, so you need to deal with the depth of the tree when it is too deep. In addition, although the probability of occurrence of the timeout problem has decreased, it has not appeared in the game, but it needs to be improved.
2. In the model part of the guessing opponent, the improved probability table is not well matched with other modules. At that time, the probability table was abandoned, so the implementation of the probability table needs to be considered.
3. In the module for teammates, there is still a need for improvement. In the course of the game, there is no cooperation with teammates, resulting in the loss of the card.
