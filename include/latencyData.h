/**
 Trains Protocol: Middleware for Uniform and Totally Ordered Broadcasts
 Copyright: Copyright (C) 2010-2012
 Contact: michel.simatic@telecom-sudparis.eu

 This library is free software; you can redistribute it and/or
 modify it under the terms of the GNU Lesser General Public
 License as published by the Free Software Foundation; either
 version 3 of the License, or any later version.

 This library is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 Lesser General Public License for more details.

 You should have received a copy of the GNU Lesser General Public
 License along with this library; if not, write to the Free Software
 Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307
 USA

 Developer(s): Michel Simatic, Arthur Foltz, Damien Graux, Nicolas Hascoet, Nathan Reboud
 */

/**
 * @brief Definitions related to the data treatment required for the latency test
 * @file latencyData.h
 * @author Arthur Foltz
 * @date 07 august 2012
 */

#include <sys/time.h>

typedef struct {

  unsigned int currentRecordsNb;
  unsigned int maxRecordsNb;
  struct timeval * timevalRecords;
  double * floatRecords;
  double mean;
  double variance;
  double standardDeviation;
  double min95confidenceInterval;
  double max95confidenceInterval;
  double min99confidenceInterval;
  double max99confidenceInterval;

} pingRecord;


/**
 * @brief Create a new pingRecord struct
 * @return the newly created pingRecord
 */
pingRecord newPingRecord();

/**
 * @brief Free the previously allocated fields of a pingRecord struct
 * @param[in] record The pingRecord struct to free
 * @return void
 */
void freePingRecord(pingRecord * record);

/**
 * @brief Record a value in the timevalRecords fiels of a pingRecord struct
 * @param[in] value The value to record
 * @param[in] pingRec The pingRecord which the value must be recorded in
 * @return 0 on success
 */
int recordValue(struct timeval value, pingRecord * pingRec);

/**
 * @brief Calculate the statistics related fields of a pingRecord struct
 * @param[in] record the pingRecord to work on
 * @return 0 on success
 */
int setStatistics(pingRecord * record);
