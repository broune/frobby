/* Frobby: Software for monomial ideal computations.
   Copyright (C) 2011 Bjarke Hammersholt Roune (www.broune.com)

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program.  If not, see http://www.gnu.org/licenses/.
*/
#include "stdinc.h"
#include "LatticeAlgs.h"

char getPlaceCode(NeighborPlace place) {
  switch (place) {
  case InPlane: return 'P';
  case UnderPlane: return 'U';
  case OverPlane: return 'O';
  case NoPlace: return ' ';
  default: return 'E';
  }
}

size_t pushOutFacetPositive(size_t facetPushOut,
							const vector<mpz_class>& rhs,
							const GrobLat& lat) {
  size_t onFacet = numeric_limits<size_t>::max();
  mpq_class leastEntry;

  for (size_t n = 0; n < lat.getNeighborCount(); ++n) {
	for (size_t i = 0; i < lat.getYDim(); ++i) {
	  if (i == facetPushOut)
		continue;
	  if (lat.getYMatrix()(n, i) >= rhs[i])
		goto notOnFacet;
	}

	if (onFacet == numeric_limits<size_t>::max() ||
		leastEntry > lat.getYMatrix()(n, facetPushOut)) {
	  leastEntry = lat.getYMatrix()(n, facetPushOut);
	  onFacet = n;
	}

  notOnFacet:;
  }
  return onFacet;
}

size_t pushOutFacetZero(const vector<mpz_class>& rhs, const GrobLat& lat) {
  size_t onFacet = numeric_limits<size_t>::max();
  mpq_class leastEntry;

  for (size_t n = 0; n < lat.getNeighborCount(); ++n) {
	for (size_t i = 1; i < lat.getYDim(); ++i)
	  if (-lat.getYMatrix()(n, i) >= rhs[i])
		goto notOnFacet;

	if (onFacet == numeric_limits<size_t>::max() ||
		leastEntry > -lat.getYMatrix()(n, 0)) {
	  leastEntry = -lat.getYMatrix()(n, 0);
	  onFacet = n;
	}

  notOnFacet:;
  }
  return onFacet;
}

void Mlfb::reset(size_t offset, const vector<Neighbor>& points) {
  ASSERT(!points.empty());
  _points = points;
  _offset = offset;

  const GrobLat& lat = getPoint(0).getGrobLat();

  _rhs.resize(lat.getYDim());
  for (size_t var = 0; var < lat.getYDim(); ++var) {
	_rhs[var] = points[0].getY(var);
	for (size_t p = 1; p < points.size(); ++p)
	  if (_rhs[var] < points[p].getY(var))
		_rhs[var] = points[p].getY(var);
  }
	  
  // order to have maxima along diagonal if possible.
  if (getPointCount() == lat.getYDim()) {
	for (size_t i = 0; i < lat.getYDim(); ++i)
	  for (size_t p = 0; p < getPointCount(); ++p)
		if (getPoint(p).getY(i) == getRhs()[i])
		  swap(_points[i], _points[p]);
  }

  // Compute MLFB index.
  if (getPointCount() - 1 == lat.getHDim())
  {
	Matrix mat(getPointCount() - 1, lat.getHDim());
	for (size_t point = 1; point < getPointCount(); ++point)
	  for (size_t var = 0; var < lat.getHDim(); ++var)
		mat(point - 1, var) = getPoint(point).getH(var);
	index = determinant(mat);
  }

  if (getPointCount() == 4) {
	Matrix mat(4, lat.getHDim());
	for (size_t point = 0; point < getPointCount(); ++point)
	  for (size_t var = 0; var < lat.getHDim(); ++var)
		mat(point, var) = getPoint(point).getH(var);
	_isParallelogram = ::isParallelogram(mat);
  } else
	_isParallelogram = false;
}

