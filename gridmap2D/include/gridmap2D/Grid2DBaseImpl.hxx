/*
 * OctoMap - An Efficient Probabilistic 3D Mapping Framework Based on Octrees
 * http://octomap.github.com/
 *
 * Copyright (c) 2009-2013, K.M. Wurm and A. Hornung, University of Freiburg
 * All rights reserved.
 * License: New BSD
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 *     * Redistributions of source code must retain the above copyright
 *       notice, this list of conditions and the following disclaimer.
 *     * Redistributions in binary form must reproduce the above copyright
 *       notice, this list of conditions and the following disclaimer in the
 *       documentation and/or other materials provided with the distribution.
 *     * Neither the name of the University of Freiburg nor the names of its
 *       contributors may be used to endorse or promote products derived from
 *       this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */

#undef max
#undef min
#include <limits>

#ifdef _OPENMP
#include <omp.h>
#endif

namespace gridmap2D {

	template <class NODE, class I>
	Grid2DBaseImpl<NODE, I>::Grid2DBaseImpl(double resolution) :
		I(), gridmap(NULL), grid_max_val(32768),
		resolution(resolution)
	{
		init();
	}

	template <class NODE, class I>
	Grid2DBaseImpl<NODE, I>::Grid2DBaseImpl(double resolution, unsigned int grid_max_val) :
		I(), gridmap(NULL), grid_max_val(grid_max_val),
		resolution(resolution)
	{
		init();
	}


	template <class NODE, class I>
	Grid2DBaseImpl<NODE, I>::~Grid2DBaseImpl(){
		clear();
	}


	template <class NODE, class I>
	Grid2DBaseImpl<NODE, I>::Grid2DBaseImpl(const Grid2DBaseImpl<NODE, I>& rhs) :
		gridmap(NULL), grid_max_val(rhs.grid_max_val),
		resolution(rhs.resolution)
	{
		init();

		// Copy all of node
		if (rhs.gridmap){
			for (OccupancyGridMap::iterator it = rhs.gridmap->begin(); it != rhs.gridmap->end(); it++){
				gridmap->insert(std::pair<Grid2DKey, NODE*>(it->first, new NODE(*(it->second))));
			}
		}
	}

	template <class NODE, class I>
	void Grid2DBaseImpl<NODE, I>::init(){
		this->setResolution(this->resolution);
		for (unsigned i = 0; i < 2; i++){
			max_value[i] = -(std::numeric_limits<double>::max());
			min_value[i] = std::numeric_limits<double>::max();
		}
		size_changed = true;

		// create as many KeyRays as there are OMP_THREADS defined,
		// one buffer for each thread
#ifdef _OPENMP
#pragma omp parallel
#pragma omp critical
		{
			if (omp_get_thread_num() == 0){
				this->keyrays.resize(omp_get_num_threads());
			}

		}
#else
		this->keyrays.resize(1);
#endif

	}

	template <class NODE, class I>
	void Grid2DBaseImpl<NODE, I>::swapContent(Grid2DBaseImpl<NODE, I>& other){
		OccupancyGridMap* this_gridmap = gridamp;
		gridamp = other.gridamp;
		other.gridamp = this_gridmap;

		size_t this_size = this->tree_size;
		this->tree_size = other.tree_size;
		other.tree_size = this_size;
	}

	// Imlement operator==() - To do.
	/*template <class NODE, class I>
	bool Grid2DBaseImpl<NODE, I>::operator== (const Grid2DBaseImpl<NODE, I>& other) const{
		if (grid_max_val != other.grid_max_val || resolution != other.resolution){
			return false;
		}

		// traverse all nodes, check if structure the same
		Grid2DBaseImpl<NODE, I>::tree_iterator it = this->begin_tree();
		Grid2DBaseImpl<NODE, I>::tree_iterator end = this->end_tree();
		Grid2DBaseImpl<NODE, I>::tree_iterator other_it = other.begin_tree();
		Grid2DBaseImpl<NODE, I>::tree_iterator other_end = other.end_tree();

		for (; it != end; ++it, ++other_it){
			if (other_it == other_end)
				return false;

			if (it.getDepth() != other_it.getDepth()
				|| it.getKey() != other_it.getKey()
				|| !(*it == *other_it))
			{
				return false;
			}
		}

		if (other_it != other_end)
			return false;

		return true;
	}*/

