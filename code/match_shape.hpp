#pragma once

#include "match.hpp"
#include "shape.hpp"

// Members binding for Shape hierarchy

template <> struct match_members<Shape>    {};

template <> struct match_members<Circle>   { CM(0,Circle::get_center); CM(1,Circle::radius); };
template <> struct match_members<Square>   { CM(0,Square::upper_left); CM(1,Square::side);   };
template <> struct match_members<Triangle> { CM(0,Triangle::first);    CM(1,Triangle::second); CM(2,Triangle::third); };

// Members binding for std::pair and cloc C struct we use for unions case

template <typename X, typename Y> 
            struct match_members<std::pair<X,Y>> { CM(0,std::pair<X,Y>::first);  CM(1,std::pair<X,Y>::second); };
template <> struct match_members<cloc>     { CM(0,cloc::first); CM(1,cloc::second); };

// Members binding for discriminated union (ADT) case of Shape hierarchy

template <> struct match_members<ADTShape> { CM(0,ADTShape::kind); };

template <> struct match_members_ex<ADTShape,ADTShape::circle>   { CM(0,ADTShape::center);     CM(1,ADTShape::radius); };
template <> struct match_members_ex<ADTShape,ADTShape::square>   { CM(0,ADTShape::upper_left); CM(1,ADTShape::size); };
template <> struct match_members_ex<ADTShape,ADTShape::triangle> { CM(0,ADTShape::first);      CM(1,ADTShape::second); CM(2,ADTShape::third); };

// Members binding for inherited class from the above ADT

template <> struct match_members<ADTShapeEx> { CM(0,ADTShape::kind); };
template <> struct match_members_ex<ADTShapeEx,ADTShape::circle>   { CM(0,ADTShape::center);     CM(1,ADTShape::radius); CM(2,ADTShapeEx::extra_field); };
template <> struct match_members_ex<ADTShapeEx,ADTShape::square>   { CM(0,ADTShape::upper_left); CM(1,ADTShape::size);   CM(2,ADTShapeEx::extra_field); };
template <> struct match_members_ex<ADTShapeEx,ADTShape::triangle> { CM(0,ADTShape::first);      CM(1,ADTShape::second); CM(2,ADTShape::third); CM(3,ADTShapeEx::extra_field); };