void computeMlfbs(vector<Mlfb>& mlfbs, const GrobLat& lat) {
  BigIdeal initialIdeal;
  lat.getInitialIdeal(initialIdeal);

  BigTermRecorder recorder;
  SliceParams params;
  SliceFacade facade(params, initialIdeal, recorder);
  facade.computeIrreducibleDecomposition(true);
  auto_ptr<BigIdeal> rhsesOwner = recorder.releaseIdeal();
  BigIdeal& rhses = *rhsesOwner;
  ASSERT(recorder.empty());

  mlfbs.clear();
  mlfbs.resize(rhses.getGeneratorCount());

  vector<Neighbor> points;
  for (size_t i = 0; i < mlfbs.size(); ++i) {
	Mlfb& mlfb = mlfbs[i];
	points.clear();

	points.push_back(Neighbor(lat));
	for (size_t gen = 0; gen < initialIdeal.getGeneratorCount(); ++gen) {
	  for (size_t var = 0; var < initialIdeal.getVarCount(); ++var)
		if (initialIdeal[gen][var] > rhses[i][var])
		  goto skipIt;
	  points.push_back(Neighbor(lat, gen));
	skipIt:;
	}

	mlfb.reset(i, points);
	CHECK(rhses[i] == mlfb.getRhs());
  }

  Matrix nullSpaceBasis;
  nullSpace(nullSpaceBasis, lat.getMatrix());
  transpose(nullSpaceBasis, nullSpaceBasis);
  // the basis is the rows of NullSpaceBasis at this point.

  for (size_t m = 0; m < mlfbs.size(); ++m) {
	Mlfb& mlfb = mlfbs[m];

	if (mlfb.getPointCount() != lat.getYDim())
	  continue;

	// Compute minInitialFacet.
	mlfb.minInitialFacet = 0;
	mpq_class minInitial = 0;
	for (size_t i = 0; i < mlfb.getPointCount(); ++i) {
	  mpq_class initial = mlfb.getPoint(i).getY(0);
	  if (minInitial > initial) {
		minInitial = initial;
		mlfb.minInitialFacet = i;
	  }
	}

	// Compute dot degree.
	if (nullSpaceBasis.getRowCount() == 1 &&
		lat.getYDim() == nullSpaceBasis.getColCount()) {
	  mlfb.dotDegree = 0;
	  for (size_t r = 0; r < nullSpaceBasis.getRowCount(); ++r)
		for (size_t c = 0; c < nullSpaceBasis.getColCount(); ++c)
		  mlfb.dotDegree += nullSpaceBasis(r, c) * mlfb.getRhs()[c];
	}

	// Compute Scarf edges.
	mlfb.edges.resize(lat.getYDim());
	mlfb.edgeHitsFacet.resize(lat.getYDim());
	for (size_t facetPushIn = 0; facetPushIn < lat.getYDim(); ++facetPushIn) {
	  mpq_class secondLargest;
	  size_t facetPushOut = numeric_limits<size_t>::max();

	  for (size_t neigh = 0; neigh < lat.getYDim(); ++neigh) {
		mpq_class entry = 0; // neigh == 0 represents zero.
		if (neigh > 0)
		  entry = mlfb.getPoint(neigh).getY(facetPushIn);

		if (neigh == facetPushIn) {
		  if (entry != mlfb.getRhs()[facetPushIn])
			goto skipBecauseNotGeneric;
		} else {
		  if (entry == secondLargest &&
			  facetPushOut != numeric_limits<size_t>::max())
			goto skipBecauseNotGeneric;

		  if (entry > secondLargest ||
			  facetPushOut == numeric_limits<size_t>::max()) {
			secondLargest = entry;
			facetPushOut = neigh;
		  }
		}
	  }

	  // ----------------------------------------------
	  // ** Case 1: facetPushIn > 0 and facetPushOut > 0
	  //
	  // We push in facetPushIn (discarding the non-zero neighbor
	  // previously on that facet) and hit a non-zero neighbor
	  // that is already on the MLFB. That neighbor now instead
	  // lies on facetPushIn and we push out facetPushOut in the
	  // straight forward way until it hits what will be the new
	  // neighbor on facetPushOut.
	  //
	  // ----------------------------------------------
	  // ** Case  2: facetPushIn > 0 and facetPushOut == 0
	  //
	  // We push in facetPushIn (discarding the non-zero neighbor
	  // previously on that facet) and hit zero. Zero now instead
	  // lies on facetPushIn and we push out facetPushOut. It will
	  // be pushing into the area on the opposite side from the
	  // half-set of neighbors that we are looking at, so to find
	  // the replacement neighbor to put on facetPushOut
	  // (i.e. facet zero) we need to consider the negative of the
	  // neighbors we have. When that neighbor -v has been found,
	  // we need to translate the whole body by +v so that zero
	  // will once again lie on facet zero.
	  //
	  // ----------------------------------------------
	  // ** case 3: facetPushIn == 0 and facetPushOut > 0
	  //
	  // We push in facetPushIn (discarding the zero previously on
	  // that facet) and hit a non-zero neighbor v on
	  // facetPushOut. Then v lies on facetPushIn (i.e. facet
	  // zero), so for the MLFB to have zero on facet 0 we need to
	  // translate it by -v. We then relax facetPushOut to find a
	  // replacement neighbor as in Case 1.

	  ASSERT(facetPushIn == 0 || secondLargest >= 0);
	  // In all cases the neighbor we hit moves to facetPushIn.
	  vector<mpz_class> rhs(mlfb.getRhs());

	  rhs[facetPushIn] = secondLargest;

	  if (facetPushIn == 0) {
		// Case 3: the neighbor hit moves to facet 0 so translate
		// the body to make that neighbor zero.
		for (size_t i = 0; i < lat.getYDim(); ++i)
		  rhs[i] -= mlfb.getPoint(facetPushOut).getY(i);
	  }

	  if (facetPushOut > 0) {
		// Case 1 or 3: push out in the usual way.
		size_t newNeighbor = pushOutFacetPositive(facetPushOut, rhs, lat);
		if (newNeighbor == numeric_limits<size_t>::max())
		  goto skipBecauseNotGeneric;
		rhs[facetPushOut] = lat.getYMatrix()(newNeighbor, facetPushOut);
	  }

	  if (facetPushOut == 0) {
		// Case 2: push out into negative neighbors
		size_t newNeighbor = pushOutFacetZero(rhs, lat);
		if (newNeighbor == numeric_limits<size_t>::max())
		  goto skipBecauseNotGeneric;
		for (size_t i = 0; i < lat.getYDim(); ++i)
		  rhs[i] += lat.getYMatrix()(newNeighbor, i);
		rhs[0] = 0;
	  }

	  // Find the MLFB with the right hand side that we have
	  // computed.
	  for (size_t m = 0; m < mlfbs.size(); ++m) {
		if (mlfbs[m].getRhs() == rhs) {
		  mlfb.edges[facetPushIn] = &(mlfbs[m]);
		  mlfb.edgeHitsFacet[facetPushIn] = facetPushOut;
		  goto foundMatch;
		}
	  }
	  goto skipBecauseNotGeneric;
	foundMatch:;
	}
	continue;
  skipBecauseNotGeneric:
	mlfb.edges.clear();
	mlfb.edgeHitsFacet.clear();
  }
}

