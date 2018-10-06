#include <iostream>

#include "hmm.h"
#include "map.h"

int main()
{
    HMM model;

    model.calculate_viterbi( );

    model.print_results( );
}

