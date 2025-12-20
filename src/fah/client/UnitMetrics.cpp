/******************************************************************************\

                  This file is part of the Folding@home Client.

          The fah-client runs Folding@home protein folding simulations.
                    Copyright (c) 2001-2024, foldingathome.org
                               All rights reserved.

       This program is free software; you can redistribute it and/or modify
       it under the terms of the GNU General Public License as published by
        the Free Software Foundation; either version 3 of the License, or
                       (at your option) any later version.

         This program is distributed in the hope that it will be useful,
          but WITHOUT ANY WARRANTY; without even the implied warranty of
          MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
                   GNU General Public License for more details.

     You should have received a copy of the GNU General Public License along
     with this program; if not, write to the Free Software Foundation, Inc.,
           51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.

                  For information regarding this software email:
                                 Joseph Coffland
                          joseph@cauldrondevelopment.com

\******************************************************************************/

#include "UnitMetrics.h"

#include <cbang/time/Time.h>
#include <cbang/time/TimeInterval.h>
#include <cbang/log/Logger.h>
#include <cmath>

using namespace FAH::Client;
using namespace cb;
using namespace std;

UnitMetrics::UnitMetrics() {}

void UnitMetrics::reset() {
  processStartTime = 0;
  lastSkewTimer    = 0;
  clockSkew        = 0;

  lastKnownDone                  = 0;
  lastKnownTotal                 = 0;
  lastKnownProgressUpdate        = 0;
  lastKnownProgressUpdateRunTime = 0;
}

void UnitMetrics::processStarted() {
  lastSkewTimer = processStartTime = Time::now();
  lastKnownDone = lastKnownTotal = lastKnownProgressUpdate = clockSkew = 0;
}

void UnitMetrics::processEnded(uint64_t runTime) {
  processStartTime = 0;
}

void UnitMetrics::skewTimer() {
  uint64_t now = Time::now();

  // Detect and adjust for clock skew
  int64_t delta = (int64_t)now - lastSkewTimer;
  if (delta < 0 || 300 < delta) {
    LOG_WARNING("Detected clock skew (" << TimeInterval(delta)
                << "), I/O delay, laptop hibernation, other slowdown or "
                "clock change noted, adjusting time estimates");
    clockSkew += delta;
  }

  lastSkewTimer = now;
}

uint64_t UnitMetrics::getRunTimeDelta() const {
  return processStartTime ? Time::now() - processStartTime - clockSkew : 0;
}

uint64_t UnitMetrics::getRunTime(uint64_t storedRunTime) const {
  int64_t runTime = (int64_t)storedRunTime + getRunTimeDelta();
  return 0 < runTime ? runTime : 0;
}

uint64_t UnitMetrics::getRunTimeEstimate(const cb::JSON::ValuePtr &data) const {
  // If valid, use estimate provided by the WS
  uint64_t estimate = data->selectU64("wu.data.estimate", 0);
  if (estimate) return estimate;

  // Make our own estimate
  if (getKnownProgress() && lastKnownProgressUpdateRunTime)
    return lastKnownProgressUpdateRunTime / getKnownProgress();

  // Make a wild guess based on timeout or 1 day
  return 0.2 * data->selectU64("assignment.data.timeout", Time::SEC_PER_DAY);
}

double UnitMetrics::getEstimatedProgress(const cb::JSON::ValuePtr &data, uint64_t storedRunTime, double wuProgress) const {
  // If the core process is not currently running, return saved "wu_progress"
  if (!processStartTime || !lastKnownProgressUpdate)
    return wuProgress;

  // Get estimated progress since last update from core
  uint64_t runTime     = getRunTime(storedRunTime);
  double delta         = runTime - lastKnownProgressUpdateRunTime;
  double runtimeEst    = getRunTimeEstimate(data);
  double deltaProgress = (0 < delta && 0 < runtimeEst) ? delta / runtimeEst : 0;
  if (0.01 < deltaProgress) deltaProgress = 0.01; // No more than 1%

  double progress = getKnownProgress() + deltaProgress;
  return progress < 1 ? progress : 1; // No more than 100%
}

uint64_t UnitMetrics::getCreditEstimate(const cb::JSON::ValuePtr &data, uint64_t eta) const {
  uint64_t credit = data->selectU64("assignment.data.credit", 0);

  // Compute bonus estimate
  // Use request time to account for potential client/AS clock offset
  uint64_t requested = Time::parse(data->selectString("request.data.time"));
  uint64_t timeout   = data->selectU64("assignment.data.timeout",  0);
  uint64_t deadline  = data->selectU64("assignment.data.deadline", 0);
  int64_t  delta     = (int64_t)Time::now() - requested + eta;

  // No bonus after timeout
  if (0 < delta && delta < (int64_t)timeout) {
    double bonus = sqrt(0.75 * (double)deadline / delta); // Bonus formula
    if (1 < bonus) credit *= bonus;
  }

  return credit;
}

uint64_t UnitMetrics::getETA(const cb::JSON::ValuePtr &data, uint64_t storedRunTime, double wuProgress) const {
  return getRunTimeEstimate(data) * (1 - getEstimatedProgress(data, storedRunTime, wuProgress));
}

uint64_t UnitMetrics::getPPD(uint64_t creditEstimate, uint64_t runTimeEstimate) const {
  return runTimeEstimate ? (double)creditEstimate / runTimeEstimate * Time::SEC_PER_DAY : 0;
}

double UnitMetrics::getKnownProgress() const {
  return lastKnownTotal ? (double)lastKnownDone / lastKnownTotal : 0;
}

void UnitMetrics::updateKnownProgress(uint64_t done, uint64_t total) {
  if (!total || total < done) return;

  if (lastKnownDone != done || lastKnownTotal != total) {
    lastKnownDone                  = done;
    lastKnownTotal                 = total;
    lastKnownProgressUpdate        = Time::now();
    // In Unit.cpp this uses getRunTime(), so we need to be careful.
    // However, UnitMetrics knows when the process started.
    lastKnownProgressUpdateRunTime = getRunTime(0); // This is runTime since process start
  }
}
