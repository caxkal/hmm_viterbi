#pragma once

#include <cmath>
#include <vector>

struct GeoCoordinates
{
    double longitude; // x
    double latitude; // y
    // Todo: altitude
};

bool operator==( const GeoCoordinates& coord1, const GeoCoordinates& coord2 );

struct Link
{
    GeoCoordinates start_point;
    GeoCoordinates end_point; // todo: std::vector< GeoCoordinates >
    // Todo: allowed direction
};

struct Map
{
    std::vector< Link > roads;
    // todo: get_links_in_radius/bbox

    double get_distance( const GeoCoordinates& start_point, const GeoCoordinates& end_point );

    double get_geodesic_distance( )
    {
        // Todo: implement with harvesain formula
    }

    // todo implement geodesic version
    GeoCoordinates get_closest_point( const Link& link, const GeoCoordinates& point);

    bool is_connected( const Link& link1, const Link& link2 );
};