	template <class NODE, class I>
	void Grid2DBaseImpl<NODE, I>::setResolution(double r) {
		resolution = r;
		resolution_factor = 1. / resolution;

		grid_center(0) = grid_center(1) = (float)(((double)grid_max_val) / resolution_factor);

		size_changed = true;
	}

	/*template <class NODE, class I>
	NODE* QuadTreeBaseImpl<NODE, I>::createNodeChild(NODE* node, unsigned int childIdx){
		assert(childIdx < 4);
		if (node->children == NULL) {
			allocNodeChildren(node);
		}
		assert(node->children[childIdx] == NULL);
		NODE* newNode = new NODE();
		node->children[childIdx] = static_cast<AbstractQuadTreeNode*>(newNode);

		tree_size++;
		size_changed = true;

		return newNode;
	}*/

	/*template <class NODE, class I>
	void QuadTreeBaseImpl<NODE, I>::deleteNodeChild(NODE* node, unsigned int childIdx){
		assert((childIdx < 4) && (node->children != NULL));
		assert(node->children[childIdx] != NULL);
		delete static_cast<NODE*>(node->children[childIdx]); // TODO delete check if empty
		node->children[childIdx] = NULL;

		tree_size--;
		size_changed = true;
	}*/

	/*template <class NODE, class I>
	NODE* QuadTreeBaseImpl<NODE, I>::getNodeChild(NODE* node, unsigned int childIdx) const{
		assert((childIdx < 4) && (node->children != NULL));
		assert(node->children[childIdx] != NULL);
		return static_cast<NODE*>(node->children[childIdx]);
	}*/

	/*template <class NODE, class I>
	const NODE* QuadTreeBaseImpl<NODE, I>::getNodeChild(const NODE* node, unsigned int childIdx) const{
		assert((childIdx < 4) && (node->children != NULL));
		assert(node->children[childIdx] != NULL);
		return static_cast<const NODE*>(node->children[childIdx]);
	}*/

	/*template <class NODE, class I>
	bool QuadTreeBaseImpl<NODE, I>::isNodeCollapsible(const NODE* node) const{
		// all children must exist, must not have children of
		// their own and have the same occupancy probability
		if (!nodeChildExists(node, 0))
			return false;

		const NODE* firstChild = getNodeChild(node, 0);
		if (nodeHasChildren(firstChild))
			return false;

		for (unsigned int i = 1; i < 4; i++) {
			// comparison via getChild so that casts of derived classes ensure
			// that the right == operator gets called
			if (!nodeChildExists(node, i) || nodeHasChildren(getNodeChild(node, i)) || !(*(getNodeChild(node, i)) == *(firstChild)))
				return false;
		}

		return true;
	}*/

	/*template <class NODE, class I>
	bool QuadTreeBaseImpl<NODE, I>::nodeChildExists(const NODE* node, unsigned int childIdx) const{
		assert(childIdx < 4);
		if ((node->children != NULL) && (node->children[childIdx] != NULL))
			return true;
		else
			return false;
	}*/

	/*template <class NODE, class I>
	bool QuadTreeBaseImpl<NODE, I>::nodeHasChildren(const NODE* node) const {
		if (node->children == NULL)
			return false;

		for (unsigned int i = 0; i < 4; i++){
			if (node->children[i] != NULL)
				return true;
		}
		return false;
	}*/


	/*template <class NODE, class I>
	void QuadTreeBaseImpl<NODE, I>::expandNode(NODE* node){
		assert(!nodeHasChildren(node));

		for (unsigned int k = 0; k < 4; k++) {
			NODE* newNode = createNodeChild(node, k);
			newNode->copyData(*node);
		}
	}*/

