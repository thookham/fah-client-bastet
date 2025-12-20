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

#pragma once

#include <cbang/json/Value.h>
#include <cstdint>
#include <string>

namespace FAH {
  namespace Client {

    /**
     * @brief Manages timing, progress, and credit estimations for a Work Unit.
     * 
     * This class decouples the metric calculation logic from the main Unit class,
     * handling aspects like clock skew detection, run time accumulation,
     * and ETA/PPD calculations.
     */
    class UnitMetrics {
    public:
      UnitMetrics();

      /**
       * @brief Resets all metric data to initial state.
       */
      void reset();

      /**
       * @brief Updates internal state when the core process starts.
       */
      void processStarted();

      /**
       * @brief Updates internal state when the core process ends.
       * @param runTime Total run time accumulated so far.
       */
      void processEnded(uint64_t runTime);

      /**
       * @brief Periodically called to detect system clock skew.
       */
      void skewTimer();

      /**
       * @brief Returns the run time of the current core process instance.
       */
      uint64_t getRunTimeDelta() const;

      /**
       * @brief Calculates total run time (stored + current delta).
       */
      uint64_t getRunTime(uint64_t storedRunTime) const;

      /**
       * @brief Gets the best available estimate for total run time from AS/WS or local logic.
       */
      uint64_t getRunTimeEstimate(const cb::JSON::ValuePtr &data) const;
      
      /**
       * @brief Calculates progress estimation including time since last core update.
       */
      double   getEstimatedProgress(const cb::JSON::ValuePtr &data, uint64_t storedRunTime, double wuProgress) const;
      
      /**
       * @brief Estimates credit (points) earned including potential bonuses.
       */
      uint64_t getCreditEstimate(const cb::JSON::ValuePtr &data, uint64_t eta) const;
      
      /**
       * @brief Calculates estimated time until completion.
       */
      uint64_t getETA(const cb::JSON::ValuePtr &data, uint64_t storedRunTime, double wuProgress) const;
      
      /**
       * @brief Calculates Points Per Day (PPD) estimate.
       */
      uint64_t getPPD(uint64_t creditEstimate, uint64_t runTimeEstimate) const;

      /**
       * @brief Returns the last known progress reported by the core (0.0 to 1.0).
       */
      double getKnownProgress() const;

      /**
       * @brief Updates the known progress reported by the scientific core.
       */
      void updateKnownProgress(uint64_t done, uint64_t total);

      void setClockSkew(int64_t skew) { clockSkew = skew; }
      int64_t getClockSkew() const { return clockSkew; }

    private:
      uint64_t processStartTime = 0;
      uint64_t lastSkewTimer    = 0;
      int64_t  clockSkew        = 0;

      uint64_t lastKnownDone                  = 0;
      uint64_t lastKnownTotal                 = 0;
      uint64_t lastKnownProgressUpdate        = 0;
      uint64_t lastKnownProgressUpdateRunTime = 0;
    };

  }
}
