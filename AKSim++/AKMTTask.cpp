#include "AKMTTask.h"
#include "AKThread.h"
#include "AKSchedulingPoint.h"
#include <cassert>
#include <iostream>

int AKMTTask::instanceCounter = 0;

AKMTTask::AKMTTask(int cost, AKThread* thread) : AKTask(cost) {
	_id = instanceCounter++;
	_thread = thread;
	_forkedThread = 0;
	_joinedThread = 0;
}

// AKMTTask::~AKMTTask() {

// }


void AKMTTask::setForkedThread(AKThread* thread) {
	assert(_joinedThread == NULL);
	_forkedThread = thread;
}

void AKMTTask::setJoinedThread(AKThread* thread) {
	assert(_forkedThread == NULL);
	_joinedThread = thread;

}

AKTask* AKMTTask::continuation() {
	for (it = _successors.begin(); it != _successors.end(); ++it) {
		if ( (*it)->thread() == _thread ) {
			return *it;
		}
	}
	return 0;
}

AKSchedulingPoint* AKMTTask::runStep() {
	if (_stepsRemaining == -1) {
		assert(_state == AKTaskStateReady);
		_stepsRemaining = _cost;
		_state = AKTaskStateRunning;
	}
	_stepsRemaining--;
	if (_stepsRemaining == 0) {
		_state = AKTaskStateFinished;
		this->updateSuccessors();
		if (this->isFork()) {
			AKTask* cont = this->continuation();
			assert(cont);
			return AKSchedulingPoint::forkSchedulingPoint(_forkedThread);
		} else if (this->isJoin()) {
			return AKSchedulingPoint::joinSchedulingPoint(_joinedThread);
		} else {
			return AKSchedulingPoint::completionSchedulingPoint(_thread);
		}
	}
	return NULL;
}

void AKMTTask::print() {
	std::cout << "\t\tTask " << _id << " (costs " << _cost;
	if (this->isFork()) {
		std::cout << "; forks Thread " << _forkedThread->id();
	} else if (this->isJoin()) {
		std::cout << "; joins Thread " << _joinedThread->id(); 
	}
	std::cout << "; pred: "; printSet(_predecessors);
	std::cout << "; succ: "; printSet(_successors);
	std::cout << "; state: " << this->stateString() << ");\n";
}