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

    auto start_point_x = point.longitude - start.longitude;
    auto start_point_y = point.latitude - start.latitude;
    auto start_end_x = end.longitude - start.longitude;
    auto start_end_y = end.latitude - start.latitude;
    auto dot = start_end_x * start_point_x + start_end_y * start_point_y;
    double hy =   start_end_x * start_end_x + start_end_y * start_end_y ;
    double val = dot / hy;

    GeoCoordinates result;
    if ( val < 0) {
        result.longitude = start.longitude;
        result.latitude = start.latitude;
    } else if (val > 1) {
        result.longitude = end.longitude;
        result.latitude = end.latitude;
    } else {
        result.longitude = start.longitude + start_end_x * val;
        result.latitude = start.latitude + start_end_y * val;
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
