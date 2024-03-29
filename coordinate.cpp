/*
 *  This file is a part of KNOSSOS.
 *
 *  (C) Copyright 2007-2013
 *  Max-Planck-Gesellschaft zur Foerderung der Wissenschaften e.V.
 *
 *  KNOSSOS is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License version 2 of
 *  the License as published by the Free Software Foundation.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

/*
 * For further information, visit http://www.knossostool.org or contact
 *     Joergen.Kornfeld@mpimf-heidelberg.mpg.de or
 *     Fabian.Svara@mpimf-heidelberg.mpg.de
 */

#include "knossos-global.h"

extern stateInfo *state;


/**
 * This function calculates the coordinates of the datacube from pixel coordinates
 */
Coordinate Coordinate::Px2DcCoord(Coordinate pxCoordinate) {
    Coordinate dcCoordinate;

    // Rounding should be explicit.
    dcCoordinate.x = pxCoordinate.x / state->cubeEdgeLength;
    dcCoordinate.y = pxCoordinate.y / state->cubeEdgeLength;
    dcCoordinate.z = pxCoordinate.z / state->cubeEdgeLength;

    return dcCoordinate;
}

bool Coordinate::transCoordinate(Coordinate *outCoordinate,
                                 int x,
                                 int y,
                                 int z,
                                 floatCoordinate scale, Coordinate offset) {

    /*
     *  Translate a pixel coordinate (x, y, z) relative to a dataset
     *  with scale scale and offset offset to pixel coordinate outCoordinate
     *  relative to the current dataset.
     */

    outCoordinate->x = ((float)x * scale.x
                        + (float)offset.x
                        - (float)state->offset.x)
                        / (float)state->scale.x;
    outCoordinate->y = ((float)y * scale.y
                        + (float)offset.y
                        - (float)state->offset.y)
                        / (float)state->scale.y;
    outCoordinate->z = ((float)z * scale.z
                        + (float)offset.z
                        - (float)state->offset.z)
                        / (float)state->scale.z;

    return true;
}

/** This method parses a coordinate string seperated with delimeters [],(),/,;,- */
Coordinate *Coordinate::parseRawCoordinateString(char *string) {
    Coordinate *extractedCoords = NULL;
    char coordStr[strlen(string)];
    strcpy(coordStr, string);
    char delims[] = "[]()./,; -";
    char* result = NULL;
    char* coords[3];
    int i = 0;

    if(!(extractedCoords = (Coordinate *)malloc(sizeof(Coordinate)))) {
        LOG("Out of memory")
        _Exit(false);
    }

    result = strtok(coordStr, delims);
    while(result != NULL && i < 4) {
        coords[i] = (char *)malloc(strlen(result)+1);
        strcpy(coords[i], result);
        result = strtok(NULL, delims);
        ++i;
    }

    if(i < 2) {
        LOG("Paste string doesn't contain enough delimiter-separated elements")
        goto fail;
    }

    if((extractedCoords->x = atoi(coords[0])) < 0) {
        LOG("Error converting paste string to coordinate")
        goto fail;
    }
    if((extractedCoords->y = atoi(coords[1])) < 0) {
        LOG("Error converting paste string to coordinate")
        goto fail;
    }
    if((extractedCoords->z = atoi(coords[2])) < 0) {
        LOG("Error converting paste string to coordinate")
        goto fail;
    }

    return extractedCoords;

fail:
    free(extractedCoords);
    return NULL;

}

/** test with overloadable operator, maybe obsolet */
void Coordinate::operator=(Coordinate const &rhs) {
    x = rhs.x;
    y = rhs.y;
    z = rhs.z;

}

