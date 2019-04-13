//
//  util.h
//  waves2
//
//  Created by arun on 07/04/11.
//  Copyright 2011 __MyCompanyName__. All rights reserved.
//
#ifndef UTIL_H
#define UTIL_H

#include "vector2.h"
#include <string>
#include <vector>

class util
{
public:
    static vector2f closestPointOnLine(vector2f& p, vector2f& a, vector2f& b)
    {
        // a-b is the line, p the point in question
        vector2f c = p-a;
        vector2f V = b-a; 
        float d = V.Length();
        
        V.Normalize();
        float t = V.Dot(c);
        
        // Check to see if 奏?is beyond the extents of the line segment
        if (t < 0.0f) return (a);
        if (t > d) return (b);
        
        // Return the point between 疎?and 礎?		//set length of V to t. V is normalized so this is easy
        V.x = V.x * t;
        V.y = V.y * t;
        
        return (a+V);	
    }
    
    static vector2x closestPointOnLine(vector2x& p, vector2x& a, vector2x& b)
    {
        // a-b is the line, p the point in question
        vector2x c(p-a);
        vector2x V(b-a); 
        __int64_t d = V.lengthx();
        
        V.normalizex();
        __int64_t t = V.dotx(c);
        
        // Check to see if 奏?is beyond the extents of the line segment
        if (t < 0) return (a);
        if (t > d) return (b);
        
        // Return the point between 疎?and 礎?		//set length of V to t. V is normalized so this is easy
        V = V * (intx)t;
        
        return (a+V);	
    }
    
    static void splitString(const std::string& str, std::vector<std::string>& cont, char delim = '|') {
        std::size_t current, previous = 0;
        current = str.find(delim);
        while (current != std::string::npos) {
            cont.push_back(str.substr(previous, current - previous));
            previous = current + 1;
            current = str.find(delim, previous);
        }
        cont.push_back(str.substr(previous, current - previous));
    }
    
    static std::string stringFormat(const char *fmt, ...) {
        std::vector<char> str(150, '\0');
        va_list ap;
        while (1) {
            va_start(ap, fmt);
            auto n = vsnprintf(str.data(), str.size(), fmt, ap);
            va_end(ap);
            if ((n > -1) && (size_t(n) < str.size())) {
                return str.data();
            }
            if (n > -1)
                str.resize(n + 1);
            else
                str.resize(str.size() * 2);
        }
        return str.data();
    }
    
    static std::string trim(const std::string& str) {
        size_t first = str.find_first_not_of(' ');
        if (std::string::npos == first)
        {
            return str;
        }
        size_t last = str.find_last_not_of(' ');
        return str.substr(first, (last - first + 1));
    }
    
    static bool twoLineSegmentClosestPoints(vector2x& a, vector2x& b, vector2x& c, vector2x& d, vector2x& i1, vector2x& i2) {
        intx n1 = b.x-c.x;
        intx d1 = d.x-c.x;
        intx n2 = d.y-a.y;
        intx d2 = b.y-a.y;

        if (abs(XTOF(d1))<0.001f) {
            printf("d1%d <<\n", d1);
        }
        
//        if (abs(XTOF(d2))<0.001f) {
//            printf("d2 <<\n");
//        }
        if(abs(XTOF(d1))<0.001f || abs(XTOF(d2))<0.001f) return false;
//        if (d1<=0 || d2<=0) return false;
        
        intx s = DIVX(n2, d2);
        intx t = DIVX(n1, d1);
        
//        if(s<0 || s>FX_ONE) return false;
//        if(t<0 || t>FX_ONE) return false;
        
        i1=a+((b-a)*s);
        i2=c+((d-c)*t);
        
        return true;
    }
    
    static bool twoLineSegmentIntersection(vector2x p1, vector2x p2, vector2x p3, vector2x p4, vector2x& pa, vector2x& pb, intx& ua, intx& ub)
    {
        
        intx n1,d1;
        intx n2,d2;
        
        n1 = MULTX((p3.y-p4.y), (p1.x-p3.x)) + MULTX((p4.x-p3.x), (p1.y-p3.y));
        d1 = MULTX((p4.x-p3.x), (p1.y-p2.y)) - MULTX((p1.x-p2.x), (p4.y-p3.y));
        
        n2 = MULTX((p1.y-p2.y), (p1.x-p3.x)) + MULTX((p2.x-p1.x), (p1.y-p3.y));
        d2 = MULTX((p4.x-p3.x), (p1.y-p2.y)) - MULTX((p1.x-p2.x), (p4.y-p3.y));
        
        intx threshold = 4; // = 0.001f * 4096
        if (ABS(d1) < threshold || ABS(d2) < threshold) {
            return false;
        }
            
        intx ta = DIVX(n1, d1);
        intx tb = DIVX(n2, d2);
        
        pa = p1+(p2-p1)*ta;
        pb = p3+(p4-p3)*tb;
        return true;
        
//        //http://paulbourke.net/geometry/pointlineplane/
//        vector2x p13,p43,p21;
//        intx d1343,d4321,d1321,d4343,d2121;
//        intx numer,denom;
//
//        p13.x = p1.x - p3.x;
//        p13.y = p1.y - p3.y;
////        p13.z = p1.z - p3.z;
//        p43.x = p4.x - p3.x;
//        p43.y = p4.y - p3.y;
////        p43.z = p4.z - p3.z;
//        if (ABS(XTOF(p43.x)) < 0.001f && ABS(XTOF(p43.y)) < 0.001f)
//            return false;
//        p21.x = p2.x - p1.x;
//        p21.y = p2.y - p1.y;
////        p21.z = p2.z - p1.z;
//        if (ABS(XTOF(p21.x)) < 0.001f && ABS(XTOF(p21.y)) < 0.001f)
//            return false;
//
//        d1343 = MULTX(p13.x, p43.x) + MULTX(p13.y, p43.y);
//        d4321 = MULTX(p43.x, p21.x) + MULTX(p43.y, p21.y);
//        d1321 = MULTX(p13.x, p21.x) + MULTX(p13.y, p21.y);
//        d4343 = MULTX(p43.x, p43.x) + MULTX(p43.y, p43.y);
//        d2121 = MULTX(p21.x, p21.x) + MULTX(p21.y, p21.y);
//
//        denom = MULTX(d2121, d4343) - MULTX(d4321, d4321);
//        if (ABS(XTOF(denom)) < 0.001f)
//            return false;
//        numer = MULTX(d1343, d4321) - MULTX(d1321, d4343);
//
//        ua = DIVX(numer, denom);
//        ub = DIVX((d1343 + MULTX(d4321, ua)), d4343);
//
//        pa.x = p1.x + MULTX(ua, p21.x);
//        pa.y = p1.y + MULTX(ua, p21.y);
////        pa.z = p1.z + ua * p21.z;
//        pb.x = p3.x + MULTX(ub, p43.x);
//        pb.y = p3.y + MULTX(ub, p43.y);
////        pb.z = p3.z + ub * p43.z;
//
//        return true;
    }
};

#endif