	/*template <class NODE, class I>
	bool QuadTreeBaseImpl<NODE, I>::pruneNode(NODE* node){

		if (!isNodeCollapsible(node))
			return false;

		// set value to children's values (all assumed equal)
		node->copyData(*(getNodeChild(node, 0)));

		// delete children (known to be leafs at this point!)
		for (unsigned int i = 0; i < 4; i++) {
			deleteNodeChild(node, i);
		}
		delete[] node->children;
		node->children = NULL;

		return true;
	}*/

	/*template <class NODE, class I>
	void QuadTreeBaseImpl<NODE, I>::allocNodeChildren(NODE* node){
		// TODO NODE*
		node->children = new AbstractQuadTreeNode*[4];
		for (unsigned int i = 0; i < 4; i++) {
			node->children[i] = NULL;
		}
	}*/



	/*template <class NODE, class I>
	inline key_type Grid2DBaseImpl<NODE, I>::coordToKey(double coordinate, unsigned depth) const{
		assert(depth <= tree_depth);
		int keyval = ((int)floor(resolution_factor * coordinate));

		unsigned int diff = tree_depth - depth;
		if (!diff) // same as coordToKey without depth
			return keyval + grid_max_val;
		else // shift right and left => erase last bits. Then add offset.
			return ((keyval >> diff) << diff) + (1 << (diff - 1)) + grid_max_val;
	}*/


	template <class NODE, class I>
	bool Grid2DBaseImpl<NODE, I>::coordToKeyChecked(double coordinate, key_type& keyval) const {

		// scale to resolution and shift center for grid_max_val
		int scaled_coord = ((int)floor(resolution_factor * coordinate)) + grid_max_val;

		// keyval within range of tree?
		if ((scaled_coord >= 0) && (((unsigned int)scaled_coord) < (2 * grid_max_val))) {
			keyval = scaled_coord;
			return true;
		}
		return false;
	}


	/*template <class NODE, class I>
	bool Grid2DBaseImpl<NODE, I>::coordToKeyChecked(double coordinate, unsigned depth, key_type& keyval) const {

		// scale to resolution and shift center for grid_max_val
		int scaled_coord = ((int)floor(resolution_factor * coordinate)) + grid_max_val;

		// keyval within range of tree?
		if ((scaled_coord >= 0) && (((unsigned int)scaled_coord) < (2 * grid_max_val))) {
			keyval = scaled_coord;
			keyval = adjustKeyAtDepth(keyval, depth);
			return true;
		}
		return false;
	}*/

	template <class NODE, class I>
	bool Grid2DBaseImpl<NODE, I>::coordToKeyChecked(const point2d& point, Grid2DKey& key) const{
		for (unsigned int i = 0; i < 2; i++) {
			if (!coordToKeyChecked(point(i), key[i])) return false;
		}
		return true;
	}

	/*template <class NODE, class I>
	bool QuadTreeBaseImpl<NODE, I>::coordToKeyChecked(const point2d& point, unsigned depth, QuadTreeKey& key) const{

		for (unsigned int i = 0; i < 2; i++) {
			if (!coordToKeyChecked(point(i), depth, key[i])) return false;
		}
		return true;
	}*/

	template <class NODE, class I>
	bool Grid2DBaseImpl<NODE, I>::coordToKeyChecked(double x, double y, Grid2DKey& key) const{
		if (!(coordToKeyChecked(x, key[0]) && coordToKeyChecked(y, key[1])))
		{
			return false;
		}
		else {
			return true;
		}
	}

	/*template <class NODE, class I>
	bool QuadTreeBaseImpl<NODE, I>::coordToKeyChecked(double x, double y, unsigned depth, QuadTreeKey& key) const{

		if (!(coordToKeyChecked(x, depth, key[0])
			&& coordToKeyChecked(y, depth, key[1])))
		{
			return false;
		}
		else {
			return true;
		}
	}*/

