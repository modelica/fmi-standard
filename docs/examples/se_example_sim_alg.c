
#include <stdio.h>
#include <math.h>
#include <stdlib.h>

#include "fmi3Functions.h"

void ScheduleTaskAperiodic(fmi3Float64 interval) {

}
void ScheduleExternalTask() {

}


fmi3Instance ModelInstance = NULL;
fmi3Float64 ActivationTime = 0.0;
fmi3ValueReference AInputReferences[] = { 0,1 };
fmi3ValueReference AOutputReferences[] = { 2 };
fmi3Float64 AInput[] = { 0.0, 0.0 };
fmi3Float64 AOutput[] = { 0.0 };
fmi3ValueReference ClockReference10ms = 5;
size_t ClockElementIndex = 0;

/* tag::SE_Task10ms[] */
void ExecuteModelPartition10ms()
{
	fmi3SetFloat64(ModelInstance, AInputReferences, 2, AInput, 2);
	fmi3ActivateModelPartition(ModelInstance, ClockReference10ms, ClockElementIndex, ActivationTime);
	fmi3GetFloat64(ModelInstance, AOutputReferences, 1, AOutput, 1);
}
/* end::SE_Task10ms[] */
/* tag::SE_IntermediateUpdate[] */
void CallbackIntermediateUpdate(fmi3InstanceEnvironment instanceEnvironment,
	fmi3Float64  intermediateUpdateTime,
	fmi3Boolean  clocksTicked,
	fmi3Boolean  intermediateVariableSetRequested,
	fmi3Boolean  intermediateVariableGetAllowed,
	fmi3Boolean  intermediateStepFinished,
	fmi3Boolean  canReturnEarly,
	fmi3Boolean* earlyReturnRequested,
	fmi3Float64* earlyReturnTime) {
	fmi3Float64 interval[] = { 0.0 };
	fmi3IntervalQualifier intervalQualifier[] = { fmi3NotYetKnown };

	if (clocksTicked == fmi3True)
	{
		// ask FMU if countdown clock is about to tick
		const fmi3ValueReference aperiodicClockReferences[] = { 6 };
		fmi3GetIntervalDecimal(ModelInstance,
			aperiodicClockReferences,
			1,
			interval,
			intervalQualifier,
			1);
		if (intervalQualifier[0] == fmi3NewInterval)
		{
			// schedule task for AperiodicClock with a delay
			ScheduleTaskAperiodic(interval[0]);
		}

		// ask FMU if output clock has ticked
		fmi3ValueReference outputClockReferences[] = { 7 };
		fmi3Boolean clocksActivationState[] = { fmi3ClockInactive };
		fmi3GetClock(ModelInstance,
			outputClockReferences,
			1,
			clocksActivationState,
			1);
		if (clocksActivationState[0])
		{
			// schedule some external task
			ScheduleExternalTask();
		}
	}
}
/* end::SE_IntermediateUpdate[] */