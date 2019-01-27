#define k 64
#define FP float
#define Int int
#define UInt unsigned
#define FPVec float4
#define IntVec int4
#define UIntVec uint4

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