SeqPos nextInSeq(SeqPos pos) {
  size_t pushIn;
  for (pushIn = 0;; ++pushIn) {
	ASSERT(pushIn < 4);
	if (pushIn != pos.fixFacet1 &&
		pushIn != pos.fixFacet2 &&
		pushIn != pos.comingFromFacet)
	  break;
  }

  size_t hits = pos.mlfb->getHitsFacet(pushIn);
  ASSERT(hits != pushIn);

  SeqPos next = pos;
  next.mlfb = pos.mlfb->getEdge(pushIn);
  next.comingFromFacet = hits;

  if (pos.fixFacet1 == hits)
	next.fixFacet1 = pushIn;
  else if (pos.fixFacet2 == hits)
	next.fixFacet2 = pushIn;

  next.order();
  return next;
}

size_t computeFlatIntervalCount(const vector<SeqPos>& flatSeq) {
  if (flatSeq.empty())
	return 0u;

  const Mlfb& leftFlat = *(flatSeq.front().mlfb);
  size_t sumFacet = leftFlat.getMinInitialFacet();

  size_t aFacet = 4;
  for (size_t j = 0; j < 4; ++j) {
	if (j != 0 && j != sumFacet) {
	  aFacet = j;
	  break;
	}
  }
  CHECK(aFacet != 4);

  size_t subSeqCount = 1;
  for (size_t i = 1; i < flatSeq.size() - 1; ++i) {
	const Mlfb& prev = *(flatSeq[i - 1].mlfb);
	const Mlfb& flat = *(flatSeq[i].mlfb);
	if (flat.getHitsFacet(aFacet) != prev.getHitsFacet(aFacet))
	  ++subSeqCount;
  }
  return subSeqCount;
}

void computeFlatSeq(vector<SeqPos>& seq,
					const vector<Mlfb>& mlfbs,
					const Plane& plane) {
  // ** compute left most flat
  const Mlfb* leftFlat = 0;
  for (size_t m = 0; m < mlfbs.size(); ++m) {
	if (!plane.isFlat(mlfbs[m]))
	  continue;
	const Mlfb* toLeft = mlfbs[m].getEdge(0);
	if (!plane.isFlat(*toLeft)) {
	  CHECK(leftFlat == 0 || leftFlat == toLeft); // left flat unique
	  leftFlat = &(mlfbs[m]);
	}
  }

  seq.clear();
  if (leftFlat == 0) {
	ASSERT(!plane.hasFlat());
	return;
  }

  // ** go right as long as there is a flat there
  SeqPos pos;
  pos.mlfb = leftFlat;
  while (plane.isFlat(*pos.mlfb)) {
	ASSERT(seq.empty() || seq.back().mlfb != pos.mlfb);
	seq.push_back(pos);
	bool moved = false;
	for (size_t facet = 1; facet < 4; ++facet) {
	  if (pos.mlfb->getEdge(facet)->getEdge(0) == pos.mlfb) {
		pos.mlfb = pos.mlfb->getEdge(facet);
		moved = true;
		break;
	  }
	}
	if (!moved)
	  break;
  }
}  