	/*template <class NODE, class I>
	key_type QuadTreeBaseImpl<NODE, I>::adjustKeyAtDepth(key_type key, unsigned int depth) const{
		unsigned int diff = tree_depth - depth;

		if (diff == 0)
			return key;
		else
			return (((key - grid_max_val) >> diff) << diff) + (1 << (diff - 1)) + grid_max_val;
	}*/

	/*template <class NODE, class I>
	double QuadTreeBaseImpl<NODE, I>::keyToCoord(key_type key, unsigned depth) const{
		assert(depth <= tree_depth);

		// root is centered on 0 = 0.0
		if (depth == 0) {
			return 0.0;
		}
		else if (depth == tree_depth) {
			return keyToCoord(key);
		}
		else {
			return (floor((double(key) - double(this->grid_max_val)) / double(1 << (tree_depth - depth))) + 0.5) * this->getNodeSize(depth);
		}
	}*/

	template <class NODE, class I>
	NODE* Grid2DBaseImpl<NODE, I>::search(const point2d& value) const {
		Grid2DKey key;
		if (!coordToKeyChecked(value, key)){
			GRIDMAP2D_ERROR_STR("Error in search: [" << value << "] is out of Grid2D bounds!");
			return NULL;
		}
		else {
			return this->search(key);
		}

	}

	template <class NODE, class I>
	NODE* Grid2DBaseImpl<NODE, I>::search(double x, double y) const {
		Grid2DKey key;
		if (!coordToKeyChecked(x, y, key)){
			GRIDMAP2D_ERROR_STR("Error in search: [" << x << " " << y << "] is out of Grid2D bounds!");
			return NULL;
		}
		else {
			return this->search(key);
		}
	}


	template <class NODE, class I>
	NODE* Grid2DBaseImpl<NODE, I>::search(const Grid2DKey& key) const {
		if (gridmap == NULL)
			return NULL;

		OccupancyGridMap::iterator cell = gridmap->find(key);
		if (cell == gridmap->end())
			return NULL;
		return cell->second;
	}

	template <class NODE, class I>
	bool Grid2DBaseImpl<NODE, I>::deleteNode(const point2d& value) {
		Grid2DKey key;
		if (!coordToKeyChecked(value, key)){
			GRIDMAP2D_ERROR_STR("Error in deleteNode: [" << value << "] is out of Grid2D bounds!");
			return false;
		}
		else {
			return this->deleteNode(key);
		}
	}

	template <class NODE, class I>
	bool Grid2DBaseImpl<NODE, I>::deleteNode(double x, double y) {
		Grid2DKey key;
		if (!coordToKeyChecked(x, y, key)){
			GRIDMAP2D_ERROR_STR("Error in deleteNode: [" << x << " " << y << "] is out of Grid2D bounds!");
			return false;
		}
		else {
			return this->deleteNode(key);
		}
	}


	template <class NODE, class I>
	bool Grid2DBaseImpl<NODE, I>::deleteNode(const Grid2DKey& key) {
		if (gridmap == NULL)
			return true;

		OccupancyGridMap::iterator cell = gridmap->find(key);
		if (cell == gridmap->end())
			return false;

		gridmap->erase(cell);

		return true;
	}

	template <class NODE, class I>
	void Grid2DBaseImpl<NODE, I>::clear() {
		if (this->gridmap){
			delete this->gridmap;
			// this->tree_size = 0;
			this->gridmap = NULL;
			// max extent of tree changed:
			this->size_changed = true;
		}
	}

	/*template <class NODE, class I>
	void QuadTreeBaseImpl<NODE, I>::prune() {
		if (root == NULL)
			return;

		for (unsigned int depth = tree_depth - 1; depth > 0; --depth) {
			unsigned int num_pruned = 0;
			pruneRecurs(this->root, 0, depth, num_pruned);
			if (num_pruned == 0)
				break;
		}
	}*/

	/*template <class NODE, class I>
	void QuadTreeBaseImpl<NODE, I>::expand() {
		if (root)
			expandRecurs(root, 0, tree_depth);
	}*/

