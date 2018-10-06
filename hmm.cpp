#include "hmm.h"
#include "viterbi_decoder.h"

#include <iostream>
#include <algorithm>
#include <cassert>

#define MAX_DIST 3

HMM::HMM( )
{
    setup_inital_map_and_observations( );
    candidate_selection( );
    allocate_memory( );
    setup_inital_probability( );
    calculate_emission_probability( );
    calculate_transition_probability( );
}

void 
HMM::setup_inital_map_and_observations ( )
{
/*
    5   _ _ _           |
    4   |         *     |
    3 * |           _ _ |
    2   |               |    *
    1   | _ _ _ _ _ _ _ |
    0           *
      0 1 2 3 4 5 6 7 8 9 10 11

    * - observations
*/

    Link road1{ { 1, 1 }, {1, 5} };
    Link road2{ { 1, 1 }, {9, 1} };
    Link road3{ { 1, 5 }, {3, 5} };
    Link road4{ { 9, 1 }, {9, 3} };
    Link road5{ { 9, 3 }, {9, 5} };
    Link road6{ { 9, 3 }, {7, 3} };


    m_map.roads.emplace_back( road1 );
    m_map.roads.emplace_back( road2 );
    m_map.roads.emplace_back( road3 );
    m_map.roads.emplace_back( road4 );
    m_map.roads.emplace_back( road5 );
    m_map.roads.emplace_back( road6 );

    m_observations.emplace_back( m_observations.size( ), GeoCoordinates{0, 3} );
    m_observations.emplace_back( m_observations.size( ), GeoCoordinates{5, 0} );
    m_observations.emplace_back( m_observations.size( ), GeoCoordinates{11, 2} );
    m_observations.emplace_back( m_observations.size( ), GeoCoordinates{6, 4 } );
    m_observations.emplace_back( m_observations.size( ), GeoCoordinates{9, 5 } );
}

void
HMM::allocate_memory( )
{
    // Allocate memory for matrices
    m_emission_probabilities.resize( m_observations.size( ),
                                     std::vector<double>( m_hidden_states.size( ), 0) );

    m_transaction_probabilities.resize( m_hidden_states.size( ),
                                     std::vector<double>( m_hidden_states.size( ), 0) );
}

void
HMM::candidate_selection( )
{
    auto links = m_map.roads;
    for ( auto obs: m_observations ) {
        // todo: in real case we need to get link in some area instead of taking every link
        for ( const auto& link : links ) {
            auto projection = m_map.get_closest_point( link, obs.observation );
//            bool is_repeated = false;
//            for ( auto state: m_hidden_states ) {
//                if ( state.projection == projection ) {
//                    is_repeated = true;
//                    break;
//                }
//            }

//            if ( is_repeated ) {
//                continue;
//            }

            // filter out far candidates
            if ( m_map.get_distance( projection, obs.observation ) <= MAX_DIST ) {
                auto id = m_hidden_states.size( );
                m_hidden_states.emplace_back( State{ id, projection, link } );
                auto it = m_observation_states.find( obs.id );
                if ( it == m_observation_states.end( ) ) {
                     auto element = std::vector< StateID >( );
                     element.push_back( id );
                     m_observation_states[ obs.id ] = element;
                } else {
                    it->second.push_back( id );
                }
            }
        }
    }

    print_observation_states( m_observation_states );
    std::cout << " Candid: " << m_hidden_states.size( ) << std::endl;
    print_state( m_hidden_states );
}

void 
HMM::calculate_emission_probability( )
{
    //const double sigma_z = 4.07;
    for ( const auto& obs: m_observations )
    {
        auto sigma_z = calculate_sigma( obs);
        sigma_z = sigma_z == 0 ? 0.00001 : sigma_z; // assign small number in case sigma is 0
        sigma_z *= 1.4826;
        for ( auto& state: m_hidden_states ) {
           // auto projection = m_map.get_closest_point( state.link, obs.observation );

            auto dist = m_map.get_distance( obs.observation, state.projection );

            auto probability = 1. / ( sigma_z * std::sqrt( 2 * M_PI ) ) *
                                    std::exp( -0.5 * dist / sigma_z * dist / sigma_z );

            m_emission_probabilities[ obs.id ][ state.id ] = probability;
        }
    }

    print_matrix( m_emission_probabilities, "emission" );
}

double
HMM::calculate_sigma( const Observation& obs )
{
    std::vector< double > distances;
    for( auto os_id: m_observation_states[ obs.id ] ) {
        auto state = m_hidden_states[ os_id ];
        auto projection = state.projection;
        auto distance = m_map.get_distance( obs.observation, projection );
        distances.push_back( distance );
    }

    assert( !distances.empty());

    size_t n = distances.size( ) / 2;
    std::nth_element( distances.begin( ), distances.begin( ) + n, distances.end( ) );
    return distances[ n ];
}

