/*
*  File:        ptree.cpp
*  Description: Implementation of a partitioning tree class for CPSC 221 PA3
*  Date:        2022-03-03 01:53
*
*               ADD YOUR PRIVATE FUNCTION IMPLEMENTATIONS TO THE BOTTOM OF THIS FILE
*/

#include "ptree.h"
#include "hue_utils.h" // useful functions for calculating hue averages
#include <iostream>
#include <list>
using namespace cs221util;
using namespace std;

// The following definition may be convenient, but is not necessary to use
typedef pair<unsigned int, unsigned int> pairUI;

/////////////////////////////////
// PTree private member functions
/////////////////////////////////

/*
*  Destroys all dynamically allocated memory associated with the current PTree object.
*  You may want to add a recursive helper function for this!
*  POST: all nodes allocated into the heap have been released.
*/
void PTree::Clear() {
  // add your implementation below
  Clear(root);
}

//Clearing all the nodes from left to right
void PTree::Clear(Node* subroot){
  // if(subroot!=NULL){
  //   if(subroot->A ==NULL && subroot->B ==NULL){
  //     delete subroot;
  //   }
  //   else{
  //     Clear(subroot->A);
  //     Clear(subroot->B);
  //   }
  // }
  // else{
  //   return;
  // }
  if (subroot != NULL){
		Clear(subroot->A);
    Clear(subroot->B);
		delete subroot;
	}
}


Node* PTree::copyhelper(Node* root){
  Node* newRoot;
  if(root!=NULL){
    newRoot=new Node;
    newRoot->width=root->width;
    newRoot->height=root->height;
    newRoot->upperleft=root->upperleft;
    newRoot->avg=root->avg;
    newRoot->A=copyhelper(root->A);
    newRoot->B=copyhelper(root->B);
  } 
  else {return NULL;}
  return newRoot;
  
}

/*
*  Copies the parameter other PTree into the current PTree.
*  Does not free any memory. Should be called by copy constructor and operator=.
*  You may want a recursive helper function for this!
*  PARAM: other - the PTree which will be copied
*  PRE:   There is no dynamic memory associated with this PTree.
*  POST:  This PTree is a physically separate copy of the other PTree.
*/
void PTree::Copy(const PTree& other) {
  // add your implementation below
  root = Copy2(other.root);
}

Node* PTree::Copy2( Node* otherRoot) {
  // add your implementation below
  // if(otherRoot !=NULL){
  //   if(otherRoot->A ==NULL && otherRoot->B ==NULL){
  //     return;
  //   }
  //   if(otherRoot->A ==NULL){
  //     Node* xx = new Node(otherRoot->B);
  //     subroot->B = xx;
      
  //     Copy(otherRoot->B,subroot->B);
  //   }
  //   if(otherRoot->B ==NULL){
  //     Node* xx = new Node(otherRoot->A);
  //     subroot->A = xx;
      
  //     Copy(otherRoot->A,subroot->A);
  //   }
    
  // }
  // return;
  Node* tempNode = NULL;
	if (otherRoot != NULL){
		tempNode = new Node(otherRoot->upperleft, otherRoot->width, otherRoot->height, otherRoot->avg);
		tempNode->A = Copy2(otherRoot->A);
		tempNode->B = Copy2(otherRoot->B);
	}
	return tempNode;
}