void computePlanes(vector<Plane>& planes,
				   const GrobLat& lat,
				   vector<Mlfb>& mlfbs) {
  const SatBinomIdeal& ideal = lat.getIdeal();

  size_t varCount = lat.getYDim();
  size_t neighborCount = lat.getNeighborCount();
  vector<mpz_class> sum(varCount);
  for (size_t gen1 = 0; gen1 < neighborCount; ++gen1) {
	for (size_t gen2 = gen1 + 1; gen2 < neighborCount; ++gen2) {
	  const vector<mpz_class>& g1 = ideal.getGenerator(gen1);
	  const vector<mpz_class>& g2 = ideal.getGenerator(gen2);

	  // Set sum = g1 + g2.
	  for (size_t var = 0; var < varCount; ++var)
		sum[var] = g1[var] + g2[var];

	  if (ideal.isPointFreeBody(g1, sum) && ideal.isPointFreeBody(g2, sum)) {
		Tri tri;
		tri.a = lat.getNeighbor(gen1);
		tri.b = lat.getNeighbor(gen2);
		tri.fromFlat = ideal.isPointFreeBody(g1, g2, sum);
		Matrix rowAB(2, lat.getHDim());
		copyRow(rowAB, 0, lat.getHMatrix(), gen1);
		copyRow(rowAB, 1, lat.getHMatrix(), gen2);

		for (size_t plane = 0; plane < planes.size(); ++plane) {
		  if (hasSameRowSpace(rowAB, planes[plane].rowAB)) {
			if (!tri.fromFlat)
			  planes[plane].nonMlfbTris.push_back(tri);
			goto done;
		  }
		}

		{
		  planes.resize(planes.size() + 1);
		  Plane& plane = planes.back();
		  plane.rowAB = rowAB;
		  nullSpace(plane.nullSpaceBasis, rowAB);
		  if (!tri.fromFlat)
			plane.nonMlfbTris.push_back(tri);

		  Matrix prod;
		  product(prod, lat.getHMatrix(), plane.nullSpaceBasis);
		  plane.neighborPlace.resize(lat.getNeighborCount());
		  for (size_t gen = 0; gen < lat.getNeighborCount(); ++gen) {
			mpq_class& value = prod(gen, 0);
			NeighborPlace place = InPlane;
			if (value < 0)
			  place = UnderPlane;
			else if (value > 0)
			  place = OverPlane;
			plane.neighborPlace[gen] = place;
		  }

		  transpose(plane.nullSpaceBasis);
		}
	  done:;
	  }
	}
  }

  for (size_t p = 0; p < planes.size(); ++p) {
	Plane& plane = planes[p];
	for (size_t i = 0; i < mlfbs.size(); ++i)
	  plane.typeCounts[plane.getType(mlfbs[i])] += 1;

	computeFlatSeq(plane.flatSeq, mlfbs, plane);
	computePivots(plane.pivots, mlfbs, plane, plane.flatSeq);
	plane.flatIntervalCount = computeFlatIntervalCount(plane.flatSeq);
  }
}

void check0Graph(const vector<Mlfb>& mlfbs) {
  vector<bool> ok(mlfbs.size());
  bool sawFlat = false;
  for (size_t i =0 ; i < mlfbs.size(); ++i) {
	ok[i] = (mlfbs[i].index == 0);
	if (ok[i])
	  sawFlat = true;
  }
  if (!sawFlat)
	return;

  while (true) {
	bool done = true;
	for (size_t i = 0; i < mlfbs.size(); ++i) {
	  if (!ok[i]) {
		size_t to = mlfbs[i].getEdge(0)->getOffset();
		if (ok[to]) {
		  done = false;
		  ok[i] = true;
		}
	  }
	}
	if (done)
	  break;
  }

  for (size_t i = 0; i < mlfbs.size(); ++i) {
	CHECK(ok[i]);
  }
}

void checkMlfbs(const vector<Mlfb>& mlfbs, const GrobLat& lat) {
  CHECK(mlfbs.size() == lat.getNeighborCount() - 1);
  for (size_t m = 0; m < mlfbs.size(); ++m) {
	CHECK(mlfbs[m].isParallelogram() == (mlfbs[m].index == 0));
  }
}