void
HMM::calculate_transition_probability( )
{
    for( int i = 0; i < m_observations.size( ) - 1; ++i ) {
        auto current = m_observations[ i ];
        auto next = m_observations[ i + 1 ];
        auto observation_distance = m_map.get_distance( current.observation, next.observation );
        auto current_states = m_observation_states[ current.id ];
        auto next_states = m_observation_states[ next.id ];

        double betta = calculate_betta( current_states, next_states, observation_distance );

        for (auto cur_state_id : current_states ) {
            for ( auto next_state_id : next_states ) {
                double projection_distance = 0;
                bool res = get_true_distance( cur_state_id,next_state_id, projection_distance );
                if ( res ) {
                    auto probability = 1 / betta * std::exp( -std::abs( observation_distance - projection_distance ) / betta );
                    m_transaction_probabilities[ next_state_id ][ cur_state_id ] = probability;
                }
            }
        }
    }

    print_matrix( m_transaction_probabilities, "transition" );
}

double
HMM::calculate_betta( const std::vector< StateID >& current_states, const std::vector< StateID >& next_states,
                      double observation_distance )
{
    std::vector< double > distances;
    for (auto cur_state_id : current_states ) {
        for ( auto next_state_id : next_states ) {
            double projection_distance = 0;
            bool res = get_true_distance( cur_state_id,next_state_id, projection_distance );
            if ( res ) {
                distances.push_back( std::abs( observation_distance - projection_distance ) );
            }
        }
    }

    if( distances.empty()) {
        return 0;
    }

    size_t n = distances.size() / 2;
    std::nth_element( distances.begin(), distances.begin() + n, distances.end() );
    return 1 / std::log( 2 ) * distances[n];
}

bool
HMM::get_true_distance( StateID cur_state_id, StateID next_state_id, double& distance )
{
    bool can_transition = is_transition_possible( m_hidden_states[cur_state_id], m_hidden_states[next_state_id] );
    if( can_transition ) {
        // roads are connected
        auto diff_lat = std::abs( m_hidden_states[cur_state_id].projection.latitude -
                                  m_hidden_states[next_state_id].projection.latitude );
        auto diff_lon = std::abs( m_hidden_states[cur_state_id].projection.longitude -
                                  m_hidden_states[next_state_id].projection.longitude );
        distance =  diff_lat + diff_lon;
        return true;
    }
    return false;
}


void
HMM::setup_inital_probability( )
{
// setup this with equal probabilities

    auto size = m_map.roads.size( );
    for( auto st: m_hidden_states ) {
        m_start_probabilities.push_back( 1. / size );
    }
}

void
HMM::print_results( )
{
    std::cout << "======================================= " << std::endl;
    std::cout << " FINAL RESULTS: " << std::endl;
    for( auto s: m_most_probable_states ) {
        auto state = m_hidden_states[ s ];
        std::cout << "ID: " << state.id << " Projection: [" << state.projection.longitude  << ",  " << state.projection.latitude;
        std::cout << "], " <<  std::endl;
    }

    std::cout << "======================================= " << std::endl;
}

void
HMM::calculate_viterbi( )
{
    Viterbi decoder( m_observations, m_hidden_states, m_transaction_probabilities,
                     m_emission_probabilities, m_start_probabilities);

    decoder.calculate_viterbi();
    m_most_probable_states = decoder.get_result( );
}

bool
HMM::is_transition_possible( State st1, State st2 )
{
    if ( st1.id == st2.id ) {
        return true;
    }

    return m_map.is_connected( st1.link, st2.link );
}

void
HMM::print_state( std::vector< State > states  )
{
    std::cout << " Print hidden states: " << std::endl;
    std::cout << "------------------------ " << std::endl;
    for( auto s: states ) {
        std::cout << "ID: " << s.id << " Projection: [" << s.projection.longitude  << ",  " << s.projection.latitude;
        std::cout << "], " <<  std::endl;
    }

    std::cout << "======================================= " << std::endl;
}

void
HMM::print_matrix( std::vector< StateVector> matrix, std::string name )
{
    std::cout << " Print " << name << " matrix: " << std::endl;
    std::cout << "--------------------------------------- " << std::endl;
    for ( int i = 0; i < matrix.size( ); ++i ) {
        for( int j = 0; j < matrix[i].size( ); ++j ) {
            std::cout << matrix[i][j] << "  ";
        }
        std::cout <<  "]" << std::endl;

    }
    std::cout << "======================================= " << std::endl;
}

void
HMM::print_observation_states( const std::unordered_map< ObservationID, std::vector< StateID > >& observation_states )
{
    std::cout << " Print Observation states: " << std::endl;
    std::cout << "--------------------------------------- " << std::endl;
    for ( auto ob : observation_states ) {
        std::cout <<  ob.first << "=> [";
        for ( auto id :  ob.second ) {
            std::cout << id << ": ";
        }
        std::cout << "]" << std::endl;
    }
    std::cout << "======================================= " << std::endl;
}