/*
*  Private helper function for the constructor. Recursively builds the tree
*  according to the specification of the constructor.
*  You *may* change this if you like, but we have provided here what we
*  believe will be sufficient to use as-is.
*  PARAM:  im - full reference image used for construction
*  PARAM:  ul - upper-left image coordinate of the currently building Node's image region
*  PARAM:  w - width of the currently building Node's image region
*  PARAM:  h - height of the currently building Node's image region
*  RETURN: pointer to the fully constructed Node
*/
Node* PTree::BuildNode(PNG& im, pair<unsigned int, unsigned int> ul, unsigned int w, unsigned int h) {
  // replace the line below with your implementation
  //return nullptr;
  Node* newNode = new Node(ul,w ,h , average(im,ul,w,h), nullptr, nullptr);
  newNode->upperleft=ul;
  newNode->width=w;
  newNode->height=h;
  newNode->avg= average(im,ul,w,h);
  cout<< newNode->avg.h <<"\n";
  if(!(h==1 && w==1)){
    if(h>w){
      if(h%2==1){
        newNode->A=BuildNode(im,make_pair(ul.first,ul.second),w,(h-1)/2);
        newNode->B=BuildNode(im,make_pair(ul.first,ul.second+(h-1)/2),w,(h+1)/2);
        return newNode;
      }
      else{
        newNode->A=BuildNode(im,make_pair(ul.first,ul.second),w,h/2);
        newNode->B=BuildNode(im,make_pair(ul.first,ul.second+h/2),w,h/2);
        return newNode;
      }
    }
    else{
      if(w%2==1){
        newNode->A=BuildNode(im,make_pair(ul.first,ul.second),(w-1)/2,h);
        newNode->B=BuildNode(im,make_pair(ul.first+(w-1)/2,ul.second),(w+1)/2,h);
        return newNode;
      }
      else{
        newNode->A=BuildNode(im,make_pair(ul.first,ul.second),w/2,h);
        newNode->B=BuildNode(im,make_pair(ul.first+w/2,ul.second),w/2,h);
        return newNode;
      }
    }
  }
  else{
    newNode->A=NULL;
    newNode->B=NULL;
    return newNode;
  }
  
}

HSLAPixel PTree::average(PNG& image, pair<unsigned int, unsigned int> ul, unsigned int w, unsigned int h){
    double totalhX = 0, totalhY = 0,totals = 0,totall = 0,totala = 0;

    for (unsigned int x = ul.first; x < ul.first+w; x++) {
        for (unsigned int y = ul.second; y < ul.second+h; y++) {
            HSLAPixel *pixel = image.getPixel(x, y);
            totalhX = totalhX + Deg2X(pixel->h);
            totalhY = totalhY + Deg2Y(pixel->h);

            totals = totals + pixel->s;
            totall = totall + pixel->l;
            totala = totala + pixel->a;
        }
    }
    totalhX = totalhX/w;
    totalhY = totalhY/h;
    HSLAPixel temp = HSLAPixel( XY2Deg(totalhX,totalhY), totals/(w*h), totall/(w*h), totala/(w*h) );
    return temp;

}
////////////////////////////////
// PTree public member functions
////////////////////////////////

/*
*  Constructor that builds the PTree using the provided PNG.
*
*  The PTree represents the sub-image (actually the entire image) from (0,0) to (w-1, h-1) where
*  w-1 and h-1 are the largest valid image coordinates of the original PNG.
*  Each node corresponds to a rectangle of pixels in the original PNG, represented by
*  an (x,y) pair for the upper-left corner of the rectangle, and two unsigned integers for the
*  number of pixels on the width and height dimensions of the rectangular sub-image region the
*  node defines.
*
*  A node's two children correspond to a partition of the node's rectangular region into two
*  equal (or approximately equal) size regions which are either tiled horizontally or vertically.
*
*  If the rectangular region of a node is taller than it is wide, then its two children will divide
*  the region into vertically-tiled sub-regions of equal height:
*  +-------+
*  |   A   |
*  |       |
*  +-------+
*  |   B   |
*  |       |
*  +-------+
*
*  If the rectangular region of a node is wider than it is tall, OR if the region is exactly square,
*  then its two children will divide the region into horizontally-tiled sub-regions of equal width:
*  +-------+-------+
*  |   A   |   B   |
*  |       |       |
*  +-------+-------+
*
*  If any region cannot be divided exactly evenly (e.g. a horizontal division of odd width), then
*  child B will receive the larger half of the two subregions.
*
*  When the tree is fully constructed, each leaf corresponds to a single pixel in the PNG image.
* 
*  For the average colour, this MUST be computed separately over the node's rectangular region.
*  Do NOT simply compute this as a weighted average of the children's averages.
*  The functions defined in hue_utils.h and implemented in hue_utils.cpp will be very useful.
*  Computing the average over many overlapping rectangular regions sounds like it will be
*  inefficient, but as an exercise in theory, think about the asymptotic upper bound on the
*  number of times any given pixel is included in an average calculation.
* 
*  PARAM: im - reference image which will provide pixel data for the constructed tree's leaves
*  POST:  The newly constructed tree contains the PNG's pixel data in each leaf node.
*/
PTree::PTree(PNG& im) {
  // add your implementation below
  pair<int, int> ulxx (0, 0);
  root=BuildNode(im, ulxx, im.width(), im.height());
}

