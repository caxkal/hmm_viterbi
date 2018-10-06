
#include "viterbi.h"

#include <string>
#include <algorithm>

void
Viterbi::calculate_viterbi()
{
    // local probability
    std::vector< std::vector< double > > delta_probability;

    // argmax index
    std::vector< std::vector< std::size_t > > psi;

    auto obs_size = m_observations.size( );
    auto state_size = m_hidden_states.size( );

    delta_probability.resize( obs_size );
    for ( std::size_t i = 0; i != obs_size; ++i )
    {
        delta_probability[i].resize( state_size );
    }

    psi.resize( obs_size );
    for ( std::size_t i = 0; i != obs_size; ++i )
    {
        psi[i].resize( state_size );
    }

    m_most_probable_states.resize( obs_size );

    for ( std::size_t i = 0; i != state_size; ++i )
    {
        auto ob_id = m_observations[ 0 ].id;
        auto state = m_emission_probabilities[ ob_id ];
        delta_probability[ 0 ][ i ] = m_start_probabilities[ i ] * state[ i ];
        psi[0][ i ] = 0;
    }

    for ( std::size_t j = 1; j != obs_size; ++j ) {
        for (std::size_t k = 0; k != state_size; ++k ) {

            double d = m_emission_probabilities[ m_observations[ j ].id ][ k ];
            find_max_value_index( delta_probability[ j - 1 ],
                                  m_transaction_probabilities[ k ],
                                  d,
                                  delta_probability[ j ][ k ],
                                  psi[ j ][ k ]);
        }
    }

    auto it = std::max_element( delta_probability[obs_size - 1].begin(), delta_probability[obs_size - 1].end());
    m_most_probable_states[ obs_size - 1 ] = std::distance( delta_probability[obs_size - 1].begin( ), it );

    for ( std::size_t t = obs_size - 2; t != -1; --t )
    {
        m_most_probable_states[ t ] = psi[ t ][ m_most_probable_states[ t + 1 ] ];
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
    double max_val = temp_matrix[0] * transition_vector[0] * emission_element;

    for (std::size_t i = 1; i != m_hidden_states.size( ); ++i )
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
