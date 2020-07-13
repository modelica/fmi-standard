fmi3Status fmi3ActivateModelPartition(fmi3Instance *instance, 
	fmi3ValueReference clockReference, fmi3Float64 activationTime) 
{
    switch (clockReference) 
	{
        case 5:
			// Input clock 10msClock
            activateModelPartition10ms(instance, activationTime);
        case 6:
			// Input clock AperiodicClock
            activateModelPartitionAperiodic(instance, activationTime);
		case 8:
			// Input clock AperiodicClock
            activateModelPartition50ms(instance, activationTime);
		...
    }
}

void activateModelPartition10ms(fmi3Instance *instance, ...) 
{
	...
	if (...) 
	{
		 // outputClock ticks
		fmi3SetClock({7});
		// inform master that output clock has ticked
		fmi3Boolean clocksTicked = fmi3True;
		instance->intermediateUpdate(..., clocksTicked, ...);
	}
	...
}

void activateModelPartitionAperiodic(...) {...}

void activateModelPartition50ms(...)  {...}

fmi3Clock outClockActivationState = fmi3ClockInactive; 

fmi3Status fmi3SetClock(fmi3ValueReference clockReference)
{
	if (outputClockReferences[0] == 7) outClockActivationState = fmi3ClockActive;
}

fmi3Status fmi3GetClock(..., fmi3ValueReference outputClockReferences, 
	fmi3Clock *clocksActivationState, ...)
{
	if (outputClockReferences[0] == 7) 
	{
		clocksActivationState[0] = outClockActivationState;
		outClockActivationState = fmi3ClockInactive; 
	}
}