/*
*  Copy constructor
*  Builds a new tree as a copy of another tree.
*
*  PARAM: other - an existing PTree to be copied
*  POST:  This tree is constructed as a physically separate copy of other tree.
*/
PTree::PTree(const PTree& other) {
  // add your implementation below
  Copy(other);
}

void PTree::destoryhelper(Node* root){
  Node* n = root;
  if(n->A!=nullptr){
    destoryhelper(n->A);
  }
  if(n->B!=nullptr){
    destoryhelper(n->B);
  }
  delete n;
  root = nullptr;
}

/*
*  Assignment operator
*  Rebuilds this tree as a copy of another tree.
*
*  PARAM: other - an existing PTree to be copied
*  POST:  If other is a physically different tree in memory, all pre-existing dynamic
*           memory in this tree is deallocated and this tree is reconstructed as a
*           physically separate copy of other tree.
*         Otherwise, there is no change to this tree.
*/
PTree& PTree::operator=(const PTree& other) {
  // add your implementation below
 if(other.root!=root){
    destoryhelper(root);
    root=copyhelper(other.root);
    return *this;
  }
  else{
    return *this;
  }
  // Clear();
  // Copy(other);
  // return *this;
}

/*
*  Destructor
*  Deallocates all dynamic memory associated with the tree and destroys this PTree object.
*/
PTree::~PTree() {
  // add your implementation below
  destoryhelper(this->root);
}

/*
*  Traverses the tree and puts the leaf nodes' color data into the nodes'
*  defined image regions on the output PNG.
*  For non-pruned trees, each leaf node corresponds to a single pixel that will be coloured.
*  For pruned trees, each leaf node may cover a larger rectangular region that will be
*  entirely coloured using the node's average colour attribute.
*
*  You may want to add a recursive helper function for this!
*
*  RETURN: A PNG image of appropriate dimensions and coloured using the tree's leaf node colour data
*/
PNG PTree::Render() const {
  // replace the line below with your implementation
  PNG im(root->width, root->height);
	render(im, root);
  return im;
}

