/*
 * ESPFramework
 * Copyright (c) 2021-2026 Magnus
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 *
 */
#ifndef SRC_UTILS_HPP_
#define SRC_UTILS_HPP_

float convertCtoF(float c);
float convertFtoC(float f);
float convertKGtoLBS(float kg);
float convertLBStoKG(float lbs);
float convertCLtoUSOZ(float cl);
float convertCLtoUKOZ(float cl);
float convertUSOZtoCL(float usoz);
float convertUKOZtoCL(float ukoz);
double convertToPlato(double sg);
double convertToSG(double plato);

float reduceFloatPrecision(float f, int dec);
char* convertFloatToString(float f, char* buf, int dec = 2);

void tcp_cleanup();
void deepSleep(int t);

void printHeap(String prefix);

void forcedReset();

void checkResetReason();

String urldecode(String str);
String urlencode(String str);

#endif  // SRC_UTILS_HPP_

// EOF