	template <class NODE, class I>
	bool Grid2DBaseImpl<NODE, I>::computeRayKeys(const point2d& origin, const point2d& end, KeyRay& ray) const {

		// see "A Faster Voxel Traversal Algorithm for Ray Tracing" by Amanatides & Woo
		// basically: DDA in 3D

		ray.reset();

		Grid2DKey key_origin, key_end;
		if (!Grid2DBaseImpl<NODE, I>::coordToKeyChecked(origin, key_origin) ||
			!Grid2DBaseImpl<NODE, I>::coordToKeyChecked(end, key_end)) {
			GRIDMAP2D_WARNING_STR("coordinates ( " << origin << " -> " << end << ") out of bounds in computeRayKeys");
			return false;
		}


		if (key_origin == key_end)
			return true; // same tree cell, we're done.

		ray.addKey(key_origin);

		// Initialization phase -------------------------------------------------------

		point2d direction = (end - origin);
		float length = (float)direction.norm();
		direction /= length; // normalize vector

		int    step[2];
		double tMax[2];
		double tDelta[2];

		Grid2DKey current_key = key_origin;

		for (unsigned int i = 0; i < 2; ++i) {
			// compute step direction
			if (direction(i) > 0.0) step[i] = 1;
			else if (direction(i) < 0.0)   step[i] = -1;
			else step[i] = 0;

			// compute tMax, tDelta
			if (step[i] != 0) {
				// corner point of voxel (in direction of ray)
				double voxelBorder = this->keyToCoord(current_key[i]);
				voxelBorder += (float)(step[i] * this->resolution * 0.5);

				tMax[i] = (voxelBorder - origin(i)) / direction(i);
				tDelta[i] = this->resolution / fabs(direction(i));
			}
			else {
				tMax[i] = std::numeric_limits<double>::max();
				tDelta[i] = std::numeric_limits<double>::max();
			}
		}

		// Incremental phase  ---------------------------------------------------------

		bool done = false;
		while (!done) {

			unsigned int dim;

			// find minimum tMax:
			// dim = tMax[0] < tMax[1] ? 0 : 1;
			if (tMax[0] < tMax[1]){
				dim = 0;
			}
			else {
				dim = 1;
			}

			// advance in direction "dim"
			current_key[dim] += step[dim];
			tMax[dim] += tDelta[dim];

			assert(current_key[dim] < 2 * this->grid_max_val);

			// reached endpoint, key equv?
			if (current_key == key_end) {
				done = true;
				break;
			}
			else {

				// reached endpoint world coords?
				// dist_from_origin now contains the length of the ray when traveled until the border of the current voxel
				double dist_from_origin = std::min(tMax[0], tMax[1]);
				// if this is longer than the expected ray length, we should have already hit the voxel containing the end point with the code above (key_end).
				// However, we did not hit it due to accumulating discretization errors, so this is the point here to stop the ray as we would never reach the voxel key_end
				if (dist_from_origin > length) {
					done = true;
					break;
				}

				else {  // continue to add freespace cells
					ray.addKey(current_key);
				}
			}

			assert(ray.size() < ray.sizeMax() - 1);

		} // end while

		return true;
	}

	template <class NODE, class I>
	bool Grid2DBaseImpl<NODE, I>::computeRay(const point2d& origin, const point2d& end, std::vector<point2d>& _ray) {
		_ray.clear();
		if (!computeRayKeys(origin, end, keyrays.at(0))) return false;
		for (KeyRay::const_iterator it = keyrays[0].begin(); it != keyrays[0].end(); ++it) {
			_ray.push_back(keyToCoord(*it));
		}
		return true;
	}

	/*template <class NODE, class I>
	void Grid2DBaseImpl<NODE, I>::deleteNodeRecurs(NODE* node){
		assert(node);
		// TODO: maintain tree size?

		if (node->children != NULL) {
			for (unsigned int i = 0; i < 4; i++) {
				if (node->children[i] != NULL){
					this->deleteNodeRecurs(static_cast<NODE*>(node->children[i]));
				}
			}
			delete[] node->children;
			node->children = NULL;
		} // else: node has no children

		delete node;
	}*/


