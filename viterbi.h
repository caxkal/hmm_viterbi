#pragma once

#include <iostream>
#include <vector>

#include "hmm.h"

class Viterbi {
public:
    Viterbi( std::vector< Observation > observations,
             std::vector< State > hidden_states,
             std::vector< StateVector > transaction_probabilities,
             std::vector< StateVector > emission_probabilities,
             std::vector< double > start_probabilities )
        : m_observations( observations )
        , m_hidden_states( hidden_states )
        , m_transaction_probabilities( transaction_probabilities )
        , m_emission_probabilities( emission_probabilities )
        , m_start_probabilities( start_probabilities )
    { }

    void calculate_viterbi( );
    std::vector< StateID > get_result( ) const;

private:
    void find_max_value_index( const std::vector< double >&,
                               const StateVector&, double, double&, std::size_t& );

public:
    std::vector< Observation > m_observations;
    std::vector< State > m_hidden_states;
    std::vector< StateVector > m_transaction_probabilities;
    std::vector< StateVector > m_emission_probabilities;
    std::vector< double > m_start_probabilities;

     std::vector< StateID > m_most_probable_states;
};