void PTree::render(PNG& im, Node* root) const {
	if(root != NULL){
		if(root->A == NULL) {
			for(unsigned int x = root->upperleft.first; x< root->upperleft.first+root->width; x++){
				for(unsigned int y = root->upperleft.second; y< root->upperleft.second+root->height;y++){
					*im.getPixel(x, y) = root->avg;
				}
			}
		} else {
			render(im, root->A);
			render(im, root->B);
		}
	}
}
/*
*  Trims subtrees as high as possible in the tree. A subtree is pruned
*  (its children are cleared/deallocated) if ALL of its leaves have colour
*  within tolerance of the subtree root's average colour.
*  Pruning criteria should be evaluated on the original tree, and never on a pruned
*  tree (i.e. we expect that Prune would be called on any tree at most once).
*  When processing a subtree, you should determine if the subtree should be pruned,
*  and prune it if possible before determining if it has subtrees that can be pruned.
* 
*  You may want to add (a) recursive helper function(s) for this!
*
*  PRE:  This tree has not been previously pruned (and is not copied/assigned from a tree that has been pruned)
*  POST: Any subtrees (as close to the root as possible) whose leaves all have colour
*        within tolerance from the subtree's root colour will have their children deallocated;
*        Each pruned subtree's root becomes a leaf node.
*/
void PTree::Prune(double tolerance) {
  // add your implementation below
  prune(tolerance, root);
}
void PTree::prune(double tolerance, Node* croot){
	if (croot == NULL || croot->A == NULL){
    return;
  } 
	if (prunable(tolerance, croot, croot->avg)){
		Clear(croot->A);
		Clear(croot->B);
		croot->A = NULL;
		croot->B = NULL;
	} else {
		prune(tolerance, croot->A);
		prune(tolerance, croot->B);
	}
}
bool PTree::prunable(double tolerance, Node * croot, HSLAPixel avg) {
	// if (croot->A == NULL)
	// 	return croot->avg.dist(avg) <= tolerance;
	// return prunable(tolerance, croot->A, avg) && prunable(tolerance, croot->B, avg);



  if (croot->A != NULL){
    return (prunable(tolerance, croot->A, avg) && prunable(tolerance, croot->B, avg));
  }
	return tolerance >= croot->avg.dist(avg);
}
/*
*  Returns the total number of nodes in the tree.
*  This function should run in time linearly proportional to the size of the tree.
*
*  You may want to add a recursive helper function for this!
*/
int PTree::Size() const {
  // replace the line below with your implementation
  return size(root);
}
int PTree::size(Node* croot) const {
	// if(croot == NULL) return 0;
	// return 1+size(croot->A)+size(croot->B);

  if(croot != NULL){
    return 1+size(croot->A)+size(croot->B);
  }
  return 0;


}
/*
*  Returns the total number of leaf nodes in the tree.
*  This function should run in time linearly proportional to the size of the tree.
*
*  You may want to add a recursive helper function for this!
*/
int PTree::NumLeaves() const {
  // replace the line below with your implementation
  return numLeaves(root);
}
int PTree::numLeaves(Node * root) const {
	// if(root->A == NULL) return 1;
	// return numLeaves(root->A)+numLeaves(root->B);

  if(root->A != NULL){
    int temp = numLeaves(root->A)+numLeaves(root->B);
    return temp;
  }
  return 1;
}
/*
*  Rearranges the nodes in the tree, such that a rendered PNG will be flipped horizontally
*  (i.e. mirrored over a vertical axis).
*  This can be achieved by manipulation of the nodes' member attribute(s).
*  Note that this may possibly be executed on a pruned tree.
*  This function should run in time linearly proportional to the size of the tree.
*
*  You may want to add a recursive helper function for this!
*
*  POST: Tree has been modified so that a rendered PNG will be flipped horizontally.
*/
void PTree::FlipHorizontal() {
  // add your implementation below
  flipHorizontal(root);
}

void PTree::flipHorizontal(Node * currroot) {
	if(currroot->A == NULL){
    return;
  }
	if(currroot->width >= currroot->height ){
		// swap A and B
		currroot->A->upperleft = pair<unsigned int, unsigned int>(
			currroot->upperleft.first + currroot->B->width,
			currroot->upperleft.second
		);
		currroot->B->upperleft = pair<unsigned int, unsigned int>(
			currroot->upperleft.first,
			currroot->upperleft.second
		);
	} else {
		currroot->A->upperleft = pair<unsigned int, unsigned int>(
			currroot->upperleft.first,
			currroot->A->upperleft.second
		);
		currroot->B->upperleft = pair<unsigned int, unsigned int>(
			currroot->upperleft.first,
			currroot->B->upperleft.second
		);
	}

	flipHorizontal(currroot->A);
	flipHorizontal(currroot->B);
}
/*
*  Like the function above, rearranges the nodes in the tree, such that a rendered PNG
*  will be flipped vertically (i.e. mirrored over a horizontal axis).
*  This can be achieved by manipulation of the nodes' member attribute(s).
*  Note that this may possibly be executed on a pruned tree.
*  This function should run in time linearly proportional to the size of the tree.
*
*  You may want to add a recursive helper function for this!
*
*  POST: Tree has been modified so that a rendered PNG will be flipped vertically.
*/
void PTree::FlipVertical() {
  // add your implementation below
  
}

/*
    *  Provides access to the root of the tree.
    *  Dangerous in practice! This is only used for testing.
    */
Node* PTree::GetRoot() {
  return root;
}

//////////////////////////////////////////////
// PERSONALLY DEFINED PRIVATE MEMBER FUNCTIONS
//////////////////////////////////////////////