void checkPlanes(const vector<Plane>& planes,
				 const GrobLat& lat,
				 const vector<Mlfb>& mlfbs) {
  bool multipleIntervals = false;
  bool anyFlat = false;
  bool flatWith4Pivots = false;
  for (size_t p = 0; p < planes.size(); ++p) {
	if (planes[p].flatIntervalCount > 1)
	  multipleIntervals = true;
	if (planes[p].hasFlat()) {
	  anyFlat = true;
	  if (planes[p].pivots.size() == 4)
		flatWith4Pivots = true;
	}
  }
  if (multipleIntervals) {
	ASSERT(anyFlat);
	//CHECK(flatWith4Pivots);
	CHECK(planes.size() == 1);
  }

  if (planes.size() == 6) {
	CHECK(!anyFlat);
	CHECK(planes.size() == 6);
	for (size_t p = 0; p < planes.size(); ++p) {
	  CHECK(planes[p].pivots.size() == 4);
	}
	CHECK(lat.getNeighborCount() == 7);
	CHECK(mlfbs.size() == 6);
  }

  if (anyFlat) {
	//check0Graph(mlfbs);
	//CHECK(flatWith4Pivots);
	CHECK(planes.size() < 6);
  }
}

void checkPlane(const Plane& plane, const vector<Mlfb>& mlfbs) {
  for (size_t i = 0; i < mlfbs.size(); ++i) {
	if (plane.isPivot(mlfbs[i])) {
	  CHECK(mlfbs[i].index == -1 || mlfbs[i].index == 1);
	} else if (plane.isFlat(mlfbs[i])) {
	  CHECK(mlfbs[i].index == 0);
	}
  }
}

/** Returns the facet to push in of pivot to get to a flat. Pivot must
	be a pivot. */
size_t pivotToFlatFacet(const Mlfb& pivot, const Plane& plane) {
  size_t facet = 4;
  for (size_t push = 0; push < 4; ++push) {
	if (plane.isFlat(*pivot.getEdge(push))) {
	  CHECK(facet == 4); // adjacent to no more than one flat
	  facet = push;
	}
  }
  CHECK(facet != 4); // adjacent to at least one flat
  return facet;
}

bool disjointSeqs(const vector<SeqPos>& a, const vector<SeqPos>& b) {
  for (size_t i = 0; i < a.size(); ++i)
	for (size_t j = 0; j < b.size(); ++j)
	  if (a[i].mlfb == b[j].mlfb)
		return false;
  return true;
}

void computePivots(vector<const Mlfb*>& pivots,
				   const vector<Mlfb>& mlfbs,
				   const Plane& plane,
				   const vector<SeqPos>& flatSeq) {
  pivots.clear();
  for (size_t m = 0; m < mlfbs.size(); ++m)
	if (plane.isPivot(mlfbs[m]))
	  pivots.push_back(&(mlfbs[m]));
  if (pivots.size() != 4 || flatSeq.empty())
	return; // no idea about proper order in this case

  pivots.clear();
  // use flat sequence to impose correct order
  size_t sumFacet = flatSeq.front().mlfb->getMinInitialFacet();
  pivots.push_back(flatSeq.front().mlfb->getEdge(0));
  pivots.push_back(flatSeq.front().mlfb->getEdge(sumFacet));

  sumFacet = flatSeq.back().mlfb->getMinInitialFacet();
  for (size_t i = 0; i < 4; ++i)
	if (i != 0 && i != sumFacet)
	  pivots.push_back(flatSeq.back().mlfb->getEdge(i));
}

void computePivotSeqs(vector<vector<SeqPos> >& seqs,
					  const Mlfb& pivot,
					  const Plane& plane) {
  ASSERT(plane.pivots.size() == 4);
  ASSERT(plane.isPivot(pivot));
  size_t flatFacet = pivotToFlatFacet(pivot, plane);

  seqs.clear();
  for (size_t facet = 0; facet < 4; ++facet) {
	if (facet == flatFacet)
	  continue;
	seqs.resize(seqs.size() + 1);
	vector<SeqPos>& seq = seqs.back();
	
	SeqPos pos(&pivot, facet, flatFacet);
	seq.push_back(pos);
	do {
	  pos = nextInSeq(pos);
	  seq.push_back(pos);
	} while (!(plane.isPivot(*pos.mlfb)));	  
  }
}

