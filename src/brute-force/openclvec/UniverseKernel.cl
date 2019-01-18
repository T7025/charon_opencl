// FP and FP3 get defined at runtime.
//#define FP FP
//#define FP3 FP3
//#define fp_vec fp_vec

// To format:
// "\s(global|local|kernel)\s" -> "/*$1*/"
// Autoformat
// "/\*(global|local|kernel)\*/" -> " $1 "

#pragma OPENCL EXTENSION cl_khr_fp64 : enable

void kernel
calcNextStep(
        global
const fp_vec *mass,
        global
fp_vec *xPosition,
        global
fp_vec *yPosition,
        global
fp_vec *zPosition,
        global
fp_vec *xVelocity,
        global
fp_vec *yVelocity,
        global
fp_vec *zVelocity,
        global
fp_vec *xAcceleration,
        global
fp_vec *yAcceleration,
        global
fp_vec *zAcceleration,
const FP timeStep,
const FP softeningLength,
const unsigned numberOfBodies,
const unsigned numberOfVec,
const unsigned vecSize,
const unsigned doneFirstStep,
const unsigned doneFirstAccCalc
) {
const int gid = get_global_id(0);
if (gid >= numberOfBodies) {
return;
}
const fp_vec globXPos = xPosition[gid];
const fp_vec globYPos = yPosition[gid];
const fp_vec globZPos = zPosition[gid];
if (doneFirstStep) {

fp_vec xAcc = (fp_vec) 0;
fp_vec yAcc = (fp_vec) 0;
fp_vec zAcc = (fp_vec) 0;
for (
int i = 0;
i<numberOfVec;
++i) {
const fp_vec xDiff = xPosition[i] - globXPos;
const fp_vec yDiff = yPosition[i] - globYPos;
const fp_vec zDiff = zPosition[i] - globZPos;

const fp_vec temp = mass[i] * pown(rsqrt(
        xDiff * xDiff + yDiff * yDiff + zDiff * zDiff + softeningLength * softeningLength), 3);
xAcc +=
xDiff *temp;
yAcc +=
yDiff *temp;
zAcc +=
zDiff *temp;

for (
int j = 0;
j<vecSize;
++j) {
for (
int k = 0;
k<vecSize;
++k) {
const FP3 diff = {((const FP *) &globXPos)[k] - ((const FP *) &globXPos)[j],
                  ((const FP *) &globYPos)[k] - ((const FP *) &globYPos)[j],
                  ((const FP *) &globZPos)[k] - ((const FP *) &globZPos)[j]};
const FP temp = rsqrt(dot(diff, diff) + softeningLength * softeningLength);
const FP3 acc = diff * (FP3)(((global
FP*)&mass[i])[k] *
pown(temp,
3));
((FP*)&xAcc)[j] += acc.
x;
((FP*)&yAcc)[j] += acc.
y;
((FP*)&zAcc)[j] += acc.
z;
}
}
}

if (!doneFirstAccCalc) {
const fp_vec timeStepVec = (fp_vec) timeStep;
xVelocity[gid] +=
xAcc *timeStepVec;
yVelocity[gid] +=
yAcc *timeStepVec;
zVelocity[gid] +=
zAcc *timeStepVec;
}
else {
const fp_vec timeStepVec = (fp_vec)(timeStep * 0.5);
xVelocity[gid] += (xAcceleration[gid] + xAcc) *
timeStepVec;
yVelocity[gid] += (yAcceleration[gid] + yAcc) *
timeStepVec;
zVelocity[gid] += (zAcceleration[gid] + zAcc) *
timeStepVec;

}
xAcceleration[gid] =
xAcc;
yAcceleration[gid] =
yAcc;
zAcceleration[gid] =
zAcc;
}

barrier(CLK_GLOBAL_MEM_FENCE);
xPosition[gid] += xVelocity[gid] * (fp_vec)(timeStep) + xAcceleration[gid] * (fp_vec)(
timeStep *timeStep
* 0.5);
yPosition[gid] += yVelocity[gid] * (fp_vec)(timeStep) + yAcceleration[gid] * (fp_vec)(
timeStep *timeStep
* 0.5);
zPosition[gid] += zVelocity[gid] * (fp_vec)(timeStep) + zAcceleration[gid] * (fp_vec)(
timeStep *timeStep
* 0.5);
}
