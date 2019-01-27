#define k 64
#define FP float
#define Int int
#define UInt unsigned
#define FPVec float4
#define IntVec int4
#define UIntVec uint4

struct Position {
    FP x, y, z;
};

struct Node {
    SFCIndex sfcIndex;
    UInt depth;
    Position position;
    FP mass;

    union {
        Int children[8];
        struct {

        } leafData;
    } data;
};

UInt eq(const Node *lhs, const Node *rhs) {
    return eq_SFCIndex(&lhs->sfcIndex, &rhs->sfcIndex) && lhs->depth == rhs->depth;
}

UInt less(const Node *lhs, const Node *rhs) {
    return less_SFCIndex(&lhs->sfcIndex, &rhs->sfcIndex)
        || (eq_SFCIndex(&lhs->sfcIndex, &rhs->sfcIndex) && lhs->depth > rhs->depth);
}

UInt isLeaf(const Node *node) {
    return node->depth == k;
}

UInt isParentOf(const Node *current, const Node *other) {
    if (current->depth < other->depth) {
        UInt shift = k - current->depth;
        const UInt isMax = static_cast<const unsigned>(shift == sizeof(uintv) * 8);
        shift -= isMax;
        return (((current->sfcIndex.x ^ other->sfcIndex.x) >> shift) >> isMax == 0
                && ((current->sfcIndex.y ^ other->sfcIndex.y) >> shift) >> isMax == 0
                && ((current->sfcIndex.z ^ other->sfcIndex.z) >> shift) >> isMax == 0);
    }
    return 0;
}