void checkPivotSeqs(vector<vector<SeqPos> >& pivotSeqs,
					const Plane& plane,
					const vector<Mlfb>& mlfbs,
					const vector<SeqPos>& flatSeq) {
  CHECK(pivotSeqs.size() == 3);
  CHECK(pivotSeqs[0].size() >= 2);
  const Mlfb* pivot1 = pivotSeqs[0].front().mlfb;
  const Mlfb* pivot2 = pivotSeqs[0].back().mlfb;

  CHECK(plane.isPivot(*pivot1));
  CHECK(plane.isPivot(*pivot2));


  bool foundPlace = false;
  NeighborPlace place;
  for (size_t i = 0; i < 3; ++i) {
	CHECK(pivotSeqs[i].size() >= 2);
	// ** all sequences on same side between same pivots
	CHECK((pivotSeqs[i].front().mlfb == pivot1 &&
		   pivotSeqs[i].back().mlfb == pivot2) ||
		  (pivotSeqs[i].front().mlfb == pivot2 &&
		   pivotSeqs[i].back().mlfb == pivot1));

	for (size_t j = 1; j < pivotSeqs[i].size() - 1; ++j) {
	  const Mlfb* prev = pivotSeqs[i][j - 1].mlfb;
	  const Mlfb* current = pivotSeqs[i][j].mlfb;
	  const Mlfb* next = pivotSeqs[i][j + 1].mlfb;

	  // ** middle mlfbs are 2-2's
	  CHECK(plane.getType(*current) == 2);

	  // ** SeqPos agrees with sequence order
	  const SeqPos& pos = pivotSeqs[i][j];
	  size_t prevFacet = pos.getBackFacet();
	  size_t nextFacet = pos.getForwardFacet();
	  CHECK(current->getEdge(prevFacet) == prev);
	  CHECK(current->getEdge(nextFacet) == next);

	  // ** in-coming and out-going facets are on same plane
	  CHECK(plane.getPlace(current->getPoint(prevFacet)) ==
			plane.getPlace(current->getPoint(nextFacet)));
	  
	  // ** active facets are the same for all sequences on same side
	  if (!foundPlace) {
		place = plane.getPlace(current->getPoint(prevFacet));
	  }
	  CHECK(place == plane.getPlace(current->getPoint(prevFacet)));
	}
  }

  // ** Check that the sequences are a partition of the set of 2-2 mlfbs
  vector<bool> seen(mlfbs.size());
  for (size_t i = 0; i < 3; ++i) {
	for (size_t j = 1; j < pivotSeqs[i].size() - 1; ++j) {
	  CHECK(!seen[pivotSeqs[i][j].mlfb->getOffset()]); // sequences must be disjoint
	  seen[pivotSeqs[i][j].mlfb->getOffset()] = true;
	}
  }
  for (size_t i = 0; i < mlfbs.size(); ++i) {
	if (plane.isPivot(mlfbs[i]) || plane.isFlat(mlfbs[i]))
	  continue;
	CHECK(seen[i]); // all 2-2s in some sequence
  }
}

void computeNonSums(vector<Neighbor>& nonSums, const GrobLat& lat) {
  vector<bool> isSum(lat.getNeighborCount());
  for (size_t i = 0; i < lat.getNeighborCount(); ++i) {
	for (size_t j = 0; j < i; ++j) {
	  Neighbor sum = lat.getSum(lat.getNeighbor(i), lat.getNeighbor(j));
	  if (sum.isValid())
		isSum[sum.getRow()] = true;
	}
  }

  nonSums.clear();
  for (size_t i = 0; i < isSum.size(); ++i)
	if (!isSum[i])
	  nonSums.push_back(lat.getNeighbor(i));
}

void checkNonSums(vector<Neighbor>& nonSums, const GrobLat& lat) {
  CHECK(nonSums.size() == 3 || nonSums.size() == 4);
  if (nonSums.size() == 3) {
	Matrix mat(3, 3);
	for (size_t ns = 0; ns < 3; ++ns)
	  for (size_t var = 0; var < 3; ++var)
		mat(ns, var) = nonSums[ns].getH(var);
	mpq_class det = determinant(mat);
	CHECK(det == 1 || det == -1);
  } else {
	Matrix mat(4, 3);
	for (size_t ns = 0; ns < 4; ++ns)
	  for (size_t var = 0; var < 3; ++var)
		mat(ns, var) = nonSums[ns].getY(var);
	cout << mat << endl;
	CHECK(isParallelogram(mat));

	return; // not checking this as it seems to not be true
	mpq_class areaSq = getParallelogramAreaSq(mat);
	cout << areaSq << endl;
	CHECK(areaSq == 1);
  }
}

