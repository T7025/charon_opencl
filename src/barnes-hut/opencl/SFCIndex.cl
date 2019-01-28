//#define k 32
//#define Int int
//#define UInt unsigned
//#define Float float
//#define IntVec intX
//#define UIntVec uintX
//#define FloatVec floatX
//#define Float3 float3

struct SFCIndex {
    UInt x, y, z;
};

UInt eq_SFCIndex(const SFCIndex *lhs, const SFCIndex *rhs) {
    return lhs->x == rhs->x && lhs->y == rhs->y && lhs->z == rhs->z;
}

UInt lessMSB(const UInt x, const UInt y) {
    return x < y && x < (x ^ y);
}

UInt less_SFCIndex(const SFCIndex *lhs, const SFCIndex *rhs) {
    UInt result = lhs ^ rhs;
    UInt t1 = lhs->x ^ rhs->x;
    UInt t2 = lhs->y ^ rhs->y;
    if (lessMSB(&t1, &t2)) {
        result = lhs->y < rhs->y;
        t1 = t2;
    }
    t2 = lhs->z ^ rhs->z;
    if (lessMSB(&t1, &t2)) {
        result = lhs->z < rhs->z;
    }
    return result
}