	/*template <class NODE, class I>
	bool QuadTreeBaseImpl<NODE, I>::deleteNodeRecurs(NODE* node, unsigned int depth, unsigned int max_depth, const QuadTreeKey& key){
		if (depth >= max_depth) // on last level: delete child when going up
			return true;

		assert(node);

		unsigned int pos = computeChildIdx(key, this->tree_depth - 1 - depth);

		if (!nodeChildExists(node, pos)) {
			// child does not exist, but maybe it's a pruned node?
			if ((!nodeHasChildren(node)) && (node != this->root)) { // TODO double check for exists / root exception?
				// current node does not have children AND it's not the root node
				// -> expand pruned node
				expandNode(node);
				// tree_size and size_changed adjusted in createNodeChild(...)
			}
			else { // no branch here, node does not exist
				return false;
			}
		}

		// follow down further, fix inner nodes on way back up
		bool deleteChild = deleteNodeRecurs(getNodeChild(node, pos), depth + 1, max_depth, key);
		if (deleteChild){
			// TODO: lazy eval?
			// TODO delete check depth, what happens to inner nodes with children?
			this->deleteNodeChild(node, pos);

			if (!nodeHasChildren(node))
				return true;
			else{
				node->updateOccupancyChildren(); // TODO: occupancy?
			}
		}
		// node did not lose a child, or still has other children
		return false;
	}*/

	/*template <class NODE, class I>
	void QuadTreeBaseImpl<NODE, I>::pruneRecurs(NODE* node, unsigned int depth,
		unsigned int max_depth, unsigned int& num_pruned) {

		assert(node);

		if (depth < max_depth) {
			for (unsigned int i = 0; i < 4; i++) {
				if (nodeChildExists(node, i)) {
					pruneRecurs(getNodeChild(node, i), depth + 1, max_depth, num_pruned);
				}
			}
		} // end if depth

		else {
			// max level reached
			if (pruneNode(node)) {
				num_pruned++;
			}
		}
	}*/


	/*template <class NODE, class I>
	void QuadTreeBaseImpl<NODE, I>::expandRecurs(NODE* node, unsigned int depth,
		unsigned int max_depth) {
		if (depth >= max_depth)
			return;

		assert(node);

		// current node has no children => can be expanded
		if (!nodeHasChildren(node)){
			expandNode(node);
		}
		// recursively expand children
		for (unsigned int i = 0; i < 4; i++) {
			if (nodeChildExists(node, i)) { // TODO double check (node != NULL)
				expandRecurs(getNodeChild(node, i), depth + 1, max_depth);
			}
		}
	}*/


	template <class NODE, class I>
	std::ostream& Grid2DBaseImpl<NODE, I>::writeData(std::ostream &s) const{
		// Implement writeData() - To do.
		/*if (root)
			writeNodesRecurs(root, s);*/

		return s;
	}

	/*template <class NODE, class I>
	std::ostream& Grid2DBaseImpl<NODE, I>::writeNodesRecurs(const NODE* node, std::ostream &s) const{
		node->writeData(s);

		// 1 bit for each children; 0: empty, 1: allocated
		std::bitset<4> children;
		for (unsigned int i = 0; i < 4; i++) {
			if (nodeChildExists(node, i))
				children[i] = 1;
			else
				children[i] = 0;
		}

		char children_char = (char)children.to_ulong();
		s.write((char*)&children_char, sizeof(char));

		//     std::cout << "wrote: " << value << " "
		//               << children.to_string<char,std::char_traits<char>,std::allocator<char> >() 
		//               << std::endl;

		// recursively write children
		for (unsigned int i = 0; i < 4; i++) {
			if (children[i] == 1) {
				this->writeNodesRecurs(getNodeChild(node, i), s);
			}
		}

		return s;
	}*/

