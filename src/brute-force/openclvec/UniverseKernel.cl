// FP and FP3 get defined at runtime.
//#define FP FP
//#define FP3 FP3
//#define fp_vec fp_vec

// To format:
// "\s(global|local|kernel)\s" -> "/*$1*/"
// Autoformat
// "/\*(global|local|kernel)\*/" -> " $1 "

#pragma OPENCL EXTENSION cl_khr_fp64 : enable

void kernel calcNextStep(
         global const fp_vec *mass,
         global fp_vec *xPosition,
         global fp_vec *yPosition,
         global fp_vec *zPosition,
         global fp_vec *xVelocity,
         global fp_vec *yVelocity,
         global fp_vec *zVelocity,
         global fp_vec *xAcceleration,
         global fp_vec *yAcceleration,
         global fp_vec *zAcceleration,
                  const FP timeStep,
                  const FP softeningLength,
                  const unsigned numberOfBodies,
                  const unsigned numberOfVec,
                  const unsigned vecSize,
                  const unsigned doneFirstStep,
                  const unsigned doneFirstAccCalc
) {
    const int gid = get_global_id(0);
//    if (gid >= numberOfBodies) {
    if (gid >= numberOfVec) {
        return;
    }
    const fp_vec globXPos = xPosition[gid];
    const fp_vec globYPos = yPosition[gid];
    const fp_vec globZPos = zPosition[gid];

//    for (int i = 0; i < vecSize; ++i) {
//        printf("%s %f, %f %f %f\n", i == 0 ? '-':'|', ((global FP *) &mass[gid])[i],
//               ((const FP *) &globXPos)[i], ((const FP *) &globYPos)[i], ((const FP *) &globZPos)[i]);
//    }
    if (doneFirstStep) {

        fp_vec xAcc = (fp_vec) 0;
        fp_vec yAcc = (fp_vec) 0;
        fp_vec zAcc = (fp_vec) 0;
        for (int i = 0; i < numberOfVec; ++i) {
            const fp_vec xDiff = xPosition[i] - globXPos;
            const fp_vec yDiff = yPosition[i] - globYPos;
            const fp_vec zDiff = zPosition[i] - globZPos;
//            printf("%d, %d (nob: %d, nov: %d, vs: %d)\n(%f\t %f)\n(%f\t %f)\n(%f\t %f)\n", gid, i, numberOfBodies, numberOfVec, vecSize,
//                    xDiff.s0, xDiff.s1, yDiff.s0, yDiff.s1, zDiff.s0, zDiff.s1);
            const fp_vec temp = mass[i] * pown(rsqrt(
                    xDiff * xDiff + yDiff * yDiff + zDiff * zDiff + softeningLength * softeningLength), 3);
            xAcc += xDiff * temp;
            yAcc += yDiff * temp;
            zAcc += zDiff * temp;

        }
        for (int i = 0; i < vecSize; ++i) {
            for (int j = 0; j < vecSize; ++j) {
                const FP3 diff = {((const FP *) &globXPos)[j] - ((const FP *) &globXPos)[i],
                                  ((const FP *) &globYPos)[j] - ((const FP *) &globYPos)[i],
                                  ((const FP *) &globZPos)[j] - ((const FP *) &globZPos)[i]};
                const FP temp = rsqrt(dot(diff, diff) + softeningLength * softeningLength);
                const FP3 acc = diff * (FP3)(((global FP *) &(mass[gid]))[j] *pown(temp, 3));
//                    printf("%f,  %f %f %f\n", (((global FP *) &(mass[i]))[j]), acc.x, acc.y, acc.z);
                ((FP * ) &xAcc)[i] += acc.x;
                ((FP * ) &yAcc)[i] += acc.y;
                ((FP * ) &zAcc)[i] += acc.z;
            }
        }
//        printf()

        if (!doneFirstAccCalc) {
            const fp_vec timeStepVec = (fp_vec) timeStep;
            xVelocity[gid] += xAcc * timeStepVec;
            yVelocity[gid] += yAcc * timeStepVec;
            zVelocity[gid] += zAcc * timeStepVec;
        }
        else {
            const fp_vec timeStepVec = (fp_vec)(timeStep * 0.5);
            xVelocity[gid] += (xAcceleration[gid] + xAcc) * timeStepVec;
            yVelocity[gid] += (yAcceleration[gid] + yAcc) * timeStepVec;
            zVelocity[gid] += (zAcceleration[gid] + zAcc) * timeStepVec;

        }
        xAcceleration[gid] = xAcc;
        yAcceleration[gid] = yAcc;
        zAcceleration[gid] = zAcc;
    }

    barrier(CLK_GLOBAL_MEM_FENCE);
    xPosition[gid] += xVelocity[gid] * (fp_vec)(timeStep) + xAcceleration[gid] * (fp_vec)(timeStep * timeStep * 0.5);
    yPosition[gid] += yVelocity[gid] * (fp_vec)(timeStep) + yAcceleration[gid] * (fp_vec)(timeStep * timeStep * 0.5);
    zPosition[gid] += zVelocity[gid] * (fp_vec)(timeStep) + zAcceleration[gid] * (fp_vec)(timeStep * timeStep * 0.5);
}
