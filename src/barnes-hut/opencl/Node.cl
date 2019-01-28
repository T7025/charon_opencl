//#define k 32
//#define Int int
//#define UInt unsigned
//#define Float float
//#define IntVec intX
//#define UIntVec uintX
//#define FloatVec floatX
//#define Float3 float3

struct Node {
    SFCIndex sfcIndex;
    UInt depth;
    Float3 position;
    FLoat mass;

    union {
        Int children[8];
        struct {
            Float3 velocity, acceleration;
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
        auto shift = k - current->depth;
        // If xor of (depth) most significant bits is zero (=they are equal),
        // then this Node is a parent of other Node.
        return current->depth == 0 || (((current->sfcIndex.x ^ other->sfcIndex.x) >> shift) == 0
                              && ((current->sfcIndex.y ^ other->sfcIndex.y) >> shift) == 0
                              && ((current->sfcIndex.z ^ other->sfcIndex.z) >> shift) == 0);
    }
    return 0;
}
