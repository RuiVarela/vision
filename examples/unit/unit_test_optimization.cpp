#include "../../source/vs.hpp"

static void test_max_cost()
{

    // http://dlib.net/max_cost_assignment.py.html

    // this simple example shows how to call dlib's optimal linear assignment
    // problem solver.  It is an implementation of the famous Hungarian algorithm
    // and is quite fast, operating in O(N^3) time.

    // Let's imagine you need to assign N people to N jobs.  Additionally, each
    // person will make your company a certain amount of money at each job, but each
    // person has different skills so they are better at some jobs and worse at
    // others.  You would like to find the best way to assign people to these jobs.
    // In particular, you would like to maximize the amount of money the group makes
    // as a whole.  This is an example of an assignment problem and is what is solved
    // by the dlib.max_cost_assignment() routine.

    // So in this example, let's imagine we have 3 people and 3 jobs. We represent
    // the amount of money each person will produce at each job with a cost matrix.
    // Each row corresponds to a person and each column corresponds to a job. So for
    // example, below we are saying that person 0 will make $1 at job 0, $2 at job 1,
    // and $6 at job 2.
    // cost = dlib.matrix([[1, 2, 6],
    //                     [5, 3, 6],
    //                     [4, 5, 0]])
    vs::CostMatrix cost(3, 3);
    cost(0, 0) = 1;
    cost(0, 1) = 2;
    cost(0, 2) = 6;

    cost(1, 0) = 5;
    cost(1, 1) = 3;
    cost(1, 2) = 6;

    cost(2, 0) = 4;
    cost(2, 1) = 5;
    cost(2, 2) = 0;

    // To find out the best assignment of people to jobs we just need to call this
    // function.
    vs::Assignment assignment = vs::assignmentMaxCost(cost);

    // This prints optimal assignments:  [2, 0, 1]
    // which indicates that we should assign the person from the first row of the
    // cost matrix to job 2, the middle row person to job 0, and the bottom row
    // person to job 1.
    UTEST(assignment[0] == 2);
    UTEST(assignment[1] == 0);
    UTEST(assignment[2] == 1);

    // This prints optimal cost:  16.0
    // which is correct since our optimal assignment is 6+5+5.
    UTEST(vs::assignmentCost(cost, assignment) == 16);
}

int unit_tests_optimization(int argc, char **argv)
{
    test_max_cost();
    return 0;
}
