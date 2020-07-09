// master implementation

// Task scheduler
	// every 10 ms:
	Scheduler->ScheduleTask(Task10ms);
	// every 50ms:
	Scheduler->ScheduleTask(Task50ms);

Task10ms.Execute()
{
	// Set inputs with valueReference 0 and 1 associated to clockIndex 5
	fmi3SetFloat64(s, {0,1}, &AIn);
	// call for 10msClock tick (clockIndex 5)
	fmi3ActivateModelPartition(s, 5, 0, Task10ms.ActivationTime);
  	// Get output with valueReference 2 associated to clockIndex 0
	fmi3GetFloat64(s, {2}, &AOut);
};

TaskAperiodic.Execute()
{
	// Set input with valueReference 3 associated to clockIndex 6
	fmi3SetFloat64(s, {3}, &BIn);
	// call for AperiodicClock tick (clockIndex 6)
	fmi3ActivateModelPartition(s, 6, 0, TaskAperiodic.ActivationTime);
  	// Get output with valueReference 4 associated to clockIndex 6
	fmi3GetFloat64(s, {4}, &BOut);
};

Task50ms.Execute()
{
	...
	// call for 50msClock tick (clockIndex 8)
	fmi3ActivateModelPartition(s, 8, 0, Task50ms.ActivationTime);
};

void CallbackIntermediateUpdate(..., fmi3Boolean clocksTicked, ...)
{
	fmi3ValueReference outputClockReferences = {7};	
	fmi3Boolean[] clocksActivationState = {fmi3ClockInactive};
	if (clocksTicked)
	{
		// ask FMU if output clock has ticked
		fmi3GetClocks(... outputClockReferences, &clocksActivationState, ...);
	}
	if (clocksActivationState[0])
	{
		// schedule task for AperiodicClock 
		Scheduler->ScheduleTask(TaskAperiodic);
	}
}