void checkFlatSeq(const vector<SeqPos>& flatSeq,
				  const GrobLat& lat,
				  const Plane& plane) {
  if (flatSeq.empty())
	return;
  size_t sumf = flatSeq.front().mlfb->getMinInitialFacet();
  CHECK(sumf != 0);

  size_t af = 4;
  for (size_t j = 0; j < 4; ++j) {
	if (j != 0 && j != sumf) {
	  af = j;
	  break;
	}
  }

  size_t bf = 4;
  for (size_t j = 0; j < 4; ++j) {
	if (j != 0 && j != sumf && j != af) {
	  bf = j;
	  break;
	}
  }

  for (size_t i = 0; i < flatSeq.size(); ++i) {
	const Mlfb& flat = *(flatSeq[i].mlfb);
	Neighbor sum = flat.getPoint(sumf);
	Neighbor a = flat.getPoint(af);
	Neighbor b = flat.getPoint(bf);

	CHECK(sumf == flat.getMinInitialFacet());
	CHECK(lat.getSum(a, b) == sum);

	// right-going edges
	if (i < flatSeq.size() - 1) {
	  // not at right end	
	  CHECK(flat.getEdge(af) == flat.getEdge(bf));
	  const Mlfb& next = *(flatSeq[i + 1].mlfb);
	  CHECK(flat.getEdge(af) == &next);
	  if (flat.getHitsFacet(af) == sumf) {
		CHECK(flat.getHitsFacet(bf) == 0);
		CHECK(next.hasPoint(b));
		CHECK(next.hasPoint(sum));
		CHECK(next.hasPoint(lat.getSum(sum, b)));
	  } else {
		CHECK(flat.getHitsFacet(af) == 0);
		CHECK(flat.getHitsFacet(bf) == sumf);
		CHECK(next.hasPoint(a));
		CHECK(next.hasPoint(sum));
		CHECK(next.hasPoint(lat.getSum(sum, a)));
	  }
	} else {
	  // at right end
	  CHECK(plane.isPivot(*flat.getEdge(af)));
	  CHECK(plane.isPivot(*flat.getEdge(bf)));
	  CHECK(flat.getEdge(af) != flat.getEdge(bf));
	}	  

	// left-going edges
	if (i > 0) {
	  // not at left end
	  CHECK(flat.getEdge(0) == flat.getEdge(sumf));
	  const Mlfb& prev = *(flatSeq[i - 1].mlfb);
	  CHECK(flat.getEdge(0) == &prev);
	  if (flat.getHitsFacet(0) == af) {
		CHECK(flat.getHitsFacet(sumf) == bf);
	  } else {
		CHECK(flat.getHitsFacet(0) == bf);
		CHECK(flat.getHitsFacet(sumf) == af);
	  }
	} else {
	  // at left end
	  CHECK(plane.isPivot(*flat.getEdge(0)));
	  CHECK(plane.isPivot(*flat.getEdge(sumf)));
	  CHECK(flat.getEdge(0) != flat.getEdge(sumf));
	}
  }
}

void checkNonMlfbTris(const GrobLat& lat,
					  const vector<Mlfb>& mlfbs,
					  const vector<const Mlfb*>& pivots,
					  const vector<Tri>& nonMlfbTris,
					  const Plane& plane) {
  CHECK(nonMlfbTris.size() == 1);
  const Tri& tri = nonMlfbTris.back();
  Neighbor a = tri.a;
  Neighbor b = tri.b;
  Neighbor sumAB = lat.getSum(a, b);

  // ** tri is not a flat
  for (size_t i = 0; i < mlfbs.size(); ++i) {
	if (plane.isFlat(mlfbs[i])) {
	  CHECK(!mlfbs[i].hasPoint(a) ||
			!mlfbs[i].hasPoint(b) ||
			!mlfbs[i].hasPoint(sumAB));
	}
  }

  // ** find unique non-flat MLFB with {0,a,a+b}
  const Mlfb* mlfbA = 0;
  for (size_t i = 0; i < mlfbs.size(); ++i) {
	if (!plane.isFlat(mlfbs[i]) &&
		mlfbs[i].hasPoint(a) &&
		mlfbs[i].hasPoint(sumAB)) {
	  CHECK(mlfbA == 0);
	  mlfbA = &(mlfbs[i]);
	}
  }
  CHECK(mlfbA != 0);

  // ** find unique non-flat MLFB with {0,b,a+b}
  const Mlfb* mlfbB = 0;
  for (size_t i = 0; i < mlfbs.size(); ++i) {
	if (!plane.isFlat(mlfbs[i]) &&
		mlfbs[i].hasPoint(b) &&
		mlfbs[i].hasPoint(sumAB)) {
	  CHECK(mlfbB == 0);
	  mlfbB = &(mlfbs[i]);
	}
  }
  CHECK(mlfbB != 0);

  // ** both parts must be pivots
  CHECK(plane.isPivot(*mlfbA));
  CHECK(plane.isPivot(*mlfbB));

  // ** the two pivots must be on the left
  CHECK((mlfbA == pivots[0] && mlfbB == pivots[1]) ||
		(mlfbA == pivots[1] && mlfbB == pivots[0]));
}

const char* getEdgePos(size_t index) {
  switch (index) {
  case 0: return "sw";
  case 1: return "se";
  case 2: return "ne";
  case 3: return "nw";
  default: ASSERT(false);
	return "ERROR";
  }
}

mpq_class getIndexSum(const vector<Mlfb>& mlfbs) {
  mpq_class sum;
  for (size_t m = 0; m < mlfbs.size(); ++m)
	sum += mlfbs[m].index;
  return sum;
}