	template <class NODE, class I>
	std::istream& Grid2DBaseImpl<NODE, I>::readData(std::istream &s) {
		// Implement readData() - To do.
		/*if (!s.good()){
			GRIDMAP2D_WARNING_STR(__FILE__ << ":" << __LINE__ << "Warning: Input filestream not \"good\"");
		}

		this->tree_size = 0;
		size_changed = true;

		// tree needs to be newly created or cleared externally
		if (root) {
			QUADMAP_ERROR_STR("Trying to read into an existing tree.");
			return s;
		}

		root = new NODE();
		readNodesRecurs(root, s);

		tree_size = calcNumNodes();  // compute number of nodes*/
		return s;
	}

	/*template <class NODE, class I>
	std::istream& Grid2DBaseImpl<NODE, I>::readNodesRecurs(NODE* node, std::istream &s) {

		node->readData(s);

		char children_char;
		s.read((char*)&children_char, sizeof(char));
		std::bitset<4> children((unsigned long long) children_char);

		//std::cout << "read: " << node->getValue() << " "
		//            << children.to_string<char,std::char_traits<char>,std::allocator<char> >()
		//            << std::endl;

		for (unsigned int i = 0; i < 4; i++) {
			if (children[i] == 1){
				NODE* newNode = createNodeChild(node, i);
				readNodesRecurs(newNode, s);
			}
		}

		return s;
	}*/

	// non-const versions, 
	// change min/max/size_changed members

	// Implement getMetricSize() - To do.
	template <class NODE, class I>
	void Grid2DBaseImpl<NODE, I>::getMetricSize(double& x, double& y){

		double minX, minY;
		double maxX, maxY;

		getMetricMax(maxX, maxY);
		getMetricMin(minX, minY);

		x = maxX - minX;
		y = maxY - minY;
	}

	// Implement getMetricSize() - To do.
	template <class NODE, class I>
	void Grid2DBaseImpl<NODE, I>::getMetricSize(double& x, double& y) const{

		double minX, minY;
		double maxX, maxY;

		getMetricMax(maxX, maxY);
		getMetricMin(minX, minY);

		x = maxX - minX;
		y = maxY - minY;
	}

	template <class NODE, class I>
	void Grid2DBaseImpl<NODE, I>::calcMinMax() {
		if (!size_changed)
			return;

		// empty tree
		if (gridmap == NULL){
			min_value[0] = min_value[1] = 0.0;
			max_value[0] = max_value[1] = 0.0;
			size_changed = false;
			return;
		}

		for (unsigned i = 0; i < 2; i++){
			max_value[i] = -std::numeric_limits<double>::max();
			min_value[i] = std::numeric_limits<double>::max();
		}

		for (typename Grid2DBaseImpl<NODE, I>::OccupancyGridMap::iterator it = this->gridmap->begin(), end = this->gridmap->end(); it != end; ++it)
		{
			double size = this->resolution;	// is it correct?
			double halfSize = size / 2.0;
			double x = keyToCoord(it->first[0]) - halfSize;
			double y = keyToCoord(it->first[1]) - halfSize;
			if (x < min_value[0]) min_value[0] = x;
			if (y < min_value[1]) min_value[1] = y;

			x += size;
			y += size;
			if (x > max_value[0]) max_value[0] = x;
			if (y > max_value[1]) max_value[1] = y;

		}

		size_changed = false;
	}

	// Implement getMetricMin() - To do.
	template <class NODE, class I>
	void Grid2DBaseImpl<NODE, I>::getMetricMin(double& x, double& y){
		calcMinMax();
		x = min_value[0];
		y = min_value[1];
	}

	// Implement getMetricMax() - To do.
	template <class NODE, class I>
	void Grid2DBaseImpl<NODE, I>::getMetricMax(double& x, double& y){
		calcMinMax();
		x = max_value[0];
		y = max_value[1];
	}

