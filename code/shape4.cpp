#include <utility>
#include "match_generic.hpp"

typedef std::pair<double,double> loc;
struct cloc { double first; double second; };

struct Shape;
struct Circle;
struct Square;
struct Triangle;

struct ShapeVisitor
{
	virtual void visit(const Circle&)   {}
	virtual void visit(const Square&)   {}
	virtual void visit(const Triangle&) {}
};

// An Algebraic Data Type implemented through inheritance
struct Shape
{
    enum ShapeKind {SK_Circle,SK_Square,SK_Triangle};
    ShapeKind kind;
    Shape(ShapeKind k) : kind(k) {}
    virtual void accept(ShapeVisitor& v) = 0;
};

struct Circle : Shape
{
    Circle(const loc& c, const double& r) : Shape(SK_Circle), center(c), radius(r) {}

    void accept(ShapeVisitor& v) { v.visit(*this); }

    const loc& get_center() const { return center; }

    loc  center;
    double radius;
};

struct Square : Shape
{
    Square(const loc& c, const double& s) : Shape(SK_Square), upper_left(c), side(s) {}

    void accept(ShapeVisitor& v) { v.visit(*this); }

    loc  upper_left;
    double side;
};

struct Triangle : Shape
{
    Triangle(const loc& a, const loc& b, const loc& c) : Shape(SK_Triangle), first(a), second(b), third(c) {}

    void accept(ShapeVisitor& v) { v.visit(*this); }

    loc first;
    loc second;
    loc third;
};

struct ADTShape
{
	enum shape_kind {circle, square, triangle} kind;

#ifndef POD_ONLY
	ADTShape(const cloc& c, double r) : kind(circle), center(c), radius(r) {}
	ADTShape(double s, const cloc& c) : kind(square), upper_left(c), size(s) {}
	ADTShape(const cloc& x, const cloc& y, const cloc& z) : kind(triangle), first(x), second(y), third(z) {}
	virtual ~ADTShape() {}
#endif

#ifndef _MSC_VER
	union
	{
		struct { cloc center;     double radius; }; // as_circle;
		struct { cloc upper_left; double size; }; //   as_square;
		struct { cloc first, second, third; }; //    as_triangle;
	};
#else
    // MSVC doesn't seem to allow anonymous structs inside union so we just dump
    // here same members name directly to make the rest just compile at least.
	cloc center;     double radius; // as_circle;
	cloc upper_left; double size;   // as_square;
	cloc first, second, third;    // as_triangle;
#endif
};

template <> struct match_members<Shape>    { KS(Shape::kind); };

template <> struct match_members<Circle>   { KV(Shape::SK_Circle);  CM(0,Circle::get_center); CM(1,Circle::radius); };
template <> struct match_members<Square>   { KV(Shape::SK_Square);  CM(0,Square::upper_left); CM(1,Square::side);   };
template <> struct match_members<Triangle> { KV(Shape::SK_Triangle);CM(0,Triangle::first);    CM(1,Triangle::second); CM(2,Triangle::third); };

template <> struct match_members<ADTShape> { KS(ADTShape::kind); };

template <> struct match_members<ADTShape,ADTShape::circle>   { KV(ADTShape::circle);  CM(0,ADTShape::center);     CM(1,ADTShape::radius); };
template <> struct match_members<ADTShape,ADTShape::square>   { KV(ADTShape::square);  CM(0,ADTShape::upper_left); CM(1,ADTShape::size); };
template <> struct match_members<ADTShape,ADTShape::triangle> { KV(ADTShape::triangle);CM(0,ADTShape::first);      CM(1,ADTShape::second); CM(2,ADTShape::third); };

