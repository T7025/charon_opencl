#pragma OPENCL EXTENSION cl_khr_fp64 : enable

void kernel calcNextPositionDouble(
        global double3 *position, global const double3 *velocity,
        global const double3 *acceleration, const double timeStep) {
    const int i = get_global_id(0);
    const double txtdiv2 = timeStep * timeStep / 2.0;
    position[i].x += velocity[i].x * timeStep + acceleration[i].x * txtdiv2;
    position[i].y += velocity[i].y * timeStep + acceleration[i].y * txtdiv2;
    position[i].z += velocity[i].z * timeStep + acceleration[i].z * txtdiv2;
}

void kernel calcNextPositionFloat(
        global float3 *position, global const float3 *velocity,
        global const float3 *acceleration, const float timeStep) {
    const int i = get_global_id(0);
    const float txtdiv2 = timeStep * timeStep / 2.0f;
    position[i].x += velocity[i].x * timeStep + acceleration[i].x * txtdiv2;
    position[i].y += velocity[i].y * timeStep + acceleration[i].y * txtdiv2;
    position[i].z += velocity[i].z * timeStep + acceleration[i].z * txtdiv2;
}

/*void calcNextPosition() {
    for (unsigned i = 0; i < mass.size(); ++i) {
        position[i] +=
                velocity[i] * settings.timeStep + acceleration[i] * settings.timeStep * settings.timeStep / 2;
    }
}

Vec3<FP> calcAcceleration(const unsigned int target) const {
    Vec3<FP> newAcceleration{0, 0, 0};
    for (unsigned j = 0; j < mass.size(); ++j) {
//            if (target == j) continue;
        const Vec3<FP> diff = position[j] - position[target];
//            FP norm = diff.norm();
//            newAcceleration += diff * (mass[j] / (norm * norm * norm));
        FP temp = sqrt(diff.x * diff.x + diff.y * diff.y + diff.z * diff.z +
                       settings.softeningLength * settings.softeningLength);
        newAcceleration += diff * (mass[j] / (temp * temp * temp));

    }
    return newAcceleration;
}*/

void kernel calcAccelerationDouble(
        global const double3 *position, global const double *mass, const double softeningLength) {
    const int gid = get_global_id(0);

}
