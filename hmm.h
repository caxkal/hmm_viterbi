
#pragma once

#include "map.h"

#include <unordered_map>

using ObservationID = std::uint32_t;
using StateID = std::uint32_t;
using StateVector = std::vector< double >;

struct Observation
{
    Observation( ObservationID id, GeoCoordinates geo )
        : id( id )
        , observation( geo )
    {}

    ObservationID id;
    GeoCoordinates observation;
};

struct State
{
    State( StateID id, GeoCoordinates projection, Link state )
        : id( id )
        , projection( projection )
        , link ( state )
    {}

    StateID id;
    GeoCoordinates projection;
    Link link;
};

class HMM 
{
public:

    HMM( );

    void calculate_viterbi( );

    void print_results( );

private:
    void setup_inital_map_and_observations( );
    void candidate_selection( );
    void allocate_memory( );
    void calculate_emission_probability( );
    void calculate_transition_probability( );
    double calculate_sigma( const Observation& );
    void setup_inital_probability( );
    bool is_transition_possible( State, State );
    double calculate_betta( const std::vector< StateID >& current_states, const std::vector< StateID >& next_states,
                            double distance );
    bool get_true_distance( StateID cur_state_id, StateID next_state_id, double& distance );
    void print_matrix( std::vector< StateVector >, std::string );
    void print_state( std::vector< State >  );
    void print_observation_states( const std::unordered_map< ObservationID, std::vector< StateID > >&  );

private:
    Map m_map;
    std::vector< Observation > m_observations;
    std::vector< State > m_hidden_states;

    std::vector< StateVector > m_transaction_probabilities;
    std::vector< StateVector > m_emission_probabilities;
    std::vector< double > m_start_probabilities;

    std::unordered_map< ObservationID, std::vector< StateID > > m_observation_states;
    std::vector< StateID > m_most_probable_states;
};

