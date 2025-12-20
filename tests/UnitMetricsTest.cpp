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

#include <cbang/Catch.h>
#include "../src/fah/client/UnitMetrics.h"
#include <cbang/json/JSON.h>
#include <cbang/time/Time.h>

using namespace FAH::Client;
using namespace cb;

TEST_CASE("UnitMetrics calculations", "[metrics]") {
  UnitMetrics metrics;

  SECTION("Initial state") {
    CHECK(metrics.getRunTimeDelta() == 0);
    CHECK(metrics.getRunTime(0) == 0);
  }

  SECTION("Process started") {
    metrics.processStarted();
    // Timing depends on Time::now(), but delta should be small initially
    CHECK(metrics.getRunTimeDelta() < 2); 
    
    cb::JSON::ValuePtr data = cb::JSON::createDict();
    data->insert("wu", cb::JSON::createDict());
    data->get("wu")->insert("estimate", (uint64_t)1000);
    
    CHECK(metrics.getRunTimeEstimate(data) == 1000);
  }

  SECTION("RunTime accumulation") {
    metrics.processStarted();
    // We can't easily mock Time::now() here without deep cbang integration,
    // but we can verify it doesn't crash and returns reasonable values.
    uint64_t rt = metrics.getRunTime(500);
    CHECK(rt >= 500);
  }

  SECTION("Progress estimation") {
    metrics.processStarted();
    cb::JSON::ValuePtr data = cb::JSON::createDict();
    data->insert("wu", cb::JSON::createDict());
    data->get("wu")->insert("estimate", (uint64_t)1000);
    
    // With 0 progress reported, it should return wuProgress
    CHECK(metrics.getEstimatedProgress(data, 0, 0.5) == 0.5);
    
    metrics.updateKnownProgress(10, 100); // 10% progress
    CHECK(metrics.getKnownProgress() == Approx(0.1));
    
    // Estimated progress should be >= known progress
    CHECK(metrics.getEstimatedProgress(data, 0, 0) >= 0.1);
  }

  SECTION("ETA and PPD") {
    cb::JSON::ValuePtr data = cb::JSON::createDict();
    data->insert("wu", cb::JSON::createDict());
    data->get("wu")->insert("estimate", (uint64_t)1000);
    data->insert("assignment", cb::JSON::createDict());
    data->get("assignment")->insert("timeout", (uint64_t)5000);
    data->get("assignment")->insert("deadline", (uint64_t)10000);
    // Request time needs to be set for CreditEstimate
    data->insert("request", cb::JSON::createDict());
    data->get("request")->insert("time", cb::Time::now().toString());

    uint64_t eta = metrics.getETA(data, 0, 0.5);
    CHECK(eta == 500); // 1000 * (1 - 0.5)

    uint64_t ppd = metrics.getPPD(1000, 1000);
    CHECK(ppd == 1000 * 24 * 3600);
  }
}