int main()
{
#ifdef POD_ONLY
    ADTShape ac = {ADTShape::circle,   {{{1,1}, 7}}};
    ADTShape as = {ADTShape::square,   {{{1,1}, 2}}};
    ADTShape at = {ADTShape::triangle, {{{1,1}, 1}}};
#else
    cloc l00 = {0,0};
    cloc l11 = {1,1};
    cloc l10 = {1,0};
    ADTShape ac(l11, 7);
    ADTShape as(2, l11);
    ADTShape at(l11, l10, l00);
#endif

    ADTShape* adtshapes[] = {&ac,&as,&at};

    Shape* c = new Circle(loc(1,1),7);
    Shape* s = new Square(loc(1,1),2);
    Shape* t = new Triangle(loc(1,1),loc(1,0),loc(0,0));

    Shape* shapes[] = {c,s,t};

    wildcard _;
    double   x;
    loc      l;
    cloc     cl;

    double m = 0.0;

    for (size_t i = 0; i < 3; ++i)
    {
        KIND_SWITCH(shapes[i])
        {
        KIND_CASES_BEGIN 
        KIND_CASE(Circle,_,x) std::cout << "Circle"   << std::endl; m += x;       break;
        KIND_CASE(Square,_,x) std::cout << "Square"   << std::endl; m += x;       break;
        KIND_CASE(Triangle,l) std::cout << "Triangle" << std::endl; m += l.first; break;
      //KIND_CASE(Triangle,l) std::cout << "Triangle" << std::endl; m += l.first; break; // NOTE: Not possible to have another kind match
        KIND_CASES_END
        }
    }

    std::cout << m << std::endl;

    m = 0.0;

    for (size_t i = 0; i < 3; ++i)
    {
        TYPE_SWITCH(shapes[i])
        {
        TYPE_CASES_BEGIN 
        TYPE_CASE(Circle)   std::cout << "Circle"   << std::endl; m += matched->radius;      break;
        TYPE_CASE(Square)   std::cout << "Square"   << std::endl; m += matched->side;        break;
        TYPE_CASE(Triangle) std::cout << "Triangle" << std::endl; m += matched->first.first; break;
        TYPE_CASE(Triangle) std::cout << "Triangle" << std::endl; m += matched->first.first; break; // NOTE: Possible to have another type case match
        TYPE_CASES_END
        }
    }

    std::cout << m << std::endl;

    m = 0.0;

    for (size_t i = 0; i < 3; ++i)
    {
        SWITCH(shapes[i])
        {
        CASES_BEGIN 
        CASE(Circle,_,x)    std::cout << "Circle"   << std::endl; m += x;       break;
        CASE(Square,_,x)    std::cout << "Square"   << std::endl; m += x;       break;
        CASE(Triangle,l)    std::cout << "Triangle" << std::endl; m += l.first; break;
        CASE(Triangle,l)    std::cout << "Triangle" << std::endl; m += l.first; break; // NOTE: Possible to have another regular match
        CASES_END
        }
    }

    std::cout << m << std::endl;

    m = 0.0;

    for (size_t i = 0; i < 3; ++i)
    {
        UNION_SWITCH(adtshapes[i])
        {
        UNION_CASES_BEGIN 
        UNION_CASE(ADTShape::circle,_,x)  std::cout << "ADTCircle"   << std::endl; m += x;       break;
        UNION_CASE(ADTShape::square,_,x)  std::cout << "ADTSquare"   << std::endl; m += x;       break;
        UNION_CASE(ADTShape::triangle,cl) std::cout << "ADTTriangle" << std::endl; m += cl.first;break;
      //UNION_CASE(ADTShape::triangle,cl) std::cout << "ADTTriangle" << std::endl; m += cl.first;break; // NOTE: Not possible to have another union match
        UNION_CASES_END
        }
    }

    std::cout << m << std::endl;

}

