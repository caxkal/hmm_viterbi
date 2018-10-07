#pragma once

#include <iostream>
#include <vector>

#include "hmm.h"

class Viterbi
{
public:
    Viterbi( ) = default;

    void calculate_viterbi( const std::vector< Observation >& observations,
                            const std::vector< State >& hidden_states,
                            const std::vector< StateVector >& transaction_probabilities,
                            const std::vector< StateVector >& emission_probabilities,
                            const std::vector< double >& start_probabilities );
    std::vector< StateID > get_result( ) const;

private:
    void find_max_value_index( const std::vector< double >&,
                               const StateVector&, double, double&, std::size_t& );

public:
     std::vector< StateID > m_most_probable_states;
};