	// const versions
	// Implement getMetricMin() - To do.
	template <class NODE, class I>
	void Grid2DBaseImpl<NODE, I>::getMetricMin(double& mx, double& my) const {
		mx = my = std::numeric_limits<double>::max();
		if (size_changed) {
			// empty tree
			if (gridmap == NULL || gridmap->size() == 0){
				mx = my = 0.0;
				return;
			}

			for (typename Grid2DBaseImpl<NODE, I>::OccupancyGridMap::iterator it = this->gridmap->begin(), end = this->gridmap->end(); it != end; ++it) {
				double halfSize = this->resolution / 2.0;	// is it correct?s
				double x = keyToCoord(it->first[0]) - halfSize;
				double y = keyToCoord(it->first[1]) - halfSize;
				if (x < mx) mx = x;
				if (y < my) my = y;
			}
		} // end if size changed 
		else {
			mx = min_value[0];
			my = min_value[1];
		}
	}

	// Implement getMetricMax() - To do.
	template <class NODE, class I>
	void Grid2DBaseImpl<NODE, I>::getMetricMax(double& mx, double& my) const {
		mx = my = -std::numeric_limits<double>::max();
		if (size_changed) {
			// empty tree
			if (gridmap == NULL || gridmap->size() == 0){
				mx = my = 0.0;
				return;
			}

			for (typename Grid2DBaseImpl<NODE, I>::OccupancyGridMap::iterator it = this->gridmap->begin(), end = this->gridmap->end(); it != end; ++it) {
				double halfSize = this->resolution / 2.0;	// is it correct?
				double x = keyToCoord(it->first[0]) + halfSize;
				double y = keyToCoord(it->first[1]) + halfSize;
				if (x > mx) mx = x;
				if (y > my) my = y;
			}
		}
		else {
			mx = max_value[0];
			my = max_value[1];
		}
	}

	template <class NODE, class I>
	size_t Grid2DBaseImpl<NODE, I>::memoryUsage() const{
		return (sizeof(Grid2DBaseImpl<NODE, I>) + memoryUsageNode() * this->size());	// Add HashTable?
	}

	// Implement getUnknownLeafCenters - To do.
	/*template <class NODE, class I>
	void Grid2DBaseImpl<NODE, I>::getUnknownLeafCenters(point2d_list& node_centers, point2d pmin, point2d pmax, unsigned int depth) const {

		assert(depth <= tree_depth);
		if (depth == 0)
			depth = tree_depth;

		float diff[2];
		unsigned int steps[2];
		float step_size = this->resolution * pow(2, tree_depth - depth);
		for (int i = 0; i < 2; ++i) {
			diff[i] = pmax(i) - pmin(i);
			steps[i] = floor(diff[i] / step_size);
			//      std::cout << "bbx " << i << " size: " << diff[i] << " " << steps[i] << " steps\n";
		}

		point2d p = pmin;
		NODE* res;
		for (unsigned int x = 0; x < steps[0]; ++x) {
			p.x() += step_size;
			for (unsigned int y = 0; y < steps[1]; ++y) {
				p.y() += step_size;
				res = this->search(p, depth);
				if (res == NULL) {
					node_centers.push_back(p);
				}
			}
			p.y() = pmin.y();
		}
	}*/


	/*template <class NODE, class I>
	size_t Grid2DBaseImpl<NODE, I>::getNumLeafNodes() const {
		if (root == NULL)
			return 0;

		return getNumLeafNodesRecurs(root);
	}*/


	/*template <class NODE, class I>
	size_t Grid2DBaseImpl<NODE, I>::getNumLeafNodesRecurs(const NODE* parent) const {
		assert(parent);

		if (!nodeHasChildren(parent)) // this is a leaf -> terminate
			return 1;

		size_t sum_leafs_children = 0;
		for (unsigned int i = 0; i < 4; ++i) {
			if (nodeChildExists(parent, i)) {
				sum_leafs_children += getNumLeafNodesRecurs(getNodeChild(parent, i));
			}
		}
		return sum_leafs_children;
	}*/

	// Implement volume() - To do.
	template <class NODE, class I>
	double Grid2DBaseImpl<NODE, I>::volume() {
		double x, y;
		getMetricSize(x, y);
		return x*y;
	}


}