int main1()
{
    Shape* c = new Circle(loc(1,1),7);
    Shape* s = new Square(loc(2,2),2);
    Shape* t = new Triangle(loc(0,0),loc(0,1),loc(1,0));

    Shape* shapes[] = {c,s,t};
    double m = 0.0;

    for (size_t i = 0; i < 3; ++i)
    {
        auto const __selector_ptr = addr(shapes[i]); 
        switch (apply_member(__selector_ptr, match_members<remove_ref<decltype(*__selector_ptr)>::type>::kind_selector()))
        {
//-->8--------------------------------------------------------------------------  //
            {                                                                     //
//-->8--------------------------------------------------------------------------  
            }                                                                     //
        case match_members<Circle>::kind_value:                                   //
            {                                                                     //
                auto result_of_dynamic_cast = stat_cast<Circle>(__selector_ptr);  //
//-->8--------------------------------------------------------------------------  //                 
                std::cout << "Circle"   << std::endl; 
                m += result_of_dynamic_cast->radius;      
                break;
//-->8--------------------------------------------------------------------------  //
            }                                                                     //
        case match_members<Square>::kind_value:                                   //
            {                                                                     //
                auto result_of_dynamic_cast = stat_cast<Square>(__selector_ptr);  //
//-->8--------------------------------------------------------------------------  //                 
                std::cout << "Square"   << std::endl; 
                m += result_of_dynamic_cast->side;        
                break;
//-->8--------------------------------------------------------------------------  //
            }                                                                     //
        case match_members<Triangle>::kind_value:                                 //
            {                                                                     //
                auto result_of_dynamic_cast = stat_cast<Triangle>(__selector_ptr);//
//-->8--------------------------------------------------------------------------  //               
                std::cout << "Triangle" << std::endl; 
                m += result_of_dynamic_cast->first.first; 
                break;
//-->8--------------------------------------------------------------------------  //
            }                                                                     //
        default: ;                                                                //
//-->8--------------------------------------------------------------------------  //
        }
    }

    std::cout << m << std::endl;

    m = 0.0;

    for (size_t i = 0; i < 3; ++i)
    {
        static vtblmap<type_switch_info&> __vtbl2lines_map; 
        auto const __selector_ptr = addr(shapes[i]); 
        const void* __casted_ptr; 
        type_switch_info& __switch_info = __vtbl2lines_map.get(__selector_ptr); 
        switch (__switch_info.line)
        {
//--8<--------------------------------------------------------------------------  //
        default:                                                                  //
            {                                                                     //
//--8<--------------------------------------------------------------------------             
            }                                                                     //
            if (__casted_ptr = dynamic_cast<const Circle*>(__selector_ptr))       //
            {                                                                     //
                if (__switch_info.line == 0)                                      //
                {                                                                 //
                    __switch_info.line   = 97;                                    //
                    __switch_info.offset = intptr_t(__casted_ptr)-intptr_t(__selector_ptr); 
                }                                                                 //
        case 97:                                                                  //
                auto result_of_dynamic_cast = adjust_ptr<Circle>(__selector_ptr,__switch_info.offset);   
//-->8--------------------------------------------------------------------------
                std::cout << "Circle"   << std::endl; 
                m += result_of_dynamic_cast->radius;      
                break;
//--8<--------------------------------------------------------------------------  //
            }                                                                     //
            if (__casted_ptr = dynamic_cast<const Square*>(__selector_ptr))       //
            {                                                                     //
                if (__switch_info.line == 0)                                      //
                {                                                                 //
                    __switch_info.line   = 98;                                    //
                    __switch_info.offset = intptr_t(__casted_ptr)-intptr_t(__selector_ptr); 
                }                                                                 //
        case 98:                                                                  //
                auto result_of_dynamic_cast = adjust_ptr<Square>(__selector_ptr,__switch_info.offset);   
//-->8--------------------------------------------------------------------------
                std::cout << "Square"   << std::endl; 
                m += result_of_dynamic_cast->side;        
                break;
//--8<--------------------------------------------------------------------------  //
            }                                                                     //
            if (__casted_ptr = dynamic_cast<const Triangle*>(__selector_ptr))     //
            {                                                                     //
                if (__switch_info.line == 0)                                      //
                {                                                                 //
                    __switch_info.line   = 99;                                    //
                    __switch_info.offset = intptr_t(__casted_ptr)-intptr_t(__selector_ptr); 
                }                                                                 //
        case 99:                                                                  //
                auto result_of_dynamic_cast = adjust_ptr<Triangle>(__selector_ptr,__switch_info.offset); 
//-->8--------------------------------------------------------------------------
                std::cout << "Triangle" << std::endl; 
                m += result_of_dynamic_cast->first.first; 
                break;
//--8<--------------------------------------------------------------------------  //
            }                                                                     //
            if (__switch_info.line == 0)                                          //
            {                                                                     //
                __switch_info.line = 100;                                         //
            }                                                                     //
        case 100: ;                                                               //
//-->8--------------------------------------------------------------------------  //
        }
    }

    std::cout << m << std::endl;

    m = 0.0;

    wildcard _;
    double   x;
    loc      l;

    for (size_t i = 0; i < 3; ++i)
    {
        static vtblmap<type_switch_info&> __vtbl2lines_map;
        auto const __selector_ptr = addr(shapes[i]);
        const void* __casted_ptr;
        type_switch_info& __switch_info = __vtbl2lines_map.get(__selector_ptr);
        switch (__switch_info.line)
        {
//--8<--------------------------------------------------------------------------  //
        default:                                                                  //
            {                                                                     //
                {                                                                 //
//--8<--------------------------------------------------------------------------  
                }                                                                 //
            }                                                                     //
            if (__casted_ptr = dynamic_cast<const Circle*>(__selector_ptr))       //
            {                                                                     //
                if (__switch_info.line == 0)                                      //
                {                                                                 //
                    __switch_info.line   = 117;                                   //
                    __switch_info.offset = intptr_t(__casted_ptr)-intptr_t(__selector_ptr); 
                }                                                                 //
        case 117:                                                                 //
                auto result_of_dynamic_cast = adjust_ptr<Circle>(__selector_ptr,__switch_info.offset);
                if (match<Circle>(_,x)(result_of_dynamic_cast))
                {                                                                 //
//-->8--------------------------------------------------------------------------  //
                    std::cout << "Circle"   << std::endl; 
                    m += x;       
                    break;
//--8<--------------------------------------------------------------------------  //
                }                                                                 //
            }                                                                     //
            if (__casted_ptr = dynamic_cast<const Square*>(__selector_ptr))       //
            {                                                                     //
                if (__switch_info.line == 0)                                      //
                {                                                                 //
                    __switch_info.line   = 118;                                   //
                    __switch_info.offset = intptr_t(__casted_ptr)-intptr_t(__selector_ptr); 
                }                                                                 //
        case 118:                                                                 //
                auto result_of_dynamic_cast = adjust_ptr<Square>(__selector_ptr,__switch_info.offset);
                if (match<Square>(_,x)(result_of_dynamic_cast))
                {                                                                 //
//-->8--------------------------------------------------------------------------  //
                    std::cout << "Square"   << std::endl; 
                    m += x;       
                    break;
//--8<--------------------------------------------------------------------------  //
                }                                                                 //
            }                                                                     //
            if (__casted_ptr = dynamic_cast<const Triangle*>(__selector_ptr))     //
            {                                                                     //
                if (__switch_info.line == 0)                                      //
                {                                                                 //
                    __switch_info.line   = 119;                                   //
                    __switch_info.offset = intptr_t(__casted_ptr)-intptr_t(__selector_ptr); 
                }                                                                 //
        case 119:                                                                 //
                auto result_of_dynamic_cast = adjust_ptr<Triangle>(__selector_ptr,__switch_info.offset);
                if (match<Triangle>(l)(result_of_dynamic_cast))
                {                                                                 //
//-->8--------------------------------------------------------------------------  //
                    std::cout << "Triangle" << std::endl; 
                    m += l.first; 
                    break;
//--8<--------------------------------------------------------------------------  //
                }                                                                 //
            }                                                                     //
            if (__switch_info.line == 0)                                          //
            {                                                                     //
                __switch_info.line = 120;                                         //
            }                                                                     //
        case 120: ;                                                               //
//-->8--------------------------------------------------------------------------  //
        }
    }

    std::cout << m << std::endl;
    return 0;
}
