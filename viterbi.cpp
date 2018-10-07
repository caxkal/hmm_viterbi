
#include "viterbi.h"

#include <string>
#include <algorithm>

void
Viterbi::calculate_viterbi( const std::vector< Observation >& observations,
                            const std::vector< State >& hidden_states,
                            const std::vector< StateVector >& transaction_probabilities,
                            const std::vector< StateVector >& emission_probabilities,
                            const std::vector< double >& start_probabilities )
{
    auto obs_size = observations.size( );
    auto state_size = hidden_states.size( );

    // local probability
    std::vector< std::vector< double > > delta_probability( obs_size, std::vector< double >( state_size, 0 ) );

    // argmax index
    std::vector< std::vector< std::size_t > > indexes( obs_size, std::vector< std::size_t >( state_size ) );

    m_most_probable_states.resize( obs_size );

    for ( std::size_t i = 0; i != state_size; ++i )
    {
        auto ob_id = observations[ 0 ].id;
        auto state = emission_probabilities[ ob_id ];
        delta_probability[ 0 ][ i ] = start_probabilities[ i ] * state[ i ];
        indexes[ 0 ][ i ] = 0;
    }

    for ( std::size_t j = 1; j != obs_size; ++j )
    {
        for (std::size_t k = 0; k != state_size; ++k )
        {

            double emission_value = emission_probabilities[ observations[ j ].id ][ k ];
            find_max_value_index( delta_probability[ j - 1 ],
                                  transaction_probabilities[ k ],
                                  emission_value,
                                  delta_probability[ j ][ k ],
                                  indexes[ j ][ k ]);
        }
    }

    auto it = std::max_element( delta_probability[ obs_size - 1 ].begin(), delta_probability[ obs_size - 1 ].end());
    m_most_probable_states[ obs_size - 1 ] = std::distance( delta_probability[ obs_size - 1 ].begin( ), it );

    for ( int t = obs_size - 2; t != -1; --t )
    {
        m_most_probable_states[ t ] = indexes[ t ][ m_most_probable_states[ t + 1 ] ];
    }
}

void
Viterbi::find_max_value_index( const std::vector< double >& temp_matrix, const StateVector& transition_vector,
                               double emission_element,
                               double &delta_val, std::size_t &psi_idx)
{
    if ( transition_vector.empty( ) )
        return;

    double tmp = 0.0;
    std::size_t max_index = 0;
    double max_val = temp_matrix[ 0 ] * transition_vector[ 0 ] * emission_element;

    for (std::size_t i = 1; i != transition_vector.size( ); ++i )
    {
        tmp = temp_matrix[i] * transition_vector[i] * emission_element;
        if ( max_val < tmp)
        {
            max_val = tmp;
            max_index = i;
        }
    }

    delta_val = max_val;
    psi_idx = max_index;
}

std::vector< StateID >
Viterbi::get_result( ) const
{
    return m_most_probable_states;
}
