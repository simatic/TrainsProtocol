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

#include <stdlib.h>
#include <math.h>

#include "latencyData.h"

pingRecord newPingRecord(){
  pingRecord newPingRec;

  newPingRec.currentRecordsNb = 0;
  newPingRec.maxRecordsNb = 512;
  newPingRec.timevalRecords = malloc(
      newPingRec.maxRecordsNb * sizeof(struct timeval));
  newPingRec.mean = 0;
  newPingRec.variance = 0;
  newPingRec.standardDeviation = 0;
  newPingRec.min95confidenceInterval = 0;
  newPingRec.max95confidenceInterval = 0;
  newPingRec.min99confidenceInterval = 0;
  newPingRec.max99confidenceInterval = 0;
  newPingRec.floatRecords = NULL;

  return newPingRec;
}

void freePingRecord(pingRecord * record){

  if (record->timevalRecords != NULL ) {
    free(record->timevalRecords);
  }
  if (record->floatRecords != NULL ) {
    free(record->floatRecords);
  }

}

int recordValue(struct timeval value, pingRecord * pingRec){

  if (pingRec->currentRecordsNb == pingRec->maxRecordsNb) {
    pingRec->maxRecordsNb *= 2;
    pingRec->timevalRecords = realloc(pingRec->timevalRecords,
        pingRec->maxRecordsNb * sizeof(struct timeval));
  }

  pingRec->timevalRecords[pingRec->currentRecordsNb] = value;
  pingRec->currentRecordsNb++;

  return 0;
}

int setStatistics(pingRecord * record){

  int i;
  double calculatedMean = 0;
  double calculatedVariance = 0;

  record->floatRecords = malloc(record->currentRecordsNb * sizeof(double));

  //Data conversion from struct timeval to double
  //and calculation of the mean
  for (i = 0; i < record->currentRecordsNb; i++) {

    record->floatRecords[i] = ((double) record->timevalRecords[i].tv_sec) * 1000
        + ((double) record->timevalRecords[i].tv_usec) / 1000;

    calculatedMean += record->floatRecords[i];
  }
  calculatedMean /= record->currentRecordsNb;
  record->mean = calculatedMean;

  //Calculation of the variance
  for (i = 0; i < record->currentRecordsNb; i++) {

    calculatedVariance += ((record->floatRecords[i] - calculatedMean)
        * (record->floatRecords[i] - calculatedMean));

  }
  calculatedVariance /= record->currentRecordsNb;
  record->variance = calculatedVariance;

  //Calculation of the standard deviation
  record->standardDeviation = sqrt(record->variance);

  record->min95confidenceInterval = record->mean - 2 * record->standardDeviation;
  record->max95confidenceInterval = record->mean + 2 * record->standardDeviation;
  record->min99confidenceInterval = record->mean - 3 * record->standardDeviation;
  record->max99confidenceInterval = record->mean + 3 * record->standardDeviation;

  if (record->min99confidenceInterval < 0) {
    if (record->min95confidenceInterval < 0) {
      record->min95confidenceInterval = 0;
    }
    record->min99confidenceInterval;
  }

  return 0;
}
