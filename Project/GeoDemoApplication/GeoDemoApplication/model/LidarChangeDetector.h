#pragma once

/*
method:


1) 

For each of the two original data sets DSM (t1) and DSM (t2) a
segmentation of 3D object areas has to be performed. In order
to get exclusively 3D objects on the surface of the Earth - i.e. to
eliminate the influence of topography - a so-called normalized
DSM (nDSM) was determined for each data set. Therefore, in a
rough filtering process, points found to reliably lie on the
ground were extracted (e.g. Weidner & Förstner, 1995; Sithole
& Vosselman 2003; Vögtle & Steinle, 2003) which can be
interpolated to a digital terrain model (DTM). The resulting
nDSM for each date can be e calculated easily by subtracting this
DTM from the original DSM.

2)

A segmentation procedure is now applied to these nDSMs
which is based on an adapted region growing algorithm.
Because this application is focussed on buildings and their
changes, the parameters of this region growing algorithm are
chosen in such a manner that nearly all buildings are segmented
but only few vegetation objects. Taking into account that a
building has certain minimal dimensions, this process starts at a
point (crystallisation point) in which neighbourhood n points
(e.g. n=5 or n=9) exceed the minimal building height (e.g.
2.5m). Iteratively, new neighbouring points are added to this
area if their height difference does not exceed a maximum
acceptable value - defined for instance by the maximal slope of
roof planes which may occur in urban areas, e.g. 1.2m
(homogeneity criterion). Therefore, region growing
automatically stops at object boundaries of buildings and higher
vegetation objects.

3)
Now, these classification results are used to exclude all nonbuilding
objects.

4)
For the project which is presented in this paper, the following
five classes were defined:
• not-altered buildings
• added-on buildings
• reduced buildings
• new buildings
• demolished buildings

5)
The approach developed in this project is automatic, exclusively
based on height data and object-based, i.e. it analyses those
objects extracted by segmentation and classification approach
which was described in chapter 3. The change detection is
carried out in three main steps:
	1) extraction of buildings that can not be evaluated
	2) extraction and further analysis of objects that do not
	sufficiently match with the objects at the other date to
	identify new, modified and demolished buildings
	3) classification of the remaining objects into not-altered,
	added-on and reduced buildings


6)
In a first step all building segments derived from
laser scanning data were consecutively examined with regard to
included no-data pixels in the according laser scanning DSM
and were filtered out if a sufficient number of such pixels
occurred. Here, the threshold was set to 4, assuming that nodata
pixels may occur at elements on a roof, e.g. chimneys, but
it can not be tolerated if they cover large parts of a building; on
the other hand small buildings can consist of only 10 pixel (e.g.
2m x 5m ground plan), therefore, this value should not be
exceeded.

7)
In a next step, the overlaps of the extracted building areas are
examined. Based on the segments received from the DSM of the
earlier date ( DSM(t1) ), all pixels of a segment are analysed if
they are part of a segment derived from the newer DSM
( DSM(t2) ), too. Only if a segment of t1 is found which has a
certain percentage (e.g. 25%) of its ground plan area
overlapping with segments of t2, it is kept for a further analysis.
Otherwise, at all pixels the differences in between the both
DSMs are checked. If a significant number of pixels - here the
inverse percentage of the overlapping threshold is used (75%) -
exceed a deviation tolerance value, which is introduced due to
measurement accuracy and systematic deviations on slant
planes, the segment is assumed to represent a demolished or
teared off building. Otherwise the segment is also regarded for
further analysis. The same procedure is carried out also on the
segmentation result of the newer DSM and accordingly new
buildings are found.

8)
The segments not yet classified in this process are assumed to
be either unchanged or modified. Inside the ground plans of the
remaining segments, i.e. those segments not yet classified as
not-analysable, missing or new built, the according parts of the
DSMs are extracted and the elder data is subtracted from the
newer one. The resulting height difference image is filtered
using the opening operator of mathematical morphology, i.e. an
erosion with a consecutive dilatation is performed (e.g. Haralick
& Shapiro, 1992), regarding occurring height differences as
foreground values. The purpose of applying the opening
methodology is to remove single, isolated pixels or small group
of pixels with height differences. This is because they often
represent small objects on the roofs tops, e.g. chimneys or small
dormers, or differences occurred due to arbitrary measurement
deviations.
The remaining (groups of) pixels are then classified according
to the type of height differences. Three classes are used:
• height difference inside the tolerance range
• positive height difference (heightened)
• negative height difference (decreased)

Only segments containing pixels classified into the last two
classes are further analysed, the others are considered
unchanged.
For each segment, as well for those linked to t1 as those of t2
,
the dominating difference type is determined. If both classes
occur in almost the same amount inside the segment, it is not
classified; then only the altered parts are classified accordingly,
i.e. such segments contain different classified parts. In all other
cases, the whole segment is classified according to the
dominating differences type, i.e. either as added-on or reduced
building




*/

using namespace cv;
using namespace std;

#include <set>
#include <vector>
#include <list>

class Segment {
	Mat mask;

};

namespace LidarChangeDetector {
	void DoEverything();

	void SegmentImage(Mat input, list<Segment>& segments);
}

//class LidarChangeDetector {
//
//};

