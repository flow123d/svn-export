#include <iostream>

#include "bisector.h"
#include "mathfce.h"
#include "intersection.h"

using namespace mathfce;

int TBisector::numberInstance = 0;

int TBisector::generateId() {
    return TBisector::numberInstance++;
}

TBisector::TBisector() {
    id = generateId();

    X0 = new TPoint(0, 0, 0);
    U = new TVector(0, 0, 0);
}

TBisector::TBisector(const TPoint &XX0, const TVector &UU) {
    id = generateId();

    X0 = new TPoint(XX0);
    U = new TVector(UU);
}

TBisector::TBisector(const TPoint &P0, const TPoint &P1) {
    id = generateId();

    X0 = new TPoint(P0);
    U = new TVector(P0, P1);
}

TBisector::~TBisector() {
    delete X0;
    delete U;
}

TBisector & TBisector::operator =(const TBisector &b) {
    //  U = new TVector();
    //  X0 = new TPoint();
    *(*this).U = *b.U;
    *(*this).X0 = *b.X0;

    return *this;
}

std::ostream & operator <<(std::ostream &stream, const TBisector &b) {
    stream << "U = (" << b.U->X1() << ", " << b.U->X2() << ", " << b.U->X3() << ")\n";
    stream << "X0 = [" << b.X0->X() << ", " << b.X0->Y() << ", " << b.X0->Z() << "]\n";

    return stream;
}

void TBisector::SetPoint(const TPoint &P) {
    *X0 = P;
}

void TBisector::SetVector(const TVector &UU) {
    *U = UU;
}

void TBisector::SetPoints(const TPoint &P0, const TPoint &P1) {
    *X0 = P0;
    *U = (TPoint) P1 - P0;
}

TVector TBisector::GetVector() const {
    TVector V(*U);

    return V;
}

TPoint TBisector::GetPoint() const {
    TPoint tmp;
    tmp = *X0;

    return tmp;
}

TPoint TBisector::GetPoint(double t) const {
    TPoint tmp;

    tmp.SetCoord(t * *U);
    tmp = tmp + *X0;

    return tmp;
}

bool TBisector::Belong(const TPoint &P) const {
    if (IsZero(Distance(*this, P))) {
        return true;
    } else {
        return false;
    }
}