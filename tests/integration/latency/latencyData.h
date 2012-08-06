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

pingRecord newPingRecord();

void freePingRecord(pingRecord * record);

int recordValue(struct timeval value, pingRecord * pingRec);

int setStatistics(pingRecord * record);