SeqPos::SeqPos() {}
SeqPos::SeqPos(const Mlfb* mlfb, size_t nextFacet, size_t previousFacet) {
  ASSERT(mlfb != 0);
  ASSERT(nextFacet != previousFacet);
  ASSERT(nextFacet < 4);
  ASSERT(previousFacet < 4);
  this->mlfb = mlfb;

  comingFromFacet = previousFacet;
  for (size_t f = 0; f < 4; ++f)
	if (f != previousFacet && f != nextFacet)
	  fixFacet1 = f;
  for (size_t f = 0; f < 4; ++f)
	if (f != previousFacet && f != nextFacet && f != fixFacet1)
	  fixFacet2 = f;
}

size_t SeqPos::getForwardFacet() const {
  for (size_t i = 0; ; ++i) {
	ASSERT(i < 4);
	if (i != fixFacet1 && i != fixFacet2 && i != comingFromFacet)
	  return i;
  }
}

size_t SeqPos::getBackFacet() const {
  return comingFromFacet;
}

SeqPos SeqPos::getReverse() const {
  size_t to;
  for (to = 0; to < 4; ++to) {
	if (to != fixFacet1 && to != fixFacet2 && to != comingFromFacet)
	  break;
  }
  ASSERT(to != 4);
  SeqPos reverse = *this;
  reverse.comingFromFacet = to;
  return reverse;
}

void SeqPos::order() {
  if (fixFacet1 > fixFacet2)
	swap(fixFacet1, fixFacet2);
}

bool SeqPos::operator<(const SeqPos& pos) const {
  if (mlfb->getOffset() < pos.mlfb->getOffset())
	return true;
  if (mlfb->getOffset() > pos.mlfb->getOffset())
	return false;

  if (fixFacet1 < pos.fixFacet1)
	return true;
  if (fixFacet1 > pos.fixFacet1)
	return false;

  if (fixFacet2 < pos.fixFacet2)
	return true;
  if (fixFacet2 > pos.fixFacet2)
	return false;

  if (comingFromFacet < pos.comingFromFacet)
	return true;
  if (comingFromFacet > pos.comingFromFacet)
	return false;

  return false;
}

Neighbor::Neighbor():
  _lat(0), _row(0) {
}

Neighbor::Neighbor(const GrobLat& lat):
  _lat(&lat), _row(lat.getNeighborCount() + 1) {
}

Neighbor::Neighbor(const GrobLat& lat, const size_t row):
  _lat(&lat), _row(row) {
}

const mpq_class& Neighbor::getH(size_t i) const {
  ASSERT(isValid());
  ASSERT(i < getHDim());
  if (isZero())
	return _lat->getZero();
  else
	return _lat->getHMatrix()(_row, i);
}

size_t Neighbor::getHDim() const {
  ASSERT(isValid());
  return _lat->getHDim();
} 

const mpq_class& Neighbor::getY(size_t i) const {
  ASSERT(isValid());
  ASSERT(i < getYDim());
  if (isZero())
	return _lat->getZero();
  else
	return _lat->getYMatrix()(_row, i);
}

size_t Neighbor::getYDim() const {
  ASSERT(isValid());
  return _lat->getYDim();
} 

bool Neighbor::isZero() const {
  ASSERT(isValid());
  return _row == _lat->getNeighborCount() + 1;
}

bool Neighbor::isValid() const {
  return _lat != 0;
}

size_t Plane::getTypeCount(size_t type) const {
  map<size_t, size_t>::const_iterator it = typeCounts.find(type);
  if (it == typeCounts.end())
	return 0u;
  else
	return it->second;
}

size_t Plane::getMaxType() const {
  if (typeCounts.empty())
	return 0u;
  else
	return typeCounts.rbegin()->first;
}

NeighborPlace Plane::getPlace(Neighbor neighbor) const {
  if (neighbor.isZero())
	return InPlane;
  ASSERT(neighbor.getRow() < neighborPlace.size());
  return neighborPlace[neighbor.getRow()];
}

bool Plane::inPlane(Neighbor neighbor) const {
  return getPlace(neighbor) == InPlane;
}

bool Plane::isPivot(const Mlfb& mlfb) const {
  const size_t type = getType(mlfb);
  return type == 1 || type == 3;
}

bool Plane::isFlat(const Mlfb& mlfb) const {
  return getType(mlfb) == 4;
}

size_t Plane::getType(const Mlfb& mlfb) const {
  size_t type = 0;
  for (size_t i = 0; i < mlfb.getPointCount(); ++i)
	if (inPlane(mlfb.getPoint(i)))
	  ++type;
  return type;
}

string Neighbor::getName() const {
  if (isZero())
	return "zero";
  if (!isValid())
	return "none";
  ostringstream name;
  name << 'g' << (getRow() + 1);
  return name.str();
}
