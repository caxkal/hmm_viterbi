#pragma once

#include "map.h"

double
Map::get_distance( const GeoCoordinates& start_point, const GeoCoordinates& end_point)
{
    auto diff_lat =  start_point.latitude - end_point.latitude;
    auto diff_long = start_point.longitude - end_point.longitude;

    return std::hypot( diff_lat, diff_long );
}

GeoCoordinates
Map::get_closest_point( const Link& link, const GeoCoordinates& point)
{
    auto start = link.start_point;
    auto end = link.end_point;

    auto APx = point.longitude - start.longitude;
    auto APy = point.latitude - start.latitude;
    auto ABx = end.longitude - start.longitude;
    auto ABy = end.latitude - start.latitude;
    auto dot = ABx * APx + ABy * APy;
    double hy =   ABx* ABx + ABy * ABy ;
    double t = dot / hy;

    GeoCoordinates result;
    if ( t < 0) {
        result.longitude = start.longitude;
        result.latitude = start.latitude;
    } else if (t > 1) {
        result.longitude = end.longitude;
        result.latitude = end.latitude;
    } else {
        result.longitude = start.longitude + ABx * t;
        result.latitude = start.latitude + ABy * t;
    }

    return result;
}

bool
Map::is_connected( const Link& link1, const Link& link2 )
{
    return link1.start_point == link2.start_point || link1.start_point == link2.end_point ||
           link1.end_point == link2.start_point || link1.end_point == link2.end_point;
}

bool
operator==( const GeoCoordinates& coord1, const GeoCoordinates& coord2 )
{
    // Todo: for double numbers should be used comparision with epsilon
    return ( coord1.longitude == coord2.longitude && coord1.latitude == coord2.latitude );
